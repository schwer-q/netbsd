/*	$NetBSD: fdesc_vnops.c,v 1.91 2006/04/04 14:18:35 christos Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 *	@(#)fdesc_vnops.c	8.17 (Berkeley) 5/22/95
 *
 * #Id: fdesc_vnops.c,v 1.12 1993/04/06 16:17:17 jsp Exp #
 */

/*
 * /dev/fd Filesystem
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: fdesc_vnops.c,v 1.91 2006/04/04 14:18:35 christos Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/proc.h>
#include <sys/kernel.h>	/* boottime */
#include <sys/resourcevar.h>
#include <sys/socketvar.h>
#include <sys/filedesc.h>
#include <sys/vnode.h>
#include <sys/malloc.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/buf.h>
#include <sys/dirent.h>
#include <sys/tty.h>

#include <miscfs/fdesc/fdesc.h>
#include <miscfs/genfs/genfs.h>

#define cttyvp(p) ((p)->p_flag & P_CONTROLT ? (p)->p_session->s_ttyvp : NULL)

#define FDL_WANT	0x01
#define FDL_LOCKED	0x02
static int fdcache_lock;

dev_t devctty;

#if (FD_STDIN != FD_STDOUT-1) || (FD_STDOUT != FD_STDERR-1)
FD_STDIN, FD_STDOUT, FD_STDERR must be a sequence n, n+1, n+2
#endif

#define	NFDCACHE 4

#define FD_NHASH(ix) \
	(&fdhashtbl[(ix) & fdhash])
LIST_HEAD(fdhashhead, fdescnode) *fdhashtbl;
u_long fdhash;

int	fdesc_lookup(void *);
#define	fdesc_create	genfs_eopnotsupp
#define	fdesc_mknod	genfs_eopnotsupp
int	fdesc_open(void *);
#define	fdesc_close	genfs_nullop
#define	fdesc_access	genfs_nullop
int	fdesc_getattr(void *);
int	fdesc_setattr(void *);
int	fdesc_read(void *);
int	fdesc_write(void *);
int	fdesc_ioctl(void *);
int	fdesc_poll(void *);
int	fdesc_kqfilter(void *);
#define	fdesc_mmap	genfs_eopnotsupp
#define	fdesc_fcntl	genfs_fcntl
#define	fdesc_fsync	genfs_nullop
#define	fdesc_seek	genfs_seek
#define	fdesc_remove	genfs_eopnotsupp
int	fdesc_link(void *);
#define	fdesc_rename	genfs_eopnotsupp
#define	fdesc_mkdir	genfs_eopnotsupp
#define	fdesc_rmdir	genfs_eopnotsupp
int	fdesc_symlink(void *);
int	fdesc_readdir(void *);
int	fdesc_readlink(void *);
#define	fdesc_abortop	genfs_abortop
int	fdesc_inactive(void *);
int	fdesc_reclaim(void *);
#define	fdesc_lock	genfs_lock
#define	fdesc_unlock	genfs_unlock
#define	fdesc_bmap	genfs_badop
#define	fdesc_strategy	genfs_badop
int	fdesc_print(void *);
int	fdesc_pathconf(void *);
#define	fdesc_islocked	genfs_islocked
#define	fdesc_advlock	genfs_einval
#define	fdesc_bwrite	genfs_eopnotsupp
#define fdesc_revoke	genfs_revoke
#define fdesc_putpages	genfs_null_putpages

static int fdesc_attr(int, struct vattr *, struct ucred *, struct lwp *);

