/*	$NetBSD: vmparam.h,v 1.1 2000/05/09 21:56:01 bjh21 Exp $	*/

/*
 * Copyright (c) 1988 The Regents of the University of California.
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
 */

#ifndef	_ARM32_VMPARAM_H_
#define	_ARM32_VMPARAM_H_

#define	USRTEXT		VM_MIN_ADDRESS
#define	USRSTACK	VM_MAXUSER_ADDRESS

#define	MAXTSIZ		(0x007f8000)		/* max text size */
#ifndef	DFLDSIZ
#define	DFLDSIZ		(0x00800000)		/* initial data size limit */
#endif
#ifndef	MAXDSIZ
#define	MAXDSIZ		(0x00800000)		/* max data size */
#endif
#ifndef	DFLSSIZ
#define	DFLSSIZ		(512*1024)		/* initial stack size limit */
#endif
#ifndef	MAXSSIZ
#define	MAXSSIZ		(0x00200000)		/* max stack size */
#endif

/*
 * Size of shared memory map
 */
#ifndef SHMMAXPGS
#define SHMMAXPGS       1024
#endif

/*
 * The time for a process to be blocked before being very swappable.
 * This is a number of seconds which the system takes as being a non-trivial
 * amount of real time.  You probably shouldn't change this;
 * it is used in subtle ways (fractions and multiples of it are, that is, like
 * half of a `long time'', almost a long time, etc.)
 * It is related to human patience and other factors which don't really
 * change over time.
 */
#define	MAXSLP		20

/*
 * Mach derived constants
 */

/* Need to link some of these with some in param.h */

/*
 * These specify the range of virtual pages available to user
 * processes and to the kernel.  This is a bit of a delicate balancing
 * act, as we've only got 32Mb between them.  There should probably be
 * an option to have separate kernel and user spaces to ease this.
 */

#define KVM_SIZE		0x00800000 /* 8Mb */

/* User VM range */
#define	VM_MIN_ADDRESS		((vm_offset_t)0x00008000)
#define	VM_MAX_ADDRESS		((vm_offset_t)0x02000000 - KVM_SIZE)
#define	VM_MAXUSER_ADDRESS	VM_MAX_ADDRESS

/* Kernel VM range */
#define	VM_MIN_KERNEL_ADDRESS	VM_MAX_ADDRESS
#define	VM_MAX_KERNEL_ADDRESS	((vm_offset_t)0x02000000)
#define	VM_MAXKERN_ADDRESS	VM_MAX_KERNEL_ADDRESS

/* XXX max. amount of KVM to be used by buffers. */
#ifndef VM_MAX_KERNEL_BUF
#define VM_MAX_KERNEL_BUF \
	((VM_MAX_KERNEL_ADDRESS - VM_MIN_KERNEL_ADDRESS) / 4)
#endif

/* Physical memory parameters */

#define VM_PHYSSEG_MAX		2

#define VM_PHYSSEG_STRAT	VM_PSTRAT_BIGFIRST

#define VM_PHYSSEG_NOADD	/* We won't turn up extra memory during autoconfig */

#define VM_NFREELIST		2
#define VM_FREELIST_LOW		1 /* DMA-able memory (bottom 512k phys) */
#define VM_FREELIST_DEFAULT	0 /* The rest */

struct pmap_physseg {
};

#endif

/* End of vmparam.h */
