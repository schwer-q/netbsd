/*	$NetBSD: irix_prctl.c,v 1.12 2002/06/05 17:27:11 manu Exp $ */

/*-
 * Copyright (c) 2001-2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emmanuel Dreyfus.
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
__KERNEL_RCSID(0, "$NetBSD: irix_prctl.c,v 1.12 2002/06/05 17:27:11 manu Exp $");

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/signalvar.h>
#include <sys/systm.h>
#include <sys/exec.h>
#include <sys/pool.h>
#include <sys/filedesc.h>
#include <sys/vnode.h>
#include <sys/resourcevar.h>

#include <uvm/uvm_extern.h>

#include <machine/regnum.h>
#include <machine/vmparam.h>

#include <compat/svr4/svr4_types.h>

#include <compat/irix/irix_types.h>
#include <compat/irix/irix_exec.h>
#include <compat/irix/irix_prctl.h>
#include <compat/irix/irix_signal.h>
#include <compat/irix/irix_syscallargs.h>

struct irix_sproc_child_args {
	struct proc **isc_proc; 
	void *isc_entry;
	void *isc_arg;
	void *isc_aux;
	int isc_inh;
	struct proc *isc_parent;
}; 
static void irix_sproc_child __P((struct irix_sproc_child_args *));
static int irix_sproc __P((void *, unsigned int, void *, caddr_t, size_t, 
    pid_t, struct proc *, register_t *));

int
irix_sys_prctl(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_prctl_args /* {
		syscallarg(int) option;
		syscallarg(void *) arg1;
	} */ *uap = v;
	int option = SCARG(uap, option);

#ifdef DEBUG_IRIX
	printf("irix_sys_prctl(): option = %d\n", option);
#endif

	switch(option) {
	case IRIX_PR_GETSHMASK:	{	/* Get shared resources */
		struct proc *p2;
		int shmask = 0;

		p2 = pfind((pid_t)SCARG(uap, arg1));

		if (p2 == p || SCARG(uap, arg1) == 0) {
			/* XXX return our own shmask */
			return 0;
		}

		if (p2 == NULL)
			return EINVAL;

		if (p->p_vmspace == p2->p_vmspace)
			shmask |= IRIX_PR_SADDR;
		if (p->p_fd == p2->p_fd)
			shmask |= IRIX_PR_SFDS;
		if (p->p_cwdi == p2->p_cwdi);
			shmask |= IRIX_PR_SDIR;

		*retval = (register_t)shmask;
		return 0;
		break;
	}
		
	case IRIX_PR_LASTSHEXIT: 	/* "Last sproc exit" */
		/* We do nothing */
		break;

	case IRIX_PR_GETNSHARE: {	/* Number of sproc share group memb.*/
		struct irix_emuldata *ied;
		struct proc *pp;
		struct proc *sharedparent;
		int count;

		ied = (struct irix_emuldata *)p->p_emuldata;
		sharedparent = ied->ied_sharedparent;
		if (sharedparent == NULL) {
			*retval = 0;
			return 0;
		}
			
		count = 0;
		LIST_FOREACH(pp, &allproc, p_list) {
			if (irix_check_exec(pp)) {
				ied = (struct irix_emuldata *)p->p_emuldata;
				if (ied->ied_sharedparent == sharedparent)
					count++;
			}
		}

		*retval = count;
		return 0;
		break;
	}

	case IRIX_PR_TERMCHILD: {	/* Send SIGHUP to children on exit */
		struct irix_emuldata *ied;

		ied = (struct irix_emuldata *)(p->p_emuldata);
		ied->ied_pptr = p->p_pptr;
		break;
	}

	case IRIX_PR_ISBLOCKED: {	/* Is process blocked? */
		pid_t pid = (pid_t)SCARG(uap, arg1);
		struct irix_emuldata *ied;
		struct proc *target;
		struct pcred *pc;

		if (pid == 0)
			pid = p->p_pid;

		if ((target = pfind(pid)) == NULL)
			return ESRCH;

		if (irix_check_exec(target) == 0)
			return 0;

		pc = p->p_cred;
		if (!(pc->pc_ucred->cr_uid == 0 || \
		    pc->p_ruid == target->p_cred->p_ruid || \
		    pc->pc_ucred->cr_uid == target->p_cred->p_ruid || \
		    pc->p_ruid == target->p_ucred->cr_uid || \
		    pc->pc_ucred->cr_uid == target->p_ucred->cr_uid))
			return EPERM;

		ied = (struct irix_emuldata *)(target->p_emuldata);
		*retval = (ied->ied_procblk_count < 0);
		return 0;
		break;
	}

	default:
		printf("Warning: call to unimplemented prctl() command %d\n",
		    option);
		return EINVAL;
		break;
	}
	return 0;
}


