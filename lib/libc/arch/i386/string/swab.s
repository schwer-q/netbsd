/*
 * Copyright (c) 1993 Winning Strategies, Inc.
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
 *      This product includes software developed by Winning Strategies, Inc.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	$Id: swab.s,v 1.2 1993/10/21 01:40:44 jtc Exp $
 */

#if defined(LIBC_SCCS)
	.text
	.asciz "$Id: swab.s,v 1.2 1993/10/21 01:40:44 jtc Exp $"
#endif

#include "DEFS.h"

/*
 * void
 * swab (const void *src, void *dst, size_t len)
 *	copy len bytes from src to dst, swapping adjacent bytes
 *
 * On the i486, this code is negligibly faster than the code generated
 * by gcc at about half the size.  If my i386 databook is correct, it 
 * should be considerably faster than the gcc code on a i386.
 *
 * Written by:
 *	J.T. Conklin (jtc@wimsey.com), Winning Strategies, Inc.
 */

ENTRY(swab)
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx
	shrl	$1,%ecx
	jz	L4			# len = 0, jmp to func exit

	cld				# set direction forward

	testl	$7,%ecx			# copy first group of 1 to 7 words
	je	L2			# while swaping alternate bytes.
	.align	2,0x90
L1:	lodsw
	xchgb	%al,%ah
	stosw
	decl	%ecx
	testl	$7,%ecx
	jne	L1
	
L2:	shrl	$3,%ecx			# copy remainder 8 words at a time
	jz	L4			# while swapping alternate bytes.
	.align	2,0x90
L3:	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	lodsw
	xchgb	%al,%ah
	stosw
	decl	%ecx
	jnz	L3

L4:
	popl	%edi
	popl	%esi
	ret
