/*	$NetBSD: dk.c,v 1.1 2004/09/25 03:30:44 thorpej Exp $	*/

/*-
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD");

#include "opt_dkwedge.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/pool.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>
#include <sys/disk.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/conf.h>
#include <sys/callout.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>

#include <miscfs/specfs/specdev.h>

MALLOC_DEFINE(M_DKWEDGE, "dkwedge", "Disk wedge structures");

typedef enum {
	DKW_STATE_LARVAL	= 0,
	DKW_STATE_RUNNING	= 1,
	DKW_STATE_DYING		= 2,
	DKW_STATE_DEAD		= 666
} dkwedge_state_t;

struct dkwedge_softc {
	char		sc_devname[16];	/* device-style name (e.g. "dk0") */
	uint8_t		sc_wname[128];	/* wedge name (Unicode, UTF-8) */
	u_int		sc_unit;	/* our unit # */

	dkwedge_state_t sc_state;	/* state this wedge is in */

	struct disk	*sc_parent;	/* parent disk */
	daddr_t		sc_offset;	/* LBA offset of wedge in parent */
	uint64_t	sc_size;	/* size of wedge in blocks */
	char		sc_ptype[32];	/* partition type */
	dev_t		sc_pdev;	/* cached parent's dev_t */
					/* link on parent's wedge list */
	LIST_ENTRY(dkwedge_softc) sc_plink;

	int		sc_open;	/* locked by parent's rawlock */

	struct disk	sc_dk;		/* our own disk structure */
	struct bufq_state sc_bufq;	/* buffer queue */
	struct callout	sc_restart_ch;	/* callout to restart I/O */

	u_int		sc_iopend;	/* I/Os pending */
	int		sc_flags;	/* flags (splbio) */
};

#define	DK_F_WAIT_DRAIN		0x0001	/* waiting for I/O to drain */

static void	dkstart(struct dkwedge_softc *);
static void	dkiodone(struct buf *);
static void	dkrestart(void *);

static dev_type_open(dkopen);
static dev_type_close(dkclose);
static dev_type_read(dkread);
static dev_type_write(dkwrite);
static dev_type_ioctl(dkioctl);
static dev_type_strategy(dkstrategy);
static dev_type_dump(dkdump);
static dev_type_size(dksize);

const struct bdevsw dk_bdevsw = {
	dkopen, dkclose, dkstrategy, dkioctl, dkdump, dksize, D_DISK
};

const struct cdevsw dk_cdevsw = {
	dkopen, dkclose, dkread, dkwrite, dkioctl,
	    nostop, notty, nopoll, nommap, nokqfilter, D_DISK
};

static struct dkwedge_softc **dkwedges;
static u_int ndkwedges;
static struct lock dkwedges_lock = LOCK_INITIALIZER(PRIBIO, "dkwgs", 0, 0);

/*
 * dkwedge_wait_drain:
 *
 *	Wait for I/O on the wedge to drain.
 *	NOTE: Must be called at splbio()!
 */
static void
dkwedge_wait_drain(struct dkwedge_softc *sc)
{

	while (sc->sc_iopend != 0) {
		sc->sc_flags |= DK_F_WAIT_DRAIN;
		(void) tsleep(&sc->sc_iopend, PRIBIO, "dkdrn", 0);
	}
}

/*
 * dkwedge_compute_pdev:
 *
 *	Compute the parent disk's dev_t.
 */
static int
dkwedge_compute_pdev(const char *pname, dev_t *pdevp)
{
	const char *name, *cp;
	int punit, pmaj;
	char devname[16];

	name = pname;
	if ((pmaj = devsw_name2blk(name, devname, sizeof(devname))) == -1)
		return (ENODEV);
	
	name += strlen(devname);
	for (cp = name, punit = 0; *cp >= '0' && *cp <= '9'; cp++)
		punit = (punit * 10) + (*cp - '0');
	if (cp == name) {
		/* Invalid parent disk name. */
		return (ENODEV);
	}

	*pdevp = MAKEDISKDEV(pmaj, punit, RAW_PART);

	return (0);
}

/*
 * dkwedge_array_expand:
 *
 *	Expand the dkwedges array.
 */
static void
dkwedge_array_expand(void)
{
	int newcnt = ndkwedges + 16;
	struct dkwedge_softc **newarray, **oldarray;

	newarray = malloc(newcnt * sizeof(*newarray), M_DKWEDGE,
	    M_WAITOK|M_ZERO);
	if ((oldarray = dkwedges) != NULL)
		memcpy(newarray, dkwedges, ndkwedges * sizeof(*newarray));
	dkwedges = newarray;
	ndkwedges = newcnt;
	if (oldarray != NULL)
		free(oldarray, M_DKWEDGE);
}

/*
 * dkwedge_add:		[exported function]
 *
 *	Add a disk wedge based on the provided information.
 *
 *	The incoming dkw_devname[] is ignored, instead being
 *	filled in and returned to the caller.
 */