int
irix_sys_pidsprocsp(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_pidsprocsp_args /* {
		syscallarg(void *) entry;
		syscallarg(unsigned) inh;
		syscallarg(void *) arg;
		syscallarg(caddr_t) sp;
		syscallarg(irix_size_t) len;
		syscallarg(irix_pid_t) pid;
	} */ *uap = v;
	
	/* pid is ignored for now */
	printf("Warning: unsupported pid argument to IRIX sproc\n");

	return irix_sproc(SCARG(uap, entry), SCARG(uap, inh), SCARG(uap, arg),
	    SCARG(uap, sp), SCARG(uap, len), SCARG(uap, pid), p, retval);
}

int
irix_sys_sprocsp(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_sprocsp_args /* {
		syscallarg(void *) entry;
		syscallarg(unsigned) inh;
		syscallarg(void *) arg;
		syscallarg(caddr_t) sp;
		syscallarg(irix_size_t) len;
	} */ *uap = v;

	return irix_sproc(SCARG(uap, entry), SCARG(uap, inh), SCARG(uap, arg),
	    SCARG(uap, sp), SCARG(uap, len), 0, p, retval);
}

int
irix_sys_sproc(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_sproc_args /* {
		syscallarg(void *) entry;
		syscallarg(unsigned) inh;
		syscallarg(void *) arg;
	} */ *uap = v;
	
	return irix_sproc(SCARG(uap, entry), SCARG(uap, inh), SCARG(uap, arg),
	    NULL, 0, 0, p, retval);
}


