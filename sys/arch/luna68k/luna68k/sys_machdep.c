/* $NetBSD: sys_machdep.c,v 1.4 2000/12/13 18:13:07 jdolecek Exp $ */

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)sys_machdep.c	8.2 (Berkeley) 1/13/94
 */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: sys_machdep.c,v 1.4 2000/12/13 18:13:07 jdolecek Exp $");

#include "opt_compat_hpux.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/buf.h>
#include <sys/mount.h>

#include <uvm/uvm_extern.h>

#include <sys/syscallargs.h>

#include <machine/cpu.h>
#include <m68k/cacheops_30.h>
#include <m68k/cacheops_40.h>

#include <machine/cpu.h>

/* XXX should be in an include file somewhere */
#define CC_PURGE	1
#define CC_FLUSH	2
#define CC_IPURGE	4
#define CC_EXTPURGE	0x80000000
/* XXX end should be */

/*
 * Note that what we do here for a 68040 is different than HP-UX.
 *
 * In 'pux they either act on a line (len == 16), a page (len == NBPG)
 * or the whole cache (len == anything else).
 *
 * In BSD we attempt to be more optimal when acting on "odd" sizes.
 * For lengths up to 1024 we do all affected lines, up to 2*NBPG we
 * do pages, above that we do the entire cache.
 */
/*ARGSUSED1*/
int
cachectl1(req, addr, len, p)
	unsigned long req;
	vaddr_t	addr;
	size_t len;
	struct proc *p;
{
	int error = 0;

#if defined(M68040)
	if (mmutype == MMU_68040) {
		int inc = 0, doall = 0;
		paddr_t pa = 0;
		vaddr_t end;

		if (addr == 0 ||
		    ((req & ~CC_EXTPURGE) != CC_PURGE && len > 2*NBPG))
			doall = 1;

		if (!doall) {
			end = addr + len;
			if (len <= 1024) {
				addr = addr & ~0xF;
				inc = 16;
			} else {
				addr = addr & ~PGOFSET;
				inc = NBPG;
			}
		}
		do {
			/*
			 * Convert to physical address if needed.
			 * If translation fails, we perform operation on
			 * entire cache (XXX is this a rational thing to do?)
			 */
			if (!doall &&
			    (pa == 0 || ((int)addr & PGOFSET) == 0)) {
				if (pmap_extract(p->p_vmspace->vm_map.pmap,
				    addr, &pa) == FALSE)
					doall = 1;
			}
			switch (req) {
			case CC_EXTPURGE|CC_IPURGE:
			case CC_IPURGE:
				if (doall) {
					DCFA_40();
					ICPA_40();
				} else if (inc == 16) {
					DCFL_40(pa);
					ICPL_40(pa);
				} else if (inc == NBPG) {
					DCFP_40(pa);
					ICPP_40(pa);
				}
				break;
			
			case CC_EXTPURGE|CC_PURGE:
			case CC_PURGE:
				if (doall)
					DCFA_40(); /* note: flush not purge */
				else if (inc == 16)
					DCPL_40(pa);
				else if (inc == NBPG)
					DCPP_40(pa);
				break;

			case CC_EXTPURGE|CC_FLUSH:
			case CC_FLUSH:
				if (doall)
					DCFA_40();
				else if (inc == 16)
					DCFL_40(pa);
				else if (inc == NBPG)
					DCFP_40(pa);
				break;
				
			default:
				error = EINVAL;
				break;
			}
			if (doall)
				break;
			pa += inc;
			addr += inc;
		} while (addr < end);
		return(error);
	}
#endif
/* see descriptions in <m68k/include/cacheops_30.h> */
#define	DCIU()
#define	DCIA()
#define	ICIA() ICIA_30()
	switch (req) {
	case CC_EXTPURGE|CC_PURGE:
	case CC_EXTPURGE|CC_FLUSH:
	case CC_PURGE:
	case CC_FLUSH:
		DCIU();
		break;
	case CC_EXTPURGE|CC_IPURGE:
		DCIU();
		/* fall into... */
	case CC_IPURGE:
		ICIA();
		break;
	default:
		error = EINVAL;
		break;
	}
	return(error);
}

/*
 * DMA cache control
 */

/*ARGSUSED1*/
int
dma_cachectl(addr, len)
	caddr_t	addr;
	int len;
{
#if defined(M68040)
	if (mmutype == MMU_68040) {
		int inc = 0;
		int pa = 0;
		caddr_t end;

		end = addr + len;
		if (len <= 1024) {
			addr = (caddr_t)((int)addr & ~0xF);
			inc = 16;
		} else {
			addr = (caddr_t)((int)addr & ~PGOFSET);
			inc = NBPG;
		}
		do {
			/*
			 * Convert to physical address.
			 */
			if (pa == 0 || ((int)addr & PGOFSET) == 0) {
				pa = kvtop(addr);
			}
			if (inc == 16) {
				DCFL_40(pa);
				ICPL_40(pa);
			} else {
				DCFP_40(pa);
				ICPP_40(pa);
			}
			pa += inc;
			addr += inc;
		} while (addr < end);
	}
#endif	/* M68040 */
	return(0);
}

int
sys_sysarch(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
#if 0 /* unused */
	struct sys_sysarch_args /* {
		syscallarg(int) op; 
		syscallarg(void *) parms;
	} */ *uap = v;
#endif

	return (ENOSYS);
}