int
dkwedge_add(struct dkwedge_info *dkw)
{
	struct dkwedge_softc *sc, *lsc;
	struct disk *pdk;
	u_int unit;
	int error;
	dev_t pdev;

	dkw->dkw_parent[sizeof(dkw->dkw_parent) - 1] = '\0';
	pdk = disk_find(dkw->dkw_parent);
	if (pdk == NULL)
		return (ENODEV);

	error = dkwedge_compute_pdev(pdk->dk_name, &pdev);
	if (error)
		return (error);

	if (dkw->dkw_offset < 0)
		return (EINVAL);

	sc = malloc(sizeof(*sc), M_DKWEDGE, M_WAITOK|M_ZERO);
	sc->sc_state = DKW_STATE_LARVAL;
	sc->sc_parent = pdk;
	sc->sc_pdev = pdev;
	sc->sc_offset = dkw->dkw_offset;
	sc->sc_size = dkw->dkw_size;

	memcpy(sc->sc_wname, dkw->dkw_wname, sizeof(sc->sc_wname));
	sc->sc_wname[sizeof(sc->sc_wname) - 1] = '\0';

	memcpy(sc->sc_ptype, dkw->dkw_ptype, sizeof(sc->sc_ptype));
	sc->sc_ptype[sizeof(sc->sc_ptype) - 1] = '\0';

	bufq_alloc(&sc->sc_bufq,
	    BUFQ_DISK_DEFAULT_STRAT()|BUFQ_SORT_RAWBLOCK);

	callout_init(&sc->sc_restart_ch);
	callout_setfunc(&sc->sc_restart_ch, dkrestart, sc);

	/*
	 * Wedge will be added; increment the wedge count for the parent.
	 * Only allow this to happend if RAW_PART is the only thing open.
	 */
	(void) lockmgr(&pdk->dk_openlock, LK_EXCLUSIVE, NULL);
	if (pdk->dk_openmask & ~(1 << RAW_PART))
		error = EBUSY;
	else {
		/* Check for wedge overlap. */
		LIST_FOREACH(lsc, &pdk->dk_wedges, sc_plink) {
			daddr_t lastblk = sc->sc_offset + sc->sc_size - 1;
			daddr_t llastblk = lsc->sc_offset + lsc->sc_size - 1;

			if (sc->sc_offset >= lsc->sc_offset &&
			    sc->sc_offset <= llastblk) {
				/* Overlaps the tail of the exsiting wedge. */
				break;
			}
			if (lastblk >= lsc->sc_offset &&
			    lastblk <= llastblk) {
				/* Overlaps the head of the existing wedge. */
			    	break;
			}
		}
		if (lsc != NULL)
			error = EINVAL;
		else {
			pdk->dk_nwedges++;
			LIST_INSERT_HEAD(&pdk->dk_wedges, sc, sc_plink);
		}
	}
	(void) lockmgr(&pdk->dk_openlock, LK_RELEASE, NULL);
	if (error) {
		bufq_free(&sc->sc_bufq);
		free(sc, M_DKWEDGE);
		return (error);
	}

	/* Insert the larval wedge into the array. */
	(void) lockmgr(&dkwedges_lock, LK_EXCLUSIVE, NULL);
	for (error = 0;;) {
		struct dkwedge_softc **scpp;

		/*
		 * Check for a duplicate wname while searching for
		 * a slot.
		 */
		for (scpp = NULL, unit = 0; unit < ndkwedges; unit++) {
			if (dkwedges[unit] == NULL) {
				if (scpp == NULL) {
					scpp = &dkwedges[unit];
					sc->sc_unit = unit;
				}
			} else {
				/* XXX Unicode. */
				if (strcmp(dkwedges[unit]->sc_wname,
					   sc->sc_wname) == 0) {
					error = EEXIST;
					break;
				}
			}
		}
		if (error)
			break;
		KASSERT(unit == ndkwedges);
		if (scpp == NULL)
			dkwedge_array_expand();
		else {
			KASSERT(scpp == &dkwedges[sc->sc_unit]);
			*scpp = sc;
			break;
		}
	}
	(void) lockmgr(&dkwedges_lock, LK_RELEASE, NULL);
	if (error) {
		(void) lockmgr(&pdk->dk_openlock, LK_EXCLUSIVE, NULL);
		pdk->dk_nwedges--;
		LIST_REMOVE(sc, sc_plink);
		(void) lockmgr(&pdk->dk_openlock, LK_RELEASE, NULL);

		bufq_free(&sc->sc_bufq);
		free(sc, M_DKWEDGE);
		return (error);
	}

	/* Now that we know the unit #, set the devname. */
	sprintf(sc->sc_devname, "dk%u", sc->sc_unit);
	sc->sc_dk.dk_name = sc->sc_devname;

	/* Return the devname to the caller. */
	strcpy(dkw->dkw_devname, sc->sc_devname);

	/*
	 * XXX Really ought to make the disk_attach() and the changing
	 * of state to RUNNING atomic.
	 */

	disk_attach(&sc->sc_dk);

	/* Disk wedge is ready for use! */
	sc->sc_state = DKW_STATE_RUNNING;

	/* Announce our arrival. */
	aprint_normal("%s at %s: %s\n", sc->sc_devname, pdk->dk_name,
	    sc->sc_wname);	/* XXX Unicode */
	aprint_normal("%s: %llu blocks at %lld, type: %s\n",
	    sc->sc_devname, sc->sc_size, sc->sc_offset, sc->sc_ptype);

	return (0);
}

