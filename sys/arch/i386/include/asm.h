/*	$NetBSD: asm.h,v 1.26 2003/08/07 16:27:57 agc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	@(#)asm.h	5.5 (Berkeley) 5/7/91
 */

#ifndef _I386_ASM_H_
#define _I386_ASM_H_

#ifdef _KERNEL_OPT
#include "opt_multiprocessor.h"
#endif

#ifdef PIC
#define PIC_PROLOGUE	\
	pushl	%ebx;	\
	call	1f;	\
1:			\
	popl	%ebx;	\
	addl	$_GLOBAL_OFFSET_TABLE_+[.-1b], %ebx
#define PIC_EPILOGUE	\
	popl	%ebx
#define PIC_PLT(x)	x@PLT
#define PIC_GOT(x)	x@GOT(%ebx)
#define PIC_GOTOFF(x)	x@GOTOFF(%ebx)
#else
#define PIC_PROLOGUE
#define PIC_EPILOGUE
#define PIC_PLT(x)	x
#define PIC_GOT(x)	x
#define PIC_GOTOFF(x)	x
#endif

#ifdef __ELF__
# define _C_LABEL(x)	x
#else
# ifdef __STDC__
#  define _C_LABEL(x)	_ ## x
# else
#  define _C_LABEL(x)	_/**/x
# endif
#endif
#define	_ASM_LABEL(x)	x

#define CVAROFF(x, y)		_C_LABEL(x) + y

#ifdef __STDC__
# define __CONCAT(x,y)	x ## y
# define __STRING(x)	#x
#else
# define __CONCAT(x,y)	x/**/y
# define __STRING(x)	"x"
#endif

/* let kernels and others override entrypoint alignment */
#if !defined(_ALIGN_TEXT) && !defined(_KERNEL)
# ifdef __ELF__
#  define _ALIGN_TEXT .align 4
# else
#  define _ALIGN_TEXT .align 2
# endif
#endif

#define _ENTRY(x) \
	.text; _ALIGN_TEXT; .globl x; .type x,@function; x:

#ifdef _KERNEL

#if defined(MULTIPROCESSOR)
#define CPUVAR(off) %fs:__CONCAT(CPU_INFO_,off)
#else
#define CPUVAR(off) _C_LABEL(cpu_info_primary)+__CONCAT(CPU_INFO_,off)
#endif /* MULTIPROCESSOR */

/* XXX Can't use __CONCAT() here, as it would be evaluated incorrectly. */
#ifdef __ELF__
#ifdef __STDC__
#define	IDTVEC(name) \
	ALIGN_TEXT; .globl X ## name; .type X ## name,@function; X ## name:
#else 
#define	IDTVEC(name) \
	ALIGN_TEXT; .globl X/**/name; .type X/**/name,@function; X/**/name:
#endif /* __STDC__ */ 
#else 
#ifdef __STDC__
#define	IDTVEC(name) \
	ALIGN_TEXT; .globl _X ## name; .type _X ## name,@function; _X ## name: 
#else
#define	IDTVEC(name) \
	ALIGN_TEXT; .globl _X/**/name; .type _X/**/name,@function; _X/**/name:
#endif /* __STDC__ */
#endif /* __ELF__ */

#ifdef __ELF__
#define ALIGN_DATA	.align	4
#define ALIGN_TEXT	.align	4,0x90  /* 4-byte boundaries, NOP-filled */
#define SUPERALIGN_TEXT	.align	16,0x90 /* 16-byte boundaries better for 486 */
#else
#define ALIGN_DATA	.align	2
#define ALIGN_TEXT	.align	2,0x90  /* 4-byte boundaries, NOP-filled */
#define SUPERALIGN_TEXT	.align	4,0x90  /* 16-byte boundaries better for 486 */
#endif /* __ELF__ */

#define _ALIGN_TEXT ALIGN_TEXT

#endif /* _KERNEL */



#ifdef GPROF
# ifdef __ELF__
#  define _PROF_PROLOGUE	\
	pushl %ebp; movl %esp,%ebp; call PIC_PLT(__mcount); popl %ebp
# else 
#  define _PROF_PROLOGUE	\
	pushl %ebp; movl %esp,%ebp; call PIC_PLT(mcount); popl %ebp
# endif
#else
# define _PROF_PROLOGUE
#endif

#define	ENTRY(y)	_ENTRY(_C_LABEL(y)); _PROF_PROLOGUE
#define	NENTRY(y)	_ENTRY(_C_LABEL(y))
#define	ASENTRY(y)	_ENTRY(_ASM_LABEL(y)); _PROF_PROLOGUE

#define	ASMSTR		.asciz

#ifdef __ELF__
#define RCSID(x)	.section ".ident"; .asciz x
#else
#define RCSID(x)	.text; .asciz x
#endif

#ifdef NO_KERNEL_RCSIDS
#define	__KERNEL_RCSID(_n, _s)	/* nothing */
#else
#define	__KERNEL_RCSID(_n, _s)	RCSID(_s)
#endif

#ifdef __ELF__
#define	WEAK_ALIAS(alias,sym)						\
	.weak alias;							\
	alias = sym
#endif

#ifdef __STDC__
#define	WARN_REFERENCES(sym,msg)					\
	.stabs msg ## ,30,0,0,0 ;					\
	.stabs __STRING(_C_LABEL(sym)) ## ,1,0,0,0
#elif defined(__ELF__)
#define	WARN_REFERENCES(sym,msg)					\
	.stabs msg,30,0,0,0 ;						\
	.stabs __STRING(sym),1,0,0,0
#else
#define	WARN_REFERENCES(sym,msg)					\
	.stabs msg,30,0,0,0 ;						\
	.stabs __STRING(_/**/sym),1,0,0,0
#endif /* __STDC__ */



#endif /* !_I386_ASM_H_ */
