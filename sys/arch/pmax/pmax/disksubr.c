/*	$NetBSD: disksubr.c,v 1.31 2000/05/16 05:45:49 thorpej Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ufs_disksubr.c	7.16 (Berkeley) 5/4/91
 */

#include "opt_compat_ultrix.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/disk.h>
#include <sys/disklabel.h>

#ifdef COMPAT_ULTRIX
#include <machine/dec_boot.h>
#include <ufs/ufs/dinode.h>		/* XXX for fs.h */
#include <ufs/ffs/fs.h>			/* XXX for BBSIZE & SBSIZE */

char	*compat_label __P((dev_t dev, void (*strat) __P((struct buf *bp)),
	    struct disklabel *lp, struct cpu_disklabel *osdep));	/* XXX */

#endif

/*
 * Attempt to read a disk label from a device
 * using the indicated stategy routine.
 * The label must be partly set up before this:
 * secpercyl and anything required in the strategy routine
 * (e.g., sector size) must be filled in before calling us.
 * Returns null on success and an error string on failure.
 */
char *
readdisklabel(dev, strat, lp, osdep)
	dev_t dev;
	void (*strat) __P((struct buf *bp));
	struct disklabel *lp;
	struct cpu_disklabel *osdep;
{
	struct buf *bp;
	struct disklabel *dlp;
	char *msg = NULL;

	if (lp->d_secperunit == 0)
		lp->d_secperunit = 0x1fffffff;
	lp->d_npartitions = 1;
	if (lp->d_partitions[0].p_size == 0)
		lp->d_partitions[0].p_size = 0x1fffffff;
	lp->d_partitions[0].p_offset = 0;

	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;
	bp->b_blkno = LABELSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	bp->b_cylinder = LABELSECTOR / lp->d_secpercyl;
	(*strat)(bp);
	if (biowait(bp)) {
		msg = "I/O error";
	} else for (dlp = (struct disklabel *)bp->b_un.b_addr;
	    dlp <= (struct disklabel *)(bp->b_un.b_addr+DEV_BSIZE-sizeof(*dlp));
	    dlp = (struct disklabel *)((char *)dlp + sizeof(long))) {
		if (dlp->d_magic != DISKMAGIC || dlp->d_magic2 != DISKMAGIC) {
			if (msg == NULL)
				msg = "no disk label";
		} else if (dlp->d_npartitions > MAXPARTITIONS ||
			   dkcksum(dlp) != 0)
			msg = "disk label corrupted";
		else {
			*lp = *dlp;
			msg = NULL;
			break;
		}
	}
	bp->b_flags = B_INVAL | B_AGE;
	brelse(bp);
#ifdef COMPAT_ULTRIX
	/*
	 * If no NetBSD label was found, check for an Ultrix label and
	 * construct tne incore label from the Ultrix partition information.
	 */
	if (msg != NULL) {
		msg = compat_label(dev, strat, lp, osdep);
		if (msg == NULL) {
			printf("WARNING: using Ultrix partition information\n");
			/* set geometry? */
		}
	}
#endif
/* XXX If no NetBSD label or Ultrix label found, generate default label here */
	return (msg);
}

#ifdef COMPAT_ULTRIX
/*
 * Given a buffer bp, try and interpret it as an Ultrix disk label,
 * putting the partition info into a native NetBSD label
 */
char *
compat_label(dev, strat, lp, osdep)
	dev_t dev;
	void (*strat) __P((struct buf *bp));
	struct disklabel *lp;
	struct cpu_disklabel *osdep;
{
	dec_disklabel *dlp;
	struct buf *bp = NULL;
	char *msg = NULL;

	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;
	bp->b_blkno = DEC_LABEL_SECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	bp->b_cylinder = DEC_LABEL_SECTOR / lp->d_secpercyl;
	(*strat)(bp);

	if (biowait(bp)) {
                msg = "I/O error";
		goto done;
	}

	for (dlp = (dec_disklabel *)bp->b_un.b_addr;
	     dlp <= (dec_disklabel *)(bp->b_un.b_addr+DEV_BSIZE-sizeof(*dlp));
	     dlp = (dec_disklabel *)((char *)dlp + sizeof(long))) {

		int part;

		if (dlp->magic != DEC_LABEL_MAGIC) {
			printf("label: %x\n",dlp->magic);
			msg = ((msg != NULL) ? msg: "no disk label");
			goto done;
		}

#ifdef DIAGNOSTIC
/*XXX*/		printf("Interpreting Ultrix label\n");
#endif

		lp->d_magic = DEC_LABEL_MAGIC;
		lp->d_npartitions = 0;
		strncpy(lp->d_packname, "Ultrix label", 16);
		lp->d_rpm = 3600;
		lp->d_interleave = 1;
		lp->d_flags = 0;
		lp->d_bbsize = BBSIZE;
		lp->d_sbsize = SBSIZE;
		for (part = 0;
		     part <((MAXPARTITIONS<DEC_NUM_DISK_PARTS) ?
			    MAXPARTITIONS : DEC_NUM_DISK_PARTS);
		     part++) {
			lp->d_partitions[part].p_size = dlp->map[part].num_blocks;
			lp->d_partitions[part].p_offset = dlp->map[part].start_block;
			lp->d_partitions[part].p_fsize = 1024;
			lp->d_partitions[part].p_fstype =
			  (part==1) ? FS_SWAP : FS_BSDFFS;
			lp->d_npartitions += 1;

#ifdef DIAGNOSTIC
			printf(" Ultrix label rz%d%c: start %d len %d\n",
			       DISKUNIT(dev), "abcdefgh"[part],
			       lp->d_partitions[part].p_offset,
			       lp->d_partitions[part].p_size);
#endif
		}
		break;
	}

done:
	bp->b_flags = B_INVAL | B_AGE;
	brelse(bp);
	return (msg);
}
#endif /* COMPAT_ULTRIX */