/*
 * dkwedge_del:		[exported function]
 *
 *	Delete a disk wedge based on the provided information.
 *	NOTE: We look up the wedge based on the wedge devname,
 *	not wname.
 */
int
dkwedge_del(struct dkwedge_info *dkw)
{
	struct dkwedge_softc *sc = NULL;
	u_int unit;
	int bmaj, cmaj, i, mn, s;

	/* Find our softc. */
	dkw->dkw_devname[sizeof(dkw->dkw_devname) - 1] = '\0';
	(void) lockmgr(&dkwedges_lock, LK_EXCLUSIVE, NULL);
	for (unit = 0; unit < ndkwedges; unit++) {
		if ((sc = dkwedges[unit]) != NULL &&
		    strcmp(sc->sc_devname, dkw->dkw_devname) == 0 &&
		    strcmp(sc->sc_parent->dk_name, dkw->dkw_parent) == 0) {
			/* Mark the wedge as dying. */
			sc->sc_state = DKW_STATE_DYING;
			break;
		}
	}
	(void) lockmgr(&dkwedges_lock, LK_RELEASE, NULL);
	if (unit == ndkwedges)
		return (ESRCH);

	KASSERT(sc != NULL);

	/* Locate the wedge major numbers. */
	bmaj = bdevsw_lookup_major(&dk_bdevsw);
	cmaj = cdevsw_lookup_major(&dk_cdevsw);

	/* Kill any pending restart. */
	callout_stop(&sc->sc_restart_ch);

	/*
	 * dkstart() will kill any queued buffers now that the
	 * state of the wedge is not RUNNING.  Once we've done
	 * that, wait for any other pending I/O to complete.
	 */
	s = splbio();
	dkstart(sc);
	dkwedge_wait_drain(sc);
	splx(s);

	/* Nuke the vnodes for any open instances. */
	for (i = 0; i < MAXPARTITIONS; i++) {
		mn = DISKMINOR(unit, i);
		vdevgone(bmaj, mn, mn, VBLK);
		vdevgone(cmaj, mn, mn, VCHR);
	}

	/* Clean up the parent. */
	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_EXCLUSIVE, NULL);
	if (sc->sc_open) {
		if (sc->sc_parent->dk_rawopens-- == 1) {
			KASSERT(sc->sc_parent->dk_rawvp != NULL);
			(void) vn_close(sc->sc_parent->dk_rawvp, FREAD | FWRITE,
					NOCRED, curproc);
			sc->sc_parent->dk_rawvp = NULL;
		}
		sc->sc_open = 0;
	}
	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_RELEASE, NULL);

	/* Announce our departure. */
	aprint_normal("%s at %s (%s) deleted\n", sc->sc_devname,
	    sc->sc_parent->dk_name,
	    sc->sc_wname);	/* XXX Unicode */

	(void) lockmgr(&sc->sc_parent->dk_openlock, LK_EXCLUSIVE, NULL);
	sc->sc_parent->dk_nwedges--;
	LIST_REMOVE(sc, sc_plink);
	(void) lockmgr(&sc->sc_parent->dk_openlock, LK_RELEASE, NULL);

	/* Delete our buffer queue. */
	bufq_free(&sc->sc_bufq);

	/* Detach from the disk list. */
	disk_detach(&sc->sc_dk);

	/* Poof. */
	(void) lockmgr(&dkwedges_lock, LK_EXCLUSIVE, NULL);
	dkwedges[unit] = NULL;
	sc->sc_state = DKW_STATE_DEAD;
	(void) lockmgr(&dkwedges_lock, LK_RELEASE, NULL);

	free(sc, M_DKWEDGE);

	return (0);
}

/*
 * dkwedge_delall:	[exported function]
 *
 *	Delete all of the wedges on the specified disk.  Used when
 *	a disk is being detached.
 */
void
dkwedge_delall(struct disk *pdk)
{
	struct dkwedge_info dkw;
	struct dkwedge_softc *sc;

	for (;;) {
		(void) lockmgr(&pdk->dk_openlock, LK_EXCLUSIVE, NULL);
		if ((sc = LIST_FIRST(&pdk->dk_wedges)) == NULL) {
			KASSERT(pdk->dk_nwedges == 0);
			(void) lockmgr(&pdk->dk_openlock, LK_RELEASE, NULL);
			return;
		}
		strcpy(dkw.dkw_parent, pdk->dk_name);
		strcpy(dkw.dkw_devname, sc->sc_devname);
		(void) lockmgr(&pdk->dk_openlock, LK_RELEASE, NULL);
		(void) dkwedge_del(&dkw);
	}
}

