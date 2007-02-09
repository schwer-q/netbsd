/*	$NetBSD: procfs_subr.c,v 1.75 2007/02/09 21:55:36 ad Exp $	*/

/*-
 * Copyright (c) 2006 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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

/*
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
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
 *	@(#)procfs_subr.c	8.6 (Berkeley) 5/14/95
 */

/*
 * Copyright (c) 1994 Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1993 Jan-Simon Pendry
 *
 * This code is derived from software contributed to Berkeley by
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
 *	@(#)procfs_subr.c	8.6 (Berkeley) 5/14/95
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: procfs_subr.c,v 1.75 2007/02/09 21:55:36 ad Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/malloc.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/kauth.h>

#include <miscfs/procfs/procfs.h>

void procfs_hashins(struct pfsnode *);
void procfs_hashrem(struct pfsnode *);
struct vnode *procfs_hashget(pid_t, pfstype, int, struct mount *);

LIST_HEAD(pfs_hashhead, pfsnode) *pfs_hashtbl;
u_long	pfs_ihash;	/* size of hash table - 1 */
#define PFSPIDHASH(pid)	((pid) & pfs_ihash)

kmutex_t pfs_hashmutex;
struct simplelock pfs_hash_slock;

#define	ISSET(t, f)	((t) & (f))

/*
 * allocate a pfsnode/vnode pair.  the vnode is
 * referenced, and locked.
 *
 * the pid, pfs_type, and mount point uniquely
 * identify a pfsnode.  the mount point is needed
 * because someone might mount this filesystem
 * twice.
 *
 * all pfsnodes are maintained on a singly-linked
 * list.  new nodes are only allocated when they cannot
 * be found on this list.  entries on the list are
 * removed when the vfs reclaim entry is called.
 *
 * a single lock is kept for the entire list.  this is
 * needed because the getnewvnode() function can block
 * waiting for a vnode to become free, in which case there
 * may be more than one process trying to get the same
 * vnode.  this lock is only taken if we are going to
 * call getnewvnode, since the kernel itself is single-threaded.
 *
 * if an entry is found on the list, then call vget() to
 * take a reference.  this is done because there may be
 * zero references to it and so it needs to removed from
 * the vnode free list.
 */
int
procfs_allocvp(mp, vpp, pid, pfs_type, fd, p)
	struct mount *mp;
	struct vnode **vpp;
	pid_t pid;
	pfstype pfs_type;
	int fd;
	struct proc *p;
{
	struct pfsnode *pfs;
	struct vnode *vp;
	int error;

	do {
		if ((*vpp = procfs_hashget(pid, pfs_type, fd, mp)) != NULL)
			return (0);
	} while (!mutex_tryenter(&pfs_hashmutex));

	if ((error = getnewvnode(VT_PROCFS, mp, procfs_vnodeop_p, &vp)) != 0) {
		*vpp = NULL;
		mutex_exit(&pfs_hashmutex);
		return (error);
	}

	MALLOC(pfs, void *, sizeof(struct pfsnode), M_TEMP, M_WAITOK);
	vp->v_data = pfs;

	pfs->pfs_pid = pid;
	pfs->pfs_type = pfs_type;
	pfs->pfs_vnode = vp;
	pfs->pfs_flags = 0;
	pfs->pfs_fileno = PROCFS_FILENO(pid, pfs_type, fd);
	pfs->pfs_fd = fd;