int (**fdesc_vnodeop_p)(void *);
const struct vnodeopv_entry_desc fdesc_vnodeop_entries[] = {
	{ &vop_default_desc, vn_default_error },
	{ &vop_lookup_desc, fdesc_lookup },		/* lookup */
	{ &vop_create_desc, fdesc_create },		/* create */
	{ &vop_mknod_desc, fdesc_mknod },		/* mknod */
	{ &vop_open_desc, fdesc_open },			/* open */
	{ &vop_close_desc, fdesc_close },		/* close */
	{ &vop_access_desc, fdesc_access },		/* access */
	{ &vop_getattr_desc, fdesc_getattr },		/* getattr */
	{ &vop_setattr_desc, fdesc_setattr },		/* setattr */
	{ &vop_read_desc, fdesc_read },			/* read */
	{ &vop_write_desc, fdesc_write },		/* write */
	{ &vop_ioctl_desc, fdesc_ioctl },		/* ioctl */
	{ &vop_fcntl_desc, fdesc_fcntl },		/* fcntl */
	{ &vop_poll_desc, fdesc_poll },			/* poll */
	{ &vop_kqfilter_desc, fdesc_kqfilter },		/* kqfilter */
	{ &vop_revoke_desc, fdesc_revoke },		/* revoke */
	{ &vop_mmap_desc, fdesc_mmap },			/* mmap */
	{ &vop_fsync_desc, fdesc_fsync },		/* fsync */
	{ &vop_seek_desc, fdesc_seek },			/* seek */
	{ &vop_remove_desc, fdesc_remove },		/* remove */
	{ &vop_link_desc, fdesc_link },			/* link */
	{ &vop_rename_desc, fdesc_rename },		/* rename */
	{ &vop_mkdir_desc, fdesc_mkdir },		/* mkdir */
	{ &vop_rmdir_desc, fdesc_rmdir },		/* rmdir */
	{ &vop_symlink_desc, fdesc_symlink },		/* symlink */
	{ &vop_readdir_desc, fdesc_readdir },		/* readdir */
	{ &vop_readlink_desc, fdesc_readlink },		/* readlink */
	{ &vop_abortop_desc, fdesc_abortop },		/* abortop */
	{ &vop_inactive_desc, fdesc_inactive },		/* inactive */
	{ &vop_reclaim_desc, fdesc_reclaim },		/* reclaim */
	{ &vop_lock_desc, fdesc_lock },			/* lock */
	{ &vop_unlock_desc, fdesc_unlock },		/* unlock */
	{ &vop_bmap_desc, fdesc_bmap },			/* bmap */
	{ &vop_strategy_desc, fdesc_strategy },		/* strategy */
	{ &vop_print_desc, fdesc_print },		/* print */
	{ &vop_islocked_desc, fdesc_islocked },		/* islocked */
	{ &vop_pathconf_desc, fdesc_pathconf },		/* pathconf */
	{ &vop_advlock_desc, fdesc_advlock },		/* advlock */
	{ &vop_bwrite_desc, fdesc_bwrite },		/* bwrite */
	{ &vop_putpages_desc, fdesc_putpages },		/* putpages */
	{ NULL, NULL }
};

const struct vnodeopv_desc fdesc_vnodeop_opv_desc =
	{ &fdesc_vnodeop_p, fdesc_vnodeop_entries };

extern const struct cdevsw ctty_cdevsw;

/*
 * Initialise cache headers
 */
void
fdesc_init()
{
	int cttymajor;

	/* locate the major number */
	cttymajor = cdevsw_lookup_major(&ctty_cdevsw);
	devctty = makedev(cttymajor, 0);
	fdhashtbl = hashinit(NFDCACHE, HASH_LIST, M_CACHE, M_NOWAIT, &fdhash);
}

/*
 * Free hash table.
 */
void
fdesc_done()
{
	hashdone(fdhashtbl, M_CACHE);
}

/*
 * Return a locked vnode of the correct type.
 */
int
fdesc_allocvp(ftype, ix, mp, vpp)
	fdntype ftype;
	int ix;
	struct mount *mp;
	struct vnode **vpp;
{
	struct fdhashhead *fc;
	struct fdescnode *fd;
	int error = 0;

	fc = FD_NHASH(ix);
loop:
	for (fd = fc->lh_first; fd != 0; fd = fd->fd_hash.le_next) {
		if (fd->fd_ix == ix && fd->fd_vnode->v_mount == mp) {
			if (vget(fd->fd_vnode, LK_EXCLUSIVE))
				goto loop;
			*vpp = fd->fd_vnode;
			return (error);
		}
	}