/*
 * dkwedge_list:	[exported function]
 *
 *	List all of the wedges on a particular disk.
 *	If p == NULL, the buffer is in kernel space.  Otherwise, it is
 *	in user space of the specified process.
 */
int
dkwedge_list(struct disk *pdk, struct dkwedge_list *dkwl, struct proc *p)
{
	struct uio uio;
	struct iovec iov;
	struct dkwedge_softc *sc;
	struct dkwedge_info dkw;
	int error = 0;

	iov.iov_base = dkwl->dkwl_buf;
	iov.iov_len = dkwl->dkwl_bufsize;

	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = 0;
	uio.uio_resid = dkwl->dkwl_bufsize;
	uio.uio_segflg = p != NULL ? UIO_USERSPACE : UIO_SYSSPACE;
	uio.uio_rw = UIO_READ;
	uio.uio_procp = p;

	dkwl->dkwl_ncopied = 0;

	(void) lockmgr(&pdk->dk_openlock, LK_EXCLUSIVE, NULL);
	LIST_FOREACH(sc, &pdk->dk_wedges, sc_plink) {
		if (uio.uio_resid < sizeof(dkw))
			break;

		if (sc->sc_state != DKW_STATE_RUNNING)
			continue;

		strcpy(dkw.dkw_devname, sc->sc_devname);
		memcpy(dkw.dkw_wname, sc->sc_wname, sizeof(dkw.dkw_wname));
		dkw.dkw_wname[sizeof(dkw.dkw_wname) - 1] = '\0';
		strcpy(dkw.dkw_parent, sc->sc_parent->dk_name);
		dkw.dkw_offset = sc->sc_offset;
		dkw.dkw_size = sc->sc_size;
		strcpy(dkw.dkw_ptype, sc->sc_ptype);

		error = uiomove(&dkw, sizeof(dkw), &uio);
		if (error)
			break;
		dkwl->dkwl_ncopied++;
	}
	dkwl->dkwl_nwedges = pdk->dk_nwedges;
	(void) lockmgr(&pdk->dk_openlock, LK_RELEASE, NULL);

	return (error);
}

static int	dkwedge_discover_gpt(struct disk *, struct vnode *);

/*
 * dkwedge_discover:	[exported function]
 *
 *	Discover the wedges on a newly attached disk.
 */
void
dkwedge_discover(struct disk *pdk)
{
#ifndef DKWEDGE_AUTODISCOVER
	/*
	 * Require people playing with wedges to enable this explicitly.
	 */
	return;
#else
	int error;
	dev_t pdev;
	struct vnode *vp;

	error = dkwedge_compute_pdev(pdk->dk_name, &pdev);
	if (error) {
		aprint_error("%s: unable to compute pdev, error = %d\n",
		    pdk->dk_name, error);
		return;
	}

	error = bdevvp(pdev, &vp);
	if (error) {
		aprint_error("%s: unable to find vnode for pdev, error = %d\n",
		    pdk->dk_name, error);
		return;
	}

	error = vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
	if (error) {
		aprint_error("%s: unable to lock vnode for pdev, error = %d\n",
		    pdk->dk_name, error);
		vrele(vp);
		return;
	}

	error = VOP_OPEN(vp, FREAD | FWRITE, NOCRED, 0);
	if (error) {
		aprint_error("%s: unable to open device, error = %d\n",
		    pdk->dk_name, error);
		vput(vp);
		return;
	}
	/* VOP_OPEN() doesn't do this for us. */
	vp->v_writecount++;
	VOP_UNLOCK(vp, 0);

	/*
	 * For each supported partition map type, look to see if
	 * this map type exists.  If so, parse it and add the
	 * corresponding wedges.
	 */
	/* XXX Just GPT, for now. */
	dkwedge_discover_gpt(pdk, vp);

	error = vn_close(vp, FREAD | FWRITE, NOCRED, curproc);
	if (error) {
		aprint_error("%s: unable to close device, error = %d\n",
		    pdk->dk_name, error);
		/* We'll just assume the vnode has been cleaned up. */
	}
#endif /* DKWEDGE_AUTODISCOVER */
}

/*
 * dkwedge_read:
 *
 *	Read the some data from the specified disk, used for
 *	partition discovery.
 *
 *	XXX static, for now.
 */
static int
dkwedge_read(struct disk *pdk, struct vnode *vp, daddr_t blkno, void *buf,
    size_t len)
{
	struct buf b;

	BUF_INIT(&b);

	b.b_vp = vp;
	b.b_dev = vp->v_rdev;
	b.b_blkno = blkno;
	b.b_bcount = b.b_resid = len;
	b.b_flags = B_READ;
	b.b_proc = curproc;
	b.b_data = buf;

	VOP_STRATEGY(vp, &b);
	return (biowait(&b));
}

/*
 * dkwedge_lookup:
 *
 *	Look up a dkwedge_softc based on the provided dev_t.
 */