static int 
irix_sproc(entry, inh, arg, sp, len, pid, p, retval)
	void *entry;
	unsigned int inh;
	void *arg;
	caddr_t sp;
	size_t len;
	pid_t pid;
	struct proc *p;
	register_t *retval;
{
	int bsd_flags = 0;
	struct exec_vmcmd vmc;
	struct frame *tf = (struct frame *)p->p_md.md_regs;
	int error;
	struct proc *p2;
	struct irix_sproc_child_args isc;	
	struct irix_emuldata *ied;

#ifdef DEBUG_IRIX
	printf("irix_sproc(): entry = %p, inh = %x, arg = %p, sp = 0x%08lx, len = 0x%08lx, pid = %d\n", entry, inh, arg, (u_long)sp, (u_long)len, pid);
#endif

	if (inh & IRIX_PR_SADDR)
		bsd_flags |= FORK_SHAREVM;
	if (inh & IRIX_PR_SFDS)
		bsd_flags |= FORK_SHAREFILES;
	if (inh & (IRIX_PR_SUMASK|IRIX_PR_SDIR)) {
		bsd_flags |= FORK_SHARECWD;
		/* Forget them so that we don't get warning below */
		inh &= ~(IRIX_PR_SUMASK|IRIX_PR_SDIR);
	}
	/* We know how to do PR_SUMASK and PR_SDIR together only */
	if (inh & IRIX_PR_SUMASK)
		printf("Warning: unimplemented IRIX sproc flag PR_SUMASK\n");
	if (inh & IRIX_PR_SDIR)
		printf("Warning: unimplemented IRIX sproc flag PR_SDIR\n");

	/* 
	 * Setting up child stack 
	 */
	if (inh & IRIX_PR_SADDR) {
		if (len == 0) 
			len = p->p_rlimit[RLIMIT_STACK].rlim_cur;
		if (sp == NULL)
			sp = (caddr_t)(round_page(tf->f_regs[SP]) 
			    - IRIX_SPROC_STACK_OFFSET - len);
			while (trunc_page((u_long)sp) >= 
			    (u_long)p->p_vmspace->vm_maxsaddr)
				sp -= IRIX_SPROC_STACK_OFFSET;

		bzero(&vmc, sizeof(vmc));
		vmc.ev_addr = trunc_page((u_long)sp);
		vmc.ev_len = round_page(len);
		vmc.ev_prot = UVM_PROT_RWX;
		vmc.ev_flags = UVM_FLAG_COPYONW|UVM_FLAG_FIXED|UVM_FLAG_OVERLAY;
		vmc.ev_proc = vmcmd_map_zero;
#ifdef DEBUG_IRIX
		printf("irix_sproc(): new stack addr=0x%08lx, len=0x%08lx\n", 
		    (u_long)sp, (u_long)len);
#endif
		if ((error = (*vmc.ev_proc)(p, &vmc)) != 0)
			return error;

		p->p_vmspace->vm_maxsaddr = (void *)trunc_page((u_long)sp);
	}

	/*
	 * Arguments for irix_sproc_child()
	 */
	isc.isc_proc = &p2;
	isc.isc_entry = entry;
	isc.isc_arg = arg;
	isc.isc_inh = inh;
	isc.isc_parent = p;
	if ((error = copyin((void *)(tf->f_regs[SP] + 28), 
	    &isc.isc_aux, sizeof(isc.isc_aux))) != 0)
		isc.isc_aux = 0;
	/*
	 * If revelant, initialize as the parent od the shared group
	 */
	ied = (struct irix_emuldata *)(p->p_emuldata);
	if (ied->ied_sharedparent == NULL)
		ied->ied_sharedparent = p;

	if ((error = fork1(p, bsd_flags, SIGCHLD, (void *)sp, len, 
	    (void *)irix_sproc_child, (void *)&isc, retval, &p2)) != 0)
		return error;

	/* 
	 * Some local variables are referenced in irix_sproc_child()
	 * through isc. We need to ensure the child does not use them
	 * anymore before leaving.
	 */
	(void)ltsleep((void *)&isc, 0, "sproc", 0, NULL);

	retval[0] = (register_t)p2->p_pid;
	retval[1] = 0;

	return 0;
}

static void
irix_sproc_child(isc)
	struct irix_sproc_child_args *isc;
{
	struct proc *p2 = *isc->isc_proc;
	struct frame *tf = (struct frame *)p2->p_md.md_regs;
	int inh = isc->isc_inh;
	struct proc *parent = isc->isc_parent;
	struct pcred *pc;
	struct plimit *pl;
	struct irix_emuldata *ied;
	struct irix_emuldata *parent_ied;

	/*
	 * Handle shared process UID/GID
	 */
	if (inh & IRIX_PR_SID) {
		pc = p2->p_cred;
		parent->p_cred->p_refcnt++;
		p2->p_cred = parent->p_cred;
		if (--pc->p_refcnt == 0) {
			crfree(pc->pc_ucred);	
			pool_put(&pcred_pool, pc);
		}
	}

	/* 
	 * Handle shared process limits
	 */
	if (inh & IRIX_PR_SULIMIT) {
		pl = p2->p_limit;
		parent->p_limit->p_refcnt++;
		p2->p_limit = parent->p_limit;
		if(--pl->p_refcnt == 0)
			limfree(pl);
	}
		
	/* 
	 * Setup PC to return to the child entry point 
	 */
	tf->f_regs[PC] = (unsigned long)isc->isc_entry;

	/* 
	 * Setup child arguments 
	 * The libc stub will copy S3 to A1 once we return to userland.
	 */
	tf->f_regs[A0] = (unsigned long)isc->isc_arg;
	tf->f_regs[A1] = (unsigned long)isc->isc_aux;
	tf->f_regs[S3] = (unsigned long)isc->isc_aux;