	/*
	 * otherwise lock the array while we call getnewvnode
	 * since that can block.
	 */
	if (fdcache_lock & FDL_LOCKED) {
		fdcache_lock |= FDL_WANT;
		(void) tsleep(&fdcache_lock, PINOD, "fdcache", 0);
		goto loop;
	}
	fdcache_lock |= FDL_LOCKED;

	error = getnewvnode(VT_FDESC, mp, fdesc_vnodeop_p, vpp);
	if (error)
		goto out;
	MALLOC(fd, void *, sizeof(struct fdescnode), M_TEMP, M_WAITOK);
	(*vpp)->v_data = fd;
	fd->fd_vnode = *vpp;
	fd->fd_type = ftype;
	fd->fd_fd = -1;
	fd->fd_link = 0;
	fd->fd_ix = ix;
	VOP_LOCK(*vpp, LK_EXCLUSIVE);
	LIST_INSERT_HEAD(fc, fd, fd_hash);

out:;
	fdcache_lock &= ~FDL_LOCKED;

	if (fdcache_lock & FDL_WANT) {
		fdcache_lock &= ~FDL_WANT;
		wakeup(&fdcache_lock);
	}

	return (error);
}

/*
 * vp is the current namei directory
 * ndp is the name to locate in that directory...
 */
int
fdesc_lookup(v)
	void *v;
{
	struct vop_lookup_args /* {
		struct vnode * a_dvp;
		struct vnode ** a_vpp;
		struct componentname * a_cnp;
	} */ *ap = v;
	struct vnode **vpp = ap->a_vpp;
	struct vnode *dvp = ap->a_dvp;
	struct componentname *cnp = ap->a_cnp;
	struct lwp *l = cnp->cn_lwp;
	const char *pname = cnp->cn_nameptr;
	struct proc *p = l->l_proc;
	int numfiles = p->p_fd->fd_nfiles;
	unsigned fd = 0;
	int error;
	struct vnode *fvp;
	const char *ln;

	if (cnp->cn_namelen == 1 && *pname == '.') {
		*vpp = dvp;
		VREF(dvp);
		return (0);
	}

	switch (VTOFDESC(dvp)->fd_type) {
	default:
	case Flink:
	case Fdesc:
	case Fctty:
		error = ENOTDIR;
		goto bad;

	case Froot:
		if (cnp->cn_namelen == 2 && memcmp(pname, "fd", 2) == 0) {
			error = fdesc_allocvp(Fdevfd, FD_DEVFD, dvp->v_mount, &fvp);
			if (error)
				goto bad;
			*vpp = fvp;
			fvp->v_type = VDIR;
			goto good;
		}

		if (cnp->cn_namelen == 3 && memcmp(pname, "tty", 3) == 0) {
			struct vnode *ttyvp = cttyvp(p);
			if (ttyvp == NULL) {
				error = ENXIO;
				goto bad;
			}
			error = fdesc_allocvp(Fctty, FD_CTTY, dvp->v_mount, &fvp);
			if (error)
				goto bad;
			*vpp = fvp;
			fvp->v_type = VCHR;
			goto good;
		}

		ln = 0;
		switch (cnp->cn_namelen) {
		case 5:
			if (memcmp(pname, "stdin", 5) == 0) {
				ln = "fd/0";
				fd = FD_STDIN;
			}
			break;
		case 6:
			if (memcmp(pname, "stdout", 6) == 0) {
				ln = "fd/1";
				fd = FD_STDOUT;
			} else
			if (memcmp(pname, "stderr", 6) == 0) {
				ln = "fd/2";
				fd = FD_STDERR;
			}
			break;
		}

		if (ln) {
			error = fdesc_allocvp(Flink, fd, dvp->v_mount, &fvp);
			if (error)
				goto bad;
			/* XXXUNCONST */
			VTOFDESC(fvp)->fd_link = __UNCONST(ln);
			*vpp = fvp;
			fvp->v_type = VLNK;
			goto good;
		} else {
			error = ENOENT;
			goto bad;
		}

		/* FALL THROUGH */

	case Fdevfd:
		if (cnp->cn_namelen == 2 && memcmp(pname, "..", 2) == 0) {
			VOP_UNLOCK(dvp, 0);
			cnp->cn_flags |= PDIRUNLOCK;
			error = fdesc_root(dvp->v_mount, vpp);
			if (error)
				goto bad;
			/*
			 * If we're at the last component and need the
			 * parent locked, undo the unlock above.
			 */
			if (((~cnp->cn_flags & (ISLASTCN | LOCKPARENT)) == 0) &&
				   ((error = vn_lock(dvp, LK_EXCLUSIVE)) == 0))
				cnp->cn_flags &= ~PDIRUNLOCK;
			return (error);
		}

		fd = 0;
		while (*pname >= '0' && *pname <= '9') {
			fd = 10 * fd + *pname++ - '0';
			if (fd >= numfiles)
				break;
		}

		if (*pname != '\0') {
			error = ENOENT;
			goto bad;
		}

		if (fd >= numfiles || p->p_fd->fd_ofiles[fd] == NULL ||
		    FILE_IS_USABLE(p->p_fd->fd_ofiles[fd]) == 0) {
			error = EBADF;
			goto bad;
		}

		error = fdesc_allocvp(Fdesc, FD_DESC+fd, dvp->v_mount, &fvp);
		if (error)
			goto bad;
		VTOFDESC(fvp)->fd_fd = fd;
		*vpp = fvp;
		goto good;
	}

bad:;
	*vpp = NULL;
	return (error);

good:;
	/*
	 * As "." was special cased above, we now unlock the parent if we're
	 * suppoed to. We're only supposed to not unlock if this is the
	 * last component, and the caller requested LOCKPARENT. So if either
	 * condition is false, unlock.
	 */
	if (((~cnp->cn_flags) & (ISLASTCN | LOCKPARENT)) != 0) {
		VOP_UNLOCK(dvp, 0);
		cnp->cn_flags |= PDIRUNLOCK;
	}
	return (0);
}