static struct dkwedge_softc *
dkwedge_lookup(dev_t dev)
{
	int unit = DISKUNIT(dev);

	if (unit >= ndkwedges)
		return (NULL);

	KASSERT(dkwedges != NULL);

	return (dkwedges[unit]);
}

/*
 * dkopen:		[devsw entry point]
 *
 *	Open a wedge.
 */
static int
dkopen(dev_t dev, int flags, int fmt, struct proc *p)
{
	struct dkwedge_softc *sc = dkwedge_lookup(dev);
	struct vnode *vp;
	int error;

	if (sc == NULL)
		return (ENODEV);

	if (sc->sc_state != DKW_STATE_RUNNING)
		return (ENXIO);

	/*
	 * We go through a complicated little dance to only open the parent
	 * vnode once per wedge, no matter how many times the wedge is
	 * opened.  The reason?  We see one dkopen() per open call, but
	 * only dkclose() on the last close.
	 */
	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_EXCLUSIVE, NULL);
	if (sc->sc_open == 0) {
		if (sc->sc_parent->dk_rawopens++ == 0) {
			KASSERT(sc->sc_parent->dk_rawvp == NULL);
			error = bdevvp(sc->sc_pdev, &vp);
			if (error)
				goto popen_fail;
			error = vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
			if (error) {
				vrele(vp);
				goto popen_fail;
			}
			error = VOP_OPEN(vp, FREAD | FWRITE, NOCRED, 0);
			if (error) {
				vput(vp);
				goto popen_fail;
			}
			/* VOP_OPEN() doesn't do this for us. */
			vp->v_writecount++;
			VOP_UNLOCK(vp, 0);
			sc->sc_parent->dk_rawvp = vp;
		}
		sc->sc_open = 1;
	}
	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_RELEASE, NULL);

	return (0);

 popen_fail:
	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_RELEASE, NULL);
	return (error);
}

/*
 * dkclose:		[devsw entry point]
 *
 *	Close a wedge.
 */
static int
dkclose(dev_t dev, int flags, int fmt, struct proc *p)
{
	struct dkwedge_softc *sc = dkwedge_lookup(dev);
	int error = 0;

	KASSERT(sc->sc_open);

	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_EXCLUSIVE, NULL);

	if (sc->sc_parent->dk_rawopens-- == 1) {
		KASSERT(sc->sc_parent->dk_rawvp != NULL);
		error = vn_close(sc->sc_parent->dk_rawvp, FREAD | FWRITE,
				 NOCRED, p);
		sc->sc_parent->dk_rawvp = NULL;
	}
	sc->sc_open = 0;

	(void) lockmgr(&sc->sc_parent->dk_rawlock, LK_RELEASE, NULL);

	return (error);
}

/*
 * dkstragegy:		[devsw entry point]
 *
 *	Perform I/O based on the wedge I/O strategy.
 */
static void
dkstrategy(struct buf *bp)
{
	struct dkwedge_softc *sc = dkwedge_lookup(bp->b_dev);
	int s;

	if (sc->sc_state != DKW_STATE_RUNNING) {
		bp->b_error = ENXIO;
		bp->b_flags |= B_ERROR;
		goto done;
	}

	/* If it's an empty transfer, wake up the top half now. */
	if (bp->b_bcount == 0)
		goto done;

	/* Make sure it's in-range. */
	if (bounds_check_with_mediasize(bp, DEV_BSIZE, sc->sc_size) <= 0)
		goto done;

	/* Translate it to the parent's raw LBA. */
	bp->b_rawblkno = bp->b_blkno + sc->sc_offset;

	/* Place it in the queue and start I/O on the unit. */
	s = splbio();
	sc->sc_iopend++;
	BUFQ_PUT(&sc->sc_bufq, bp);
	dkstart(sc);
	splx(s);
	return;

 done:
	bp->b_resid = bp->b_bcount;
	biodone(bp);
}

/*
 * dkstart:
 *
 *	Start I/O that has been enqueued on the wedge.
 *	NOTE: Must be called at splbio()!
 */
