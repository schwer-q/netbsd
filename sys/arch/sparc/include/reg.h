/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)reg.h	8.1 (Berkeley) 6/11/93
 *
 * from: Header: reg.h,v 1.8 92/11/26 02:04:44 torek Exp 
 * $Id: reg.h,v 1.3 1994/02/11 16:51:35 pk Exp $
 */

#ifndef _MACHINE_REG_H_
#define	_MACHINE_REG_H_

/*
 * Registers passed to trap/syscall/etc.
 * This structure is known to occupy exactly 80 bytes (see locore.s).
 * Note, tf_global[0] is not actually written (since g0 is always 0).
 * (The slot tf_global[0] is used to send a copy of %wim to kernel gdb.
 * This is known as `cheating'.)
 */
struct trapframe {
	int	tf_psr;		/* psr */
	int	tf_pc;		/* return pc */
	int	tf_npc;		/* return npc */
	int	tf_y;		/* %y register */
	int	tf_global[8];	/* global registers in trap's caller */
	int	tf_out[8];	/* output registers in trap's caller */
};

/*
 * Register windows.  Each stack pointer (%o6 aka %sp) in each window
 * must ALWAYS point to some place at which it is safe to scribble on
 * 64 bytes.  (If not, your process gets mangled.)  Furthermore, each
 * stack pointer should be aligned on an 8-byte boundary (the kernel
 * as currently coded allows arbitrary alignment, but with a hefty
 * performance penalty).
 */
struct rwindow {
	int	rw_local[8];		/* %l0..%l7 */
	int	rw_in[8];		/* %i0..%i7 */
};

/*
 * Clone trapframe for now; this seems to be the more useful
 * than the old struct reg above.
 */
struct reg {
	int	r_psr;		/* psr */
	int	r_pc;		/* return pc */
	int	r_npc;		/* return npc */
	int	r_y;		/* %y register */
	int	r_global[8];	/* global registers in trap's caller */
	int	r_out[8];	/* output registers in trap's caller */
};

#include <machine/fsr.h>

/*
 * FP coprocessor registers.
 *
 * FP_QSIZE is the maximum coprocessor instruction queue depth
 * of any implementation on which the kernel will run.  David Hough:
 * ``I'd suggest allowing 16 ... allowing an indeterminate variable
 * size would be even better''.  Of course, we cannot do that; we
 * need to malloc these.
 */
#define	FP_QSIZE	16

struct fp_qentry {
	int	*fq_addr;		/* the instruction's address */
	int	fq_instr;		/* the instruction itself */
};
struct fpstate {
	u_int	fs_regs[32];		/* our view is 32 32-bit registers */
	int	fs_fsr;			/* %fsr */
	int	fs_qsize;		/* actual queue depth */
	struct	fp_qentry fs_queue[FP_QSIZE];	/* queue contents */
};

/*
 * Clone fpstate into an fpreg structure to satisfy <kern/sys_process.c>
 */
struct fpreg {
	u_int	fr_regs[32];		/* our view is 32 32-bit registers */
	int	fr_fsr;			/* %fsr */
	int	fr_qsize;		/* actual queue depth */
	struct	fp_qentry fr_queue[FP_QSIZE];	/* queue contents */
};

#endif /* _MACHINE_REG_H_ */