int
fdesc_open(v)
	void *v;
{
	struct vop_open_args /* {
		struct vnode *a_vp;
		int  a_mode;
		struct ucred *a_cred;
		struct lwp *a_l;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;

	switch (VTOFDESC(vp)->fd_type) {
	case Fdesc:
		/*
		 * XXX Kludge: set dupfd to contain the value of the
		 * the file descriptor being sought for duplication. The error
		 * return ensures that the vnode for this device will be
		 * released by vn_open. Open will detect this special error and
		 * take the actions in dupfdopen.  Other callers of vn_open or
		 * VOP_OPEN will simply report the error.
		 */
		curlwp->l_dupfd = VTOFDESC(vp)->fd_fd;	/* XXX */
		return EDUPFD;

	case Fctty:
		return ((*ctty_cdevsw.d_open)(devctty, ap->a_mode, 0, ap->a_l));
	case Froot:
	case Fdevfd:
	case Flink:
		break;
	}

	return (0);
}

static int
fdesc_attr(fd, vap, cred, l)
	int fd;
	struct vattr *vap;
	struct ucred *cred;
	struct lwp *l;
{
	struct proc *p = l->l_proc;
	struct filedesc *fdp = p->p_fd;
	struct file *fp;
	struct stat stb;
	int error;

	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);

	switch (fp->f_type) {
	case DTYPE_VNODE:
		simple_unlock(&fp->f_slock);
		error = VOP_GETATTR((struct vnode *) fp->f_data, vap, cred, l);
		if (error == 0 && vap->va_type == VDIR) {
			/*
			 * directories can cause loops in the namespace,
			 * so turn off the 'x' bits to avoid trouble.
			 */
			vap->va_mode &= ~(S_IXUSR|S_IXGRP|S_IXOTH);
		}
		break;

	default:
		FILE_USE(fp);
		memset(&stb, 0, sizeof(stb));
		error = (*fp->f_ops->fo_stat)(fp, &stb, l);
		FILE_UNUSE(fp, l);
		if (error)
			break;

		vattr_null(vap);
		switch(fp->f_type) {
		case DTYPE_SOCKET:
			vap->va_type = VSOCK;
			break;
		case DTYPE_PIPE:
			vap->va_type = VFIFO;
			break;
		default:
			/* use VNON perhaps? */
			vap->va_type = VBAD;
			break;
		}
		vap->va_mode = stb.st_mode;
		vap->va_nlink = stb.st_nlink;
		vap->va_uid = stb.st_uid;
		vap->va_gid = stb.st_gid;
		vap->va_fsid = stb.st_dev;
		vap->va_fileid = stb.st_ino;
		vap->va_size = stb.st_size;
		vap->va_blocksize = stb.st_blksize;
		vap->va_atime = stb.st_atimespec;
		vap->va_mtime = stb.st_mtimespec;
		vap->va_ctime = stb.st_ctimespec;
		vap->va_gen = stb.st_gen;
		vap->va_flags = stb.st_flags;
		vap->va_rdev = stb.st_rdev;
		vap->va_bytes = stb.st_blocks * stb.st_blksize;
		break;
	}

	return (error);
}