static void
dkstart(struct dkwedge_softc *sc)
{
	struct buf *bp, *nbp;

	/* Do as much work as has been enqueued. */
	while ((bp = BUFQ_PEEK(&sc->sc_bufq)) != NULL) {
		if (sc->sc_state != DKW_STATE_RUNNING) {
			(void) BUFQ_GET(&sc->sc_bufq);
			if (sc->sc_iopend-- == 1 &&
			    (sc->sc_flags & DK_F_WAIT_DRAIN) != 0) {
				sc->sc_flags &= ~DK_F_WAIT_DRAIN;
				wakeup(&sc->sc_iopend);
			}
			bp->b_error = ENXIO;
			bp->b_flags |= B_ERROR;
			bp->b_resid = bp->b_bcount;
			biodone(bp);
		}

		/* Instrumentation. */
		disk_busy(&sc->sc_dk);
		
		nbp = pool_get(&bufpool, PR_NOWAIT);
		if (nbp == NULL) {
			/*
			 * No resources to run this request; leave the
			 * buffer queued up, and schedule a timer to
			 * restart the queue in 1/2 a second.
			 */
			disk_unbusy(&sc->sc_dk, 0, bp->b_flags & B_READ);
			callout_schedule(&sc->sc_restart_ch, hz / 2);
			return;
		}

		(void) BUFQ_GET(&sc->sc_bufq);

		BUF_INIT(nbp);
		nbp->b_data = bp->b_data;
		nbp->b_flags = bp->b_flags | B_CALL;
		nbp->b_iodone = dkiodone;
		nbp->b_proc = bp->b_proc;
		nbp->b_blkno = bp->b_rawblkno;
		nbp->b_dev = sc->sc_parent->dk_rawvp->v_rdev;
		nbp->b_vp = sc->sc_parent->dk_rawvp;
		nbp->b_bcount = bp->b_bcount;
		nbp->b_private = bp;
		BIO_COPYPRIO(nbp, bp);

		if ((nbp->b_flags & B_READ) == 0)
			V_INCR_NUMOUTPUT(nbp->b_vp);
		VOP_STRATEGY(nbp->b_vp, nbp);
	}
}

/*
 * dkiodone:
 *
 *	I/O to a wedge has completed; alert the top half.
 *	NOTE: Must be called at splbio()!
 */
static void
dkiodone(struct buf *bp)
{
	struct buf *obp = bp->b_private;
	struct dkwedge_softc *sc = dkwedge_lookup(obp->b_dev);

	if (bp->b_flags & B_ERROR) {
		obp->b_flags |= B_ERROR;
		obp->b_error = bp->b_error;
	}
	obp->b_resid = bp->b_resid;
	pool_put(&bufpool, bp);

	if (sc->sc_iopend-- == 1 && (sc->sc_flags & DK_F_WAIT_DRAIN) != 0) {
		sc->sc_flags &= ~DK_F_WAIT_DRAIN;
		wakeup(&sc->sc_iopend);
	}

	disk_unbusy(&sc->sc_dk, obp->b_bcount - obp->b_resid,
	    obp->b_flags & B_READ);

	biodone(obp);

	/* Kick the queue in case there is more work we can do. */
	dkstart(sc);
}

/*
 * dkrestart:
 *
 *	Restart the work queue after it was stalled due to
 *	a resource shortage.  Invoked via a callout.
 */
static void
dkrestart(void *v)
{
	struct dkwedge_softc *sc = v;
	int s;

	s = splbio();
	dkstart(sc);
	splx(s);
}

/*
 * dkread:		[devsw entry point]
 *
 *	Read from a wedge.
 */
static int
dkread(dev_t dev, struct uio *uio, int flags)
{
	struct dkwedge_softc *sc = dkwedge_lookup(dev);

	if (sc->sc_state != DKW_STATE_RUNNING)
		return (ENXIO);
	
	return (physio(dkstrategy, NULL, dev, B_READ,
		       sc->sc_parent->dk_driver->d_minphys, uio));
}

/*
 * dkwrite:		[devsw entry point]
 *
 *	Write to a wedge.
 */
static int
dkwrite(dev_t dev, struct uio *uio, int flags)
{
	struct dkwedge_softc *sc = dkwedge_lookup(dev);

	if (sc->sc_state != DKW_STATE_RUNNING)
		return (ENXIO);
	
	return (physio(dkstrategy, NULL, dev, B_WRITE,
		       sc->sc_parent->dk_driver->d_minphys, uio));
}

/*
 * dkioctl:		[devsw entry point]
 *
 *	Perform an ioctl request on a wedge.
 */
static int
dkioctl(dev_t dev, u_long cmd, caddr_t data, int flag, struct proc *p)
{
	struct dkwedge_softc *sc = dkwedge_lookup(dev);
	int error = 0;

	if (sc->sc_state != DKW_STATE_RUNNING)
		return (ENXIO);

	switch (cmd) {
	case DIOCGWEDGEINFO:
	    {
	    	struct dkwedge_info *dkw = (void *) data;

		strcpy(dkw->dkw_devname, sc->sc_devname);
	    	memcpy(dkw->dkw_wname, sc->sc_wname, sizeof(dkw->dkw_wname));
		dkw->dkw_wname[sizeof(dkw->dkw_wname) - 1] = '\0';
		strcpy(dkw->dkw_parent, sc->sc_parent->dk_name);
		dkw->dkw_offset = sc->sc_offset;
		dkw->dkw_size = sc->sc_size;
		strcpy(dkw->dkw_ptype, sc->sc_ptype);

		break;
	    }

	default:
		error = ENOTTY;
	}

	return (error);
}

/*
 * dksize:		[devsw entry point]
 *
 *	Query the size of a wedge for the purpose of performing a dump
 *	or for swapping to.
 */
static int
dksize(dev_t dev)
{

	/* XXX */
	return (-1);
}