	switch (pfs_type) {
	case PFSroot:	/* /proc = dr-xr-xr-x */
		vp->v_flag = VROOT;
		/*FALLTHROUGH*/
	case PFSproc:	/* /proc/N = dr-xr-xr-x */
		pfs->pfs_mode = S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
		vp->v_type = VDIR;
		break;

	case PFScurproc:	/* /proc/curproc = lr-xr-xr-x */
	case PFSself:	/* /proc/self    = lr-xr-xr-x */
	case PFScwd:	/* /proc/N/cwd = lr-xr-xr-x */
	case PFSchroot:	/* /proc/N/chroot = lr-xr-xr-x */
	case PFSexe:	/* /proc/N/exe = lr-xr-xr-x */
		pfs->pfs_mode = S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
		vp->v_type = VLNK;
		break;

	case PFSfd:
		if (fd == -1) {	/* /proc/N/fd = dr-xr-xr-x */
			pfs->pfs_mode = S_IRUSR|S_IXUSR;
			vp->v_type = VDIR;
		} else {	/* /proc/N/fd/M = [ps-]rw------- */
			struct file *fp;
			struct vnode *vxp;

			mutex_enter(&p->p_mutex);
			fp = fd_getfile(p->p_fd, pfs->pfs_fd);
			mutex_exit(&p->p_mutex);
			if (fp == NULL) {
				error = EBADF;
				goto bad;
			}
			FILE_USE(fp);

			pfs->pfs_mode = S_IRUSR|S_IWUSR;
			switch (fp->f_type) {
			case DTYPE_VNODE:
				vxp = (struct vnode *)fp->f_data;

				/*
				 * We make symlinks for directories
				 * to avoid cycles.
				 */
				if (vxp->v_type == VDIR)
					goto symlink;
				vp->v_type = vxp->v_type;
				break;
			case DTYPE_PIPE:
				vp->v_type = VFIFO;
				break;
			case DTYPE_SOCKET:
				vp->v_type = VSOCK;
				break;
			case DTYPE_KQUEUE:
			case DTYPE_MISC:
			symlink:
				pfs->pfs_mode = S_IRUSR|S_IXUSR|S_IRGRP|
				    S_IXGRP|S_IROTH|S_IXOTH;
				vp->v_type = VLNK;
				break;
			default:
				error = EOPNOTSUPP;
				FILE_UNUSE(fp, curlwp);
				goto bad;
			}
			FILE_UNUSE(fp, curlwp);
		}
		break;

	case PFSfile:	/* /proc/N/file = -rw------- */
	case PFSmem:	/* /proc/N/mem = -rw------- */
	case PFSregs:	/* /proc/N/regs = -rw------- */
	case PFSfpregs:	/* /proc/N/fpregs = -rw------- */
		pfs->pfs_mode = S_IRUSR|S_IWUSR;
		vp->v_type = VREG;
		break;

	case PFSctl:	/* /proc/N/ctl = --w------ */
	case PFSnote:	/* /proc/N/note = --w------ */
	case PFSnotepg:	/* /proc/N/notepg = --w------ */
		pfs->pfs_mode = S_IWUSR;
		vp->v_type = VREG;
		break;

	case PFSmap:	/* /proc/N/map = -r--r--r-- */
	case PFSmaps:	/* /proc/N/maps = -r--r--r-- */
	case PFSstatus:	/* /proc/N/status = -r--r--r-- */
	case PFSstat:	/* /proc/N/stat = -r--r--r-- */
	case PFScmdline:	/* /proc/N/cmdline = -r--r--r-- */
	case PFSemul:	/* /proc/N/emul = -r--r--r-- */
	case PFSmeminfo:	/* /proc/meminfo = -r--r--r-- */
	case PFSdevices:	/* /proc/devices = -r--r--r-- */
	case PFScpuinfo:	/* /proc/cpuinfo = -r--r--r-- */
	case PFSuptime:	/* /proc/uptime = -r--r--r-- */
	case PFSmounts:	/* /proc/mounts = -r--r--r-- */
		pfs->pfs_mode = S_IRUSR|S_IRGRP|S_IROTH;
		vp->v_type = VREG;
		break;

#ifdef __HAVE_PROCFS_MACHDEP
	PROCFS_MACHDEP_NODETYPE_CASES
		procfs_machdep_allocvp(vp);
		break;
#endif

	default:
		panic("procfs_allocvp");
	}

	procfs_hashins(pfs);
	uvm_vnp_setsize(vp, 0);
	mutex_exit(&pfs_hashmutex);

	*vpp = vp;
	return (0);

 bad:
	mutex_exit(&pfs_hashmutex);
	FREE(pfs, M_TEMP);
	ungetnewvnode(vp);
	return (error);
}

int
procfs_freevp(vp)
	struct vnode *vp;
{
	struct pfsnode *pfs = VTOPFS(vp);

	procfs_hashrem(pfs);

	FREE(vp->v_data, M_TEMP);
	vp->v_data = 0;
	return (0);
}

int
procfs_rw(v)
	void *v;
{
	struct vop_read_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct uio *uio = ap->a_uio;
	struct lwp *curl;
	struct lwp *l;
	struct pfsnode *pfs = VTOPFS(vp);
	struct proc *p;
	int error;

	if (uio->uio_offset < 0)
		return EINVAL;

	if ((error = procfs_proc_lock(pfs->pfs_pid, &p, ESRCH)) != 0)
		return error;

	curl = curlwp;

	/*
	 * Do not allow init to be modified while in secure mode; it
	 * could be duped into changing the security level.
	 */
#define	M2K(m)	((m) == UIO_READ ? KAUTH_REQ_PROCESS_CANPROCFS_READ : \
		 KAUTH_REQ_PROCESS_CANPROCFS_WRITE)
	mutex_enter(&p->p_mutex);
	error = kauth_authorize_process(curl->l_cred, KAUTH_PROCESS_CANPROCFS,
	    p, pfs, KAUTH_ARG(M2K(uio->uio_rw)), NULL);
	mutex_exit(&p->p_mutex);
	if (error) {
		procfs_proc_unlock(p);
		return (error);
	}