int
fdesc_getattr(v)
	void *v;
{
	struct vop_getattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct lwp *a_l;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
	struct vattr *vap = ap->a_vap;
	unsigned fd;
	int error = 0;

	switch (VTOFDESC(vp)->fd_type) {
	case Froot:
	case Fdevfd:
	case Flink:
	case Fctty:
		VATTR_NULL(vap);
		vap->va_fileid = VTOFDESC(vp)->fd_ix;

#define R_ALL (S_IRUSR|S_IRGRP|S_IROTH)
#define W_ALL (S_IWUSR|S_IWGRP|S_IWOTH)
#define X_ALL (S_IXUSR|S_IXGRP|S_IXOTH)

		switch (VTOFDESC(vp)->fd_type) {
		case Flink:
			vap->va_mode = R_ALL|X_ALL;
			vap->va_type = VLNK;
			vap->va_rdev = 0;
			vap->va_nlink = 1;
			vap->va_size = strlen(VTOFDESC(vp)->fd_link);
			break;

		case Fctty:
			vap->va_mode = R_ALL|W_ALL;
			vap->va_type = VCHR;
			vap->va_rdev = devctty;
			vap->va_nlink = 1;
			vap->va_size = 0;
			break;

		default:
			vap->va_mode = R_ALL|X_ALL;
			vap->va_type = VDIR;
			vap->va_rdev = 0;
			vap->va_nlink = 2;
			vap->va_size = DEV_BSIZE;
			break;
		}
		vap->va_uid = 0;
		vap->va_gid = 0;
		vap->va_fsid = vp->v_mount->mnt_stat.f_fsidx.__fsid_val[0];
		vap->va_blocksize = DEV_BSIZE;
		vap->va_atime.tv_sec = boottime.tv_sec;
		vap->va_atime.tv_nsec = 0;
		vap->va_mtime = vap->va_atime;
		vap->va_ctime = vap->va_mtime;
		vap->va_gen = 0;
		vap->va_flags = 0;
		vap->va_bytes = 0;
		break;

	case Fdesc:
		fd = VTOFDESC(vp)->fd_fd;
		error = fdesc_attr(fd, vap, ap->a_cred, ap->a_l);
		break;

	default:
		panic("fdesc_getattr");
		break;
	}

	if (error == 0)
		vp->v_type = vap->va_type;

	return (error);
}