/*
 * dkdump:		[devsw entry point]
 *
 *	Perform a crash dump to a wedge.
 */
static int
dkdump(dev_t dev, daddr_t blkno, caddr_t va, size_t size)
{

	/* XXX */
	return (ENXIO);
}

/*****************************************************************************
 * EFI GUID Partition Table support
 *****************************************************************************/

#include <sys/disklabel_gpt.h>
#include <sys/uuid.h>

static const struct {
	struct uuid ptype_guid;
	const char *ptype_str;
} gpt_ptype_guid_to_str_tab[] = {
	{ GPT_ENT_TYPE_EFI,		"msdos" },	/* XXX yes? */
#if 0
	{ GPT_ENT_TYPE_FREEBSD,		??? },
#endif
	{ GPT_ENT_TYPE_FREEBSD_SWAP,	"swap" },	/* XXX for now */
	{ GPT_ENT_TYPE_FREEBSD_UFS,	"ffs" },	/* XXX for now */

	/* XXX What about the MS and Linux types? */

	{ { 0 },			NULL },
};

static const char *
gpt_ptype_guid_to_str(const struct uuid *guid)
{
	int i;

	for (i = 0; gpt_ptype_guid_to_str_tab[i].ptype_str != NULL; i++) {
		if (memcmp(&gpt_ptype_guid_to_str_tab[i].ptype_guid,
			   guid, sizeof(*guid)) == 0)
			return (gpt_ptype_guid_to_str_tab[i].ptype_str);
	}

	return (NULL);
}

static const uint32_t gpt_crc_tab[16] = {
	0x00000000U, 0x1db71064U, 0x3b6e20c8U, 0x26d930acU,
	0x76dc4190U, 0x6b6b51f4U, 0x4db26158U, 0x5005713cU,
	0xedb88320U, 0xf00f9344U, 0xd6d6a3e8U, 0xcb61b38cU,
	0x9b64c2b0U, 0x86d3d2d4U, 0xa00ae278U, 0xbdbdf21cU
};

static uint32_t
gpt_crc32(const void *vbuf, size_t len)
{
	const uint8_t *buf = vbuf;
	uint32_t crc;

	crc = 0xffffffffU;
	while (len--) {
		crc ^= *buf++;
		crc = (crc >> 4) ^ gpt_crc_tab[crc & 0xf];
		crc = (crc >> 4) ^ gpt_crc_tab[crc & 0xf];
	}

	return (crc ^ 0xffffffffU);
}

static int
gpt_verify_header_crc(struct gpt_hdr *hdr)
{
	uint32_t crc;
	int rv;

	crc = hdr->hdr_crc_self;
	hdr->hdr_crc_self = 0;
	rv = le32toh(crc) == gpt_crc32(hdr, le32toh(hdr->hdr_size));
	hdr->hdr_crc_self = crc;

	return (rv);
}