/*
 * Check new disk label for sensibility
 * before setting it.
 */
int
setdisklabel(olp, nlp, openmask, osdep)
	struct disklabel *olp, *nlp;
	u_long openmask;
	struct cpu_disklabel *osdep;
{
	int i;
	struct partition *opp, *npp;

	if (nlp->d_magic != DISKMAGIC || nlp->d_magic2 != DISKMAGIC ||
	    dkcksum(nlp) != 0)
		return (EINVAL);
	while ((i = ffs((long)openmask)) != 0) {
		i--;
		openmask &= ~(1 << i);
		if (nlp->d_npartitions <= i)
			return (EBUSY);
		opp = &olp->d_partitions[i];
		npp = &nlp->d_partitions[i];
		if (npp->p_offset != opp->p_offset || npp->p_size < opp->p_size)
			return (EBUSY);
		/*
		 * Copy internally-set partition information
		 * if new label doesn't include it.		XXX
		 */
		if (npp->p_fstype == FS_UNUSED && opp->p_fstype != FS_UNUSED) {
			npp->p_fstype = opp->p_fstype;
			npp->p_fsize = opp->p_fsize;
			npp->p_frag = opp->p_frag;
			npp->p_cpg = opp->p_cpg;
		}
	}
 	nlp->d_checksum = 0;
 	nlp->d_checksum = dkcksum(nlp);
	*olp = *nlp;
	return (0);
}

/*
 * Write disk label back to device after modification.
 */
int
writedisklabel(dev, strat, lp, osdep)
	dev_t dev;
	void (*strat) __P((struct buf *bp));
	struct disklabel *lp;
	struct cpu_disklabel *osdep;
{
	struct buf *bp;
	struct disklabel *dlp;
	int labelpart;
	int error = 0;

	labelpart = DISKPART(dev);
	if (lp->d_partitions[labelpart].p_offset != 0) {
		if (lp->d_partitions[0].p_offset != 0)
			return (EXDEV);			/* not quite right */
		labelpart = 0;
	}
	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = makedev(major(dev), DISKMINOR(DISKUNIT(dev), labelpart));
	bp->b_blkno = LABELSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_READ;
	(*strat)(bp);
	if ((error = biowait(bp)) != 0)
		goto done;
	for (dlp = (struct disklabel *)bp->b_un.b_addr;
	    dlp <= (struct disklabel *)
	      (bp->b_un.b_addr + lp->d_secsize - sizeof(*dlp));
	    dlp = (struct disklabel *)((char *)dlp + sizeof(long))) {
		if (dlp->d_magic == DISKMAGIC && dlp->d_magic2 == DISKMAGIC &&
		    dkcksum(dlp) == 0) {
			*dlp = *lp;
			bp->b_flags = B_WRITE;
			(*strat)(bp);
			error = biowait(bp);
			goto done;
		}
	}
	error = ESRCH;
done:
	brelse(bp);
	return (error);
}

/*
 * UNTESTED !!
 *
 * Determine the size of the transfer, and make sure it is
 * within the boundaries of the partition. Adjust transfer
 * if needed, and signal errors or early completion.
 */
int
bounds_check_with_label(bp, lp, wlabel)
	struct buf *bp;
	struct disklabel *lp;
	int wlabel;
{

	struct partition *p = lp->d_partitions + DISKPART(bp->b_dev);
	int labelsect = lp->d_partitions[RAW_PART].p_offset;
	int maxsz = p->p_size;
	int sz = (bp->b_bcount + DEV_BSIZE - 1) >> DEV_BSHIFT;

	/* overwriting disk label ? */
	/* XXX should also protect bootstrap in first 8K */
	if (bp->b_blkno + p->p_offset <= LABELSECTOR + labelsect &&
	    (bp->b_flags & B_READ) == 0 && wlabel == 0) {
		bp->b_error = EROFS;
		goto bad;
	}

	/* beyond partition? */
	if (bp->b_blkno < 0 || bp->b_blkno + sz > maxsz) {
		/* if exactly at end of disk, return an EOF */
		if (bp->b_blkno == maxsz) {
			bp->b_resid = bp->b_bcount;
			return(0);
		}
		/* or truncate if part of it fits */
		sz = maxsz - bp->b_blkno;
		if (sz <= 0) {
			bp->b_error = EINVAL;
			goto bad;
		}
		bp->b_bcount = sz << DEV_BSHIFT;
	}

	/* calculate cylinder for disksort to order transfers with */
	bp->b_resid = (bp->b_blkno + p->p_offset) / lp->d_secpercyl;
	return(1);
bad:
	bp->b_flags |= B_ERROR;
	return(-1);
}