int
fdesc_setattr(v)
	void *v;
{
	struct vop_setattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct lwp *a_l;
	} */ *ap = v;
	struct filedesc *fdp = ap->a_l->l_proc->p_fd;
	struct file *fp;
	unsigned fd;

	/*
	 * Can't mess with the root vnode
	 */
	switch (VTOFDESC(ap->a_vp)->fd_type) {
	case Fdesc:
		break;

	case Fctty:
		return (0);

	default:
		return (EACCES);
	}

	fd = VTOFDESC(ap->a_vp)->fd_fd;
	if ((fp = fd_getfile(fdp, fd)) == NULL)
		return (EBADF);

	/*
	 * XXX: Can't reasonably set the attr's on any types currently.
	 *      On vnode's this will cause truncation and socket/pipes make
	 *      no sense.
	 */
	simple_unlock(&fp->f_slock);
	return (0);
}


struct fdesc_target {
	ino_t ft_fileno;
	u_char ft_type;
	u_char ft_namlen;
	const char *ft_name;
} fdesc_targets[] = {
#define N(s) sizeof(s)-1, s
	{ FD_DEVFD,  DT_DIR,     N("fd")     },
	{ FD_STDIN,  DT_LNK,     N("stdin")  },
	{ FD_STDOUT, DT_LNK,     N("stdout") },
	{ FD_STDERR, DT_LNK,     N("stderr") },
	{ FD_CTTY,   DT_UNKNOWN, N("tty")    },
#undef N
#define UIO_MX _DIRENT_RECLEN((struct dirent *)NULL, sizeof("stderr") - 1)
};
static int nfdesc_targets = sizeof(fdesc_targets) / sizeof(fdesc_targets[0]);

int
fdesc_readdir(v)
	void *v;
{
	struct vop_readdir_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
		int *a_eofflag;
		off_t **a_cookies;
		int *a_ncookies;
	} */ *ap = v;
	struct uio *uio = ap->a_uio;
	struct dirent d;
	struct filedesc *fdp;
	off_t i;
	int j;
	int error;
	off_t *cookies = NULL;
	int ncookies;

	switch (VTOFDESC(ap->a_vp)->fd_type) {
	case Fctty:
		return 0;

	case Fdesc:
		return ENOTDIR;

	default:
		break;
	}

	fdp = curproc->p_fd;

	if (uio->uio_resid < UIO_MX)
		return EINVAL;
	if (uio->uio_offset < 0)
		return EINVAL;

	error = 0;
	i = uio->uio_offset;
	(void)memset(&d, 0, UIO_MX);
	d.d_reclen = UIO_MX;
	if (ap->a_ncookies)
		ncookies = uio->uio_resid / UIO_MX;
	else
		ncookies = 0;

	if (VTOFDESC(ap->a_vp)->fd_type == Froot) {
		struct fdesc_target *ft;

		if (i >= nfdesc_targets)
			return 0;

		if (ap->a_ncookies) {
			ncookies = min(ncookies, (nfdesc_targets - i));
			cookies = malloc(ncookies * sizeof(off_t),
			    M_TEMP, M_WAITOK);
			*ap->a_cookies = cookies;
			*ap->a_ncookies = ncookies;
		}

		for (ft = &fdesc_targets[i]; uio->uio_resid >= UIO_MX &&
		    i < nfdesc_targets; ft++, i++) {
			switch (ft->ft_fileno) {
			case FD_CTTY:
				if (cttyvp(curproc) == NULL)
					continue;
				break;

			case FD_STDIN:
			case FD_STDOUT:
			case FD_STDERR:
				if (fdp == NULL)
					continue;
				if ((ft->ft_fileno - FD_STDIN) >=
				    fdp->fd_nfiles)
					continue;
				if (fdp->fd_ofiles[ft->ft_fileno - FD_STDIN]
				    == NULL
				    || FILE_IS_USABLE(
				    fdp->fd_ofiles[ft->ft_fileno - FD_STDIN])
				    == 0)
					continue;
				break;
			}

			d.d_fileno = ft->ft_fileno;
			d.d_namlen = ft->ft_namlen;
			(void)memcpy(d.d_name, ft->ft_name, ft->ft_namlen + 1);
			d.d_type = ft->ft_type;

			if ((error = uiomove(&d, UIO_MX, uio)) != 0)
				break;
			if (cookies)
				*cookies++ = i + 1;
		}
	} else {
		int nfdp = fdp ? fdp->fd_nfiles : 0;
		if (ap->a_ncookies) {
			ncookies = min(ncookies, nfdp + 2);
			cookies = malloc(ncookies * sizeof(off_t),
			    M_TEMP, M_WAITOK);
			*ap->a_cookies = cookies;
			*ap->a_ncookies = ncookies;
		}
		for (; i - 2 < nfdp && uio->uio_resid >= UIO_MX; i++) {
			switch (i) {
			case 0:
			case 1:
				d.d_fileno = FD_ROOT;		/* XXX */
				d.d_namlen = i + 1;
				(void)memcpy(d.d_name, "..", d.d_namlen);
				d.d_name[i + 1] = '\0';
				d.d_type = DT_DIR;
				break;

			default:
				KASSERT(fdp != NULL);
				j = (int)i - 2;
				if (fdp == NULL || fdp->fd_ofiles[j] == NULL ||
				    FILE_IS_USABLE(fdp->fd_ofiles[j]) == 0)
					continue;
				d.d_fileno = j + FD_STDIN;
				d.d_namlen = sprintf(d.d_name, "%d", j);
				d.d_type = DT_UNKNOWN;
				break;
			}

			if ((error = uiomove(&d, UIO_MX, uio)) != 0)
				break;
			if (cookies)
				*cookies++ = i + 1;
		}
	}

	if (ap->a_ncookies && error) {
		free(*ap->a_cookies, M_TEMP);
		*ap->a_ncookies = 0;
		*ap->a_cookies = NULL;
	}

	uio->uio_offset = i;
	return error;
}