	/*
	 * Join the shared group
	 */
	ied = (struct irix_emuldata *)(p2->p_emuldata);
	parent_ied = (struct irix_emuldata *)(parent->p_emuldata);
	ied->ied_sharedparent = parent_ied->ied_sharedparent;

	/* 
	 * We do not need isc anymore, we can wakeup our parent
	 */
	wakeup((void *)isc);

	/*
	 * Return to userland for a newly created process
	 */
	child_return((void *)p2);
	return;
}

int
irix_sys_procblk(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_procblk_args /* {
		syscallarg(int) cmd;
		syscallarg(pid_t) pid;
		syscallarg(int) count;
	} */ *uap = v;
	int cmd = SCARG(uap, cmd);
	struct irix_emuldata *ied;
	struct proc *target;
	struct pcred *pc;
	int oldcount;
	int error, last_error;
	struct proc *pp;
	struct irix_emuldata *pp_ied;
	struct irix_sys_procblk_args cup;

	/* Find the process */
	if ((target = pfind(SCARG(uap, pid))) == NULL)
		return ESRCH;

	/* May we stop it? */
	pc = p->p_cred;
	if (!(pc->pc_ucred->cr_uid == 0 || \
	    pc->p_ruid == target->p_cred->p_ruid || \
	    pc->pc_ucred->cr_uid == target->p_cred->p_ruid || \
	    pc->p_ruid == target->p_ucred->cr_uid || \
	    pc->pc_ucred->cr_uid == target->p_ucred->cr_uid))
		return EPERM;

	/* Is it an IRIX process? */
	if (irix_check_exec(target) == 0)
		return EPERM;

	ied = (struct irix_emuldata *)(target->p_emuldata);
	oldcount = ied->ied_procblk_count;
	
	switch (cmd) {
	case IRIX_PROCBLK_BLOCK:
		ied->ied_procblk_count--;
		break;

	case IRIX_PROCBLK_UNBLOCK:
		ied->ied_procblk_count++;
		break;

	case IRIX_PROCBLK_COUNT:
		if (SCARG(uap, count) > IRIX_PR_MAXBLOCKCNT ||
		    SCARG(uap, count) < IRIX_PR_MINBLOCKCNT)
			return EINVAL;
		ied->ied_procblk_count = SCARG(uap, count);
		break;

	case IRIX_PROCBLK_BLOCKALL:
	case IRIX_PROCBLK_UNBLOCKALL:
	case IRIX_PROCBLK_COUNTALL:
		SCARG(&cup, cmd) = cmd -IRIX_PROCBLK_ONLYONE;
		SCARG(&cup, count) = SCARG(uap, count);
		last_error = 0;
		
		LIST_FOREACH(pp, &allproc, p_list) { 
			/* Select IRIX processes */
			if (irix_check_exec(target) == 0)
				continue;
			
			/* Is this process in the target shared group? */
			pp_ied = (struct irix_emuldata *)pp->p_emuldata;
			if (pp_ied->ied_sharedparent != ied->ied_sharedparent)
				continue;

			/* Recall procblk for this process */
			SCARG(&cup, pid) = pp->p_pid;
			if ((error = irix_sys_procblk(p, &cup, retval)) != 0)
				last_error = error;
		}
		return last_error;
		break;
	default:
		printf("Warning: unimplemented IRIX procblk command %d\n", cmd);
		return EINVAL;
		break;
	}

	/* 
	 * We emulate the process block/unblock using SIGSTOP and SIGCONT
	 * signals. This is not very accurate, since on IRIX theses way
	 * of blocking a process are completely separated.
	 */ 
	if (oldcount >= 0 && ied->ied_procblk_count < 0) /* blocked */
		psignal(target, SIGSTOP);

	if (oldcount < 0 && ied->ied_procblk_count >= 0) /* unblocked */
		psignal(target, SIGCONT);

	return 0;
}