#undef	M2K

	mutex_enter(&p->p_smutex);
	l = proc_representative_lwp(p, NULL, 1);
	lwp_addref(l);
	mutex_exit(&p->p_smutex);

	switch (pfs->pfs_type) {
	case PFSnote:
	case PFSnotepg:
		error = procfs_donote(curl, p, pfs, uio);
		break;

	case PFSregs:
		error = procfs_doregs(curl, l, pfs, uio);
		break;

	case PFSfpregs:
		error = procfs_dofpregs(curl, l, pfs, uio);
		break;

	case PFSctl:
		error = procfs_doctl(curl, l, pfs, uio);
		break;

	case PFSstatus:
		error = procfs_dostatus(curl, l, pfs, uio);
		break;

	case PFSstat:
		error = procfs_do_pid_stat(curl, l, pfs, uio);
		break;

	case PFSmap:
		error = procfs_domap(curl, p, pfs, uio, 0);
		break;

	case PFSmaps:
		error = procfs_domap(curl, p, pfs, uio, 1);
		break;

	case PFSmem:
		error = procfs_domem(curl, l, pfs, uio);
		break;

	case PFScmdline:
		error = procfs_docmdline(curl, p, pfs, uio);
		break;

	case PFSmeminfo:
		error = procfs_domeminfo(curl, p, pfs, uio);
		break;

	case PFSdevices:
		error = procfs_dodevices(curl, p, pfs, uio);
		break;

	case PFScpuinfo:
		error = procfs_docpuinfo(curl, p, pfs, uio);
		break;

	case PFSfd:
		error = procfs_dofd(curl, p, pfs, uio);
		break;

	case PFSuptime:
		error = procfs_douptime(curl, p, pfs, uio);
		break;

	case PFSmounts:
		error = procfs_domounts(curl, p, pfs, uio);
		break;

	case PFSemul:
		error = procfs_doemul(curl, p, pfs, uio);
		break;

#ifdef __HAVE_PROCFS_MACHDEP
	PROCFS_MACHDEP_NODETYPE_CASES
		error = procfs_machdep_rw(curl, l, pfs, uio);
		break;
#endif

	default:
		error = EOPNOTSUPP;
		break;
	}

	/*
	 * Release the references that we acquired earlier.
	 */
	lwp_delref(l);
	procfs_proc_unlock(p);

	return (error);
}

/*
 * Get a string from userland into (bf).  Strip a trailing
 * nl character (to allow easy access from the shell).
 * The buffer should be *buflenp + 1 chars long.  vfs_getuserstr
 * will automatically add a nul char at the end.
 *
 * Returns 0 on success or the following errors
 *
 * EINVAL:    file offset is non-zero.
 * EMSGSIZE:  message is longer than kernel buffer
 * EFAULT:    user i/o buffer is not addressable
 */
int
vfs_getuserstr(uio, bf, buflenp)
	struct uio *uio;
	char *bf;
	int *buflenp;
{
	int xlen;
	int error;

	if (uio->uio_offset != 0)
		return (EINVAL);

	xlen = *buflenp;

	/* must be able to read the whole string in one go */
	if (xlen < uio->uio_resid)
		return (EMSGSIZE);
	xlen = uio->uio_resid;

	if ((error = uiomove(bf, xlen, uio)) != 0)
		return (error);

	/* allow multiple writes without seeks */
	uio->uio_offset = 0;

	/* cleanup string and remove trailing newline */
	bf[xlen] = '\0';
	xlen = strlen(bf);
	if (xlen > 0 && bf[xlen-1] == '\n')
		bf[--xlen] = '\0';
	*buflenp = xlen;

	return (0);
}

const vfs_namemap_t *
vfs_findname(nm, bf, buflen)
	const vfs_namemap_t *nm;
	const char *bf;
	int buflen;
{

	for (; nm->nm_name; nm++)
		if (memcmp(bf, nm->nm_name, buflen+1) == 0)
			return (nm);

	return (0);
}

/*
 * Initialize pfsnode hash table.
 */
void
procfs_hashinit()
{
	mutex_init(&pfs_hashmutex, MUTEX_DEFAULT, IPL_NONE);
	pfs_hashtbl = hashinit(desiredvnodes / 4, HASH_LIST, M_UFSMNT,
	    M_WAITOK, &pfs_ihash);
	simple_lock_init(&pfs_hash_slock);
}