int
fdesc_readlink(v)
	void *v;
{
	struct vop_readlink_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
	int error;

	if (vp->v_type != VLNK)
		return (EPERM);

	if (VTOFDESC(vp)->fd_type == Flink) {
		char *ln = VTOFDESC(vp)->fd_link;
		error = uiomove(ln, strlen(ln), ap->a_uio);
	} else {
		error = EOPNOTSUPP;
	}

	return (error);
}

int
fdesc_read(v)
	void *v;
{
	struct vop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		struct ucred *a_cred;
	} */ *ap = v;
	int error = EOPNOTSUPP;
	struct vnode *vp = ap->a_vp;

	switch (VTOFDESC(vp)->fd_type) {
	case Fctty:
		VOP_UNLOCK(vp, 0);
		error = (*ctty_cdevsw.d_read)(devctty, ap->a_uio, ap->a_ioflag);
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
		break;

	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

int
fdesc_write(v)
	void *v;
{
	struct vop_write_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		struct ucred *a_cred;
	} */ *ap = v;
	int error = EOPNOTSUPP;
	struct vnode *vp = ap->a_vp;

	switch (VTOFDESC(vp)->fd_type) {
	case Fctty:
		VOP_UNLOCK(vp, 0);
		error = (*ctty_cdevsw.d_write)(devctty, ap->a_uio,
					       ap->a_ioflag);
		vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
		break;

	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

int
fdesc_ioctl(v)
	void *v;
{
	struct vop_ioctl_args /* {
		struct vnode *a_vp;
		u_long a_command;
		void *a_data;
		int  a_fflag;
		struct ucred *a_cred;
		struct lwp *a_l;
	} */ *ap = v;
	int error = EOPNOTSUPP;

	switch (VTOFDESC(ap->a_vp)->fd_type) {
	case Fctty:
		error = (*ctty_cdevsw.d_ioctl)(devctty, ap->a_command,
					       ap->a_data, ap->a_fflag,
					       ap->a_l);
		break;

	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

int
fdesc_poll(v)
	void *v;
{
	struct vop_poll_args /* {
		struct vnode *a_vp;
		int a_events;
		struct lwp *a_l;
	} */ *ap = v;
	int revents;

	switch (VTOFDESC(ap->a_vp)->fd_type) {
	case Fctty:
		revents = (*ctty_cdevsw.d_poll)(devctty, ap->a_events, ap->a_l);
		break;

	default:
		revents = genfs_poll(v);
		break;
	}

	return (revents);
}

int
fdesc_kqfilter(v)
	void *v;
{
	struct vop_kqfilter_args /* {
		struct vnode *a_vp;
		struct knote *a_kn;
	} */ *ap = v;
	int error;
	struct proc *p;
	struct lwp *l;
	struct file *fp;

	switch (VTOFDESC(ap->a_vp)->fd_type) {
	case Fctty:
		error = (*ctty_cdevsw.d_kqfilter)(devctty, ap->a_kn);
		break;

	case Fdesc:
		/* just invoke kqfilter for the underlying descriptor */
		l = curlwp;	/* XXX hopefully ok to use curproc here */
		p = l->l_proc;
		if ((fp = fd_getfile(p->p_fd, VTOFDESC(ap->a_vp)->fd_fd)) == NULL)
			return (1);

		FILE_USE(fp);
		error = (*fp->f_ops->fo_kqfilter)(fp, ap->a_kn);
		FILE_UNUSE(fp, l);
		break;

	default:
		return (genfs_kqfilter(v));
	}

	return (error);
}

int
fdesc_inactive(v)
	void *v;
{
	struct vop_inactive_args /* {
		struct vnode *a_vp;
		struct lwp *a_l;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;

	/*
	 * Clear out the v_type field to avoid
	 * nasty things happening in vgone().
	 */
	VOP_UNLOCK(vp, 0);
	vp->v_type = VNON;
	return (0);
}

int
fdesc_reclaim(v)
	void *v;
{
	struct vop_reclaim_args /* {
		struct vnode *a_vp;
	} */ *ap = v;
	struct vnode *vp = ap->a_vp;
	struct fdescnode *fd = VTOFDESC(vp);

	LIST_REMOVE(fd, fd_hash);
	FREE(vp->v_data, M_TEMP);
	vp->v_data = 0;

	return (0);
}

/*
 * Return POSIX pathconf information applicable to special devices.
 */
int
fdesc_pathconf(v)
	void *v;
{
	struct vop_pathconf_args /* {
		struct vnode *a_vp;
		int a_name;
		register_t *a_retval;
	} */ *ap = v;

	switch (ap->a_name) {
	case _PC_LINK_MAX:
		*ap->a_retval = LINK_MAX;
		return (0);
	case _PC_MAX_CANON:
		*ap->a_retval = MAX_CANON;
		return (0);
	case _PC_MAX_INPUT:
		*ap->a_retval = MAX_INPUT;
		return (0);
	case _PC_PIPE_BUF:
		*ap->a_retval = PIPE_BUF;
		return (0);
	case _PC_CHOWN_RESTRICTED:
		*ap->a_retval = 1;
		return (0);
	case _PC_VDISABLE:
		*ap->a_retval = _POSIX_VDISABLE;
		return (0);
	case _PC_SYNC_IO:
		*ap->a_retval = 1;
		return (0);
	default:
		return (EINVAL);
	}
	/* NOTREACHED */
}

/*
 * Print out the contents of a /dev/fd vnode.
 */
/* ARGSUSED */
int
fdesc_print(v)
	void *v;
{
	printf("tag VT_NON, fdesc vnode\n");
	return (0);
}

int
fdesc_link(v)
	void *v;
{
	struct vop_link_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap = v;

	VOP_ABORTOP(ap->a_dvp, ap->a_cnp);
	vput(ap->a_dvp);
	return (EROFS);
}

int
fdesc_symlink(v)
	void *v;
{
	struct vop_symlink_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
		char *a_target;
	} */ *ap = v;

	VOP_ABORTOP(ap->a_dvp, ap->a_cnp);
	vput(ap->a_dvp);
	return (EROFS);
}