static int
dkwedge_discover_gpt(struct disk *pdk, struct vnode *vp)
{
	static const struct uuid ent_type_unused = GPT_ENT_TYPE_UNUSED;
	static const char gpt_hdr_sig[] = GPT_HDR_SIG;
	struct dkwedge_info dkw;
	void *buf;
	struct gpt_hdr *hdr;
	struct gpt_ent *ent;
	uint32_t entries, entsz;
	daddr_t lba_start, lba_end, lba_table;
	uint32_t gpe_crc;
	int error;
	u_int i;

	buf = malloc(DEV_BSIZE, M_DEVBUF, M_WAITOK);

	/*
	 * Note: We don't bother with a Legacy or Protective MBR
	 * here.  If a GPT is found, then the search stops, and
	 * the GPT is authoritative.
	 */

	/* Read in the GPT Header. */
	error = dkwedge_read(pdk, vp, GPT_HDR_BLKNO, buf, DEV_BSIZE);
	if (error)
		goto out;
	hdr = buf;

	/* Validate it. */
	if (memcmp(gpt_hdr_sig, hdr->hdr_sig, sizeof(hdr->hdr_sig)) != 0) {
		/* XXX Should check at end-of-disk. */
		error = ESRCH;
		goto out;
	}
	if (hdr->hdr_revision != htole32(GPT_HDR_REVISION)) {
		/* XXX Should check at end-of-disk. */
		error = ESRCH;
		goto out;
	}
	if (le32toh(hdr->hdr_size) > DEV_BSIZE) {
		/* XXX Should check at end-of-disk. */
		error = ESRCH;
		goto out;
	}
	if (gpt_verify_header_crc(hdr) == 0) {
		/* XXX Should check at end-of-disk. */
		error = ESRCH;
		goto out;
	}

	/* XXX Now that we found it, should we validate the backup? */

	{
		struct uuid disk_guid;
		char guid_str[UUID_STR_LEN];
		uuid_dec_le(hdr->hdr_guid, &disk_guid);
		uuid_snprintf(guid_str, sizeof(guid_str), &disk_guid);
		aprint_verbose("%s: GPT GUID: %s\n", pdk->dk_name, guid_str);
	}

	entries = le32toh(hdr->hdr_entries);
	entsz = roundup(le32toh(hdr->hdr_entsz), 8);
	if (entsz > roundup(sizeof(struct gpt_ent), 8)) {
		aprint_error("%s: bogus GPT entry size: %u\n",
		    pdk->dk_name, le32toh(hdr->hdr_entsz));
		error = EINVAL;
		goto out;
	}
	gpe_crc = le32toh(hdr->hdr_crc_table);

	/* XXX Clamp entries at 128 for now. */
	if (entries > 128) {
		aprint_error("%s: WARNING: clamping number of GPT entries to "
		    "128 (was %u)\n", pdk->dk_name, entries);
		entries = 128;
	}

	lba_start = le64toh(hdr->hdr_lba_start);
	lba_end = le64toh(hdr->hdr_lba_end);
	lba_table = le64toh(hdr->hdr_lba_table);
	if (lba_start < 0 || lba_end < 0 || lba_table < 0) {
		aprint_error("%s: GPT block numbers out of range\n",
		    pdk->dk_name);
		error = EINVAL;
		goto out;
	}

	free(buf, M_DEVBUF);
	buf = malloc(roundup(entries * entsz, DEV_BSIZE), M_DEVBUF, M_WAITOK);
	error = dkwedge_read(pdk, vp, lba_table, buf,
			     roundup(entries * entsz, DEV_BSIZE));
	if (error) {
		/* XXX Should check alternate location. */
		aprint_error("%s: unable to read GPT partition array, "
		    "error = %d\n", pdk->dk_name, error);
		goto out;
	}

	if (gpt_crc32(buf, entries * entsz) != gpe_crc) {
		/* XXX Should check alternate location. */
		aprint_error("%s: bad GPT partition array CRC\n",
		    pdk->dk_name);
		error = EINVAL;
		goto out;
	}

	/*
	 * Walk the partitions, adding a wedge for each type we know about.
	 */
	for (i = 0; i < entries; i++) {
		struct uuid ptype_guid, ent_guid;
		const char *ptype;
		int j;
		char ptype_guid_str[UUID_STR_LEN], ent_guid_str[UUID_STR_LEN];

		ent = (struct gpt_ent *)((caddr_t)buf + (i * entsz));

		uuid_dec_le(ent->ent_type, &ptype_guid);
		if (memcmp(&ptype_guid, &ent_type_unused,
			   sizeof(ptype_guid)) == 0)
			continue;

		uuid_dec_le(ent->ent_guid, &ent_guid);

		uuid_snprintf(ptype_guid_str, sizeof(ptype_guid_str),
		    &ptype_guid);
		uuid_snprintf(ent_guid_str, sizeof(ent_guid_str),
		    &ent_guid);

		/* Skip it if we don't grok this ptype. */
		if ((ptype = gpt_ptype_guid_to_str(&ptype_guid)) == NULL) {
			/*
			 * XXX Should probably just add these... maybe
			 * XXX just have an empty ptype?
			 */
			aprint_verbose("%s: skipping entry %u (%s), type %s\n",
			    pdk->dk_name, i, ent_guid_str, ptype_guid_str);
			continue;
		}
		strcpy(dkw.dkw_ptype, ptype);

		strcpy(dkw.dkw_parent, pdk->dk_name);
		dkw.dkw_offset = le64toh(ent->ent_lba_start);
		dkw.dkw_size = le64toh(ent->ent_lba_end) - dkw.dkw_offset + 1;

		/* XXX Make sure it falls within the disk's data area. */

		if (ent->ent_name[0] == 0x0000)
			strcpy(dkw.dkw_wname, ent_guid_str);
		else {
			for (j = 0; ent->ent_name[j] != 0x0000; j++) {
				/* XXX UTF-16 -> UTF-8 */
				dkw.dkw_wname[j] =
				    le16toh(ent->ent_name[j]) & 0xff;
			}
			dkw.dkw_wname[j] = '\0';
		}

		/*
		 * Try with the partition name first.  If that fails,
		 * use the GUID string.  If that fails, punt.
		 */
		if ((error = dkwedge_add(&dkw)) == EEXIST) {
			aprint_error("%s: wedge named '%s' already exists, "
			    "trying '%s'\n", pdk->dk_name,
			    dkw.dkw_wname, /* XXX Unicode */
			    ent_guid_str);
			strcpy(dkw.dkw_wname, ent_guid_str);
			error = dkwedge_add(&dkw);
		}
		if (error == EEXIST)
			aprint_error("%s: wedge named '%s' already exists, "
			    "manual intervention required\n", pdk->dk_name,
			    dkw.dkw_wname);
		else if (error)
			aprint_error("%s: error %d adding entry %u (%s), "
			    "type %s\n", pdk->dk_name, error, i, ent_guid_str,
			    ptype_guid_str);
	}
	error = 0;

 out:
	free(buf, M_DEVBUF);
	return (error);
}