void
procfs_hashreinit()
{
	struct pfsnode *pp;
	struct pfs_hashhead *oldhash, *hash;
	u_long i, oldmask, mask, val;

	hash = hashinit(desiredvnodes / 4, HASH_LIST, M_UFSMNT, M_WAITOK,
	    &mask);

	simple_lock(&pfs_hash_slock);
	oldhash = pfs_hashtbl;
	oldmask = pfs_ihash;
	pfs_hashtbl = hash;
	pfs_ihash = mask;
	for (i = 0; i <= oldmask; i++) {
		while ((pp = LIST_FIRST(&oldhash[i])) != NULL) {
			LIST_REMOVE(pp, pfs_hash);
			val = PFSPIDHASH(pp->pfs_pid);
			LIST_INSERT_HEAD(&hash[val], pp, pfs_hash);
		}
	}
	simple_unlock(&pfs_hash_slock);
	hashdone(oldhash, M_UFSMNT);
}

/*
 * Free pfsnode hash table.
 */
void
procfs_hashdone()
{
	hashdone(pfs_hashtbl, M_UFSMNT);
}

struct vnode *
procfs_hashget(pid, type, fd, mp)
	pid_t pid;
	pfstype type;
	int fd;
	struct mount *mp;
{
	struct pfs_hashhead *ppp;
	struct pfsnode *pp;
	struct vnode *vp;

loop:
	simple_lock(&pfs_hash_slock);
	ppp = &pfs_hashtbl[PFSPIDHASH(pid)];
	LIST_FOREACH(pp, ppp, pfs_hash) {
		vp = PFSTOV(pp);
		if (pid == pp->pfs_pid && pp->pfs_type == type &&
		    pp->pfs_fd == fd && vp->v_mount == mp) {
			simple_lock(&vp->v_interlock);
			simple_unlock(&pfs_hash_slock);
			if (vget(vp, LK_EXCLUSIVE | LK_INTERLOCK))
				goto loop;
			return (vp);
		}
	}
	simple_unlock(&pfs_hash_slock);
	return (NULL);
}

/*
 * Insert the pfsnode into the hash table and lock it.
 */
void
procfs_hashins(pp)
	struct pfsnode *pp;
{
	struct pfs_hashhead *ppp;

	/* lock the pfsnode, then put it on the appropriate hash list */
	lockmgr(&pp->pfs_vnode->v_lock, LK_EXCLUSIVE, (struct simplelock *)0);

	simple_lock(&pfs_hash_slock);
	ppp = &pfs_hashtbl[PFSPIDHASH(pp->pfs_pid)];
	LIST_INSERT_HEAD(ppp, pp, pfs_hash);
	simple_unlock(&pfs_hash_slock);
}

/*
 * Remove the pfsnode from the hash table.
 */
void
procfs_hashrem(pp)
	struct pfsnode *pp;
{
	simple_lock(&pfs_hash_slock);
	LIST_REMOVE(pp, pfs_hash);
	simple_unlock(&pfs_hash_slock);
}

void
procfs_revoke_vnodes(p, arg)
	struct proc *p;
	void *arg;
{
	struct pfsnode *pfs, *pnext;
	struct vnode *vp;
	struct mount *mp = (struct mount *)arg;
	struct pfs_hashhead *ppp;

	if (!(p->p_flag & P_SUGID))
		return;

	ppp = &pfs_hashtbl[PFSPIDHASH(p->p_pid)];
	for (pfs = LIST_FIRST(ppp); pfs; pfs = pnext) {
		vp = PFSTOV(pfs);
		pnext = LIST_NEXT(pfs, pfs_hash);
		if (vp->v_usecount > 0 && pfs->pfs_pid == p->p_pid &&
		    vp->v_mount == mp)
			VOP_REVOKE(vp, REVOKEALL);
	}
}

int
procfs_proc_lock(int pid, struct proc **bunghole, int notfound)
{
	struct proc *tp;
	int error = 0;

	rw_enter(&proclist_lock, RW_READER);

	if (pid == 0)
		tp = &proc0;
	else if ((tp = p_find(pid, PFIND_LOCKED)) == NULL)
		error = notfound;

	if (tp != NULL) {
		mutex_enter(&tp->p_mutex);
		error = proc_addref(tp);
		mutex_exit(&tp->p_mutex);
	}

	rw_exit(&proclist_lock);

	*bunghole = tp;
	return error;
}

void
procfs_proc_unlock(struct proc *p)
{
	mutex_enter(&p->p_mutex);
	proc_delref(p);
	mutex_exit(&p->p_mutex);
}

int
procfs_doemul(struct lwp *curl, struct proc *p,
    struct pfsnode *pfs, struct uio *uio)
{
	const char *ename = p->p_emul->e_name;
	return uiomove_frombuf(__UNCONST(ename), strlen(ename), uio);
}
