/*      $NetBSD: subr.s,v 1.5 1995/02/23 17:54:05 ragge Exp $     */

/*
 * Copyright (c) 1994 Ludd, University of Lule}, Sweden.
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
 *     This product includes software developed at Ludd, University of Lule}.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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
 */

 /* All bugs are subject to removal without further notice */
		


#include "vax/include/mtpr.h"
#include "vax/include/param.h"
#include "vax/include/loconf.h"
#include "vax/include/vmparam.h"
#include "vax/include/pte.h"
#include "vax/include/nexus.h"
#include "sys/syscall.h"
#include "sys/errno.h"


		.text

		.globl	_sigcode,_esigcode
_sigcode:	pushr	$0x3f
		subl2	$0xc,sp
		movl	0x24(sp),r0
		calls	$3,(r0)
		popr	$0x3f
		chmk	$SYS_sigreturn
		halt	
_esigcode:

		.globl	_subyte
_subyte:	.word 0x0
		movl	4(ap),r0
#		probew	$3,$1,(r0)
#		beql	suerr
		movb	8(ap),(r0)
		clrl	r0
		ret

suerr:		movl	$-1,r0
		ret

                .globl  _fubyte
_fubyte:        .word 0x0
                movl    4(ap),r0
#                prober  $3,$1,(r0)
#                beql    suerr
                movzbl	(r0),r0
                ret







		.globl _physcopypage
_physcopypage:	.word 0x7
		movl	4(ap),r0
		ashl	$-PGSHIFT,r0,r0
		bisl2	$(PG_V|PG_RO),r0

		movl	8(ap),r1
		ashl    $-PGSHIFT,r1,r1
		bisl2   $(PG_V|PG_KW),r1

		movl	r0,*(_pte_cmap)
		movl	r1,*$4+(_pte_cmap)

		movl	_v_cmap,r2
		addl3	$0x200,r2,r1
		mtpr	r1,$PR_TBIS
		mtpr	r2,$PR_TBIS

		movl	r1,r0
1:		movl	(r2)+,(r1)+
		cmpl	r0,r2
		bneq	1b
		ret


# _clearpage should be inline assembler

		.globl _clearpage
_clearpage:	.word 0x0
		movc5	$0, (sp), $0, $NBPG, *4(ap)
		ret


		.globl _badaddr
_badaddr:	.word	0x0
					# Called with addr,b/w/l
		mfpr	$0x12,r0
		mtpr	$0x1f,$0x12
		movl	4(ap),r2 	# First argument, the address
		movl	8(ap),r1 	# Sec arg, b,w,l
		pushl	r0		# Save old IPL
		clrl	r3
		movl	$4f,_memtest	# Set the return adress

		caseb	r1,$1,$4	# What is the size
1:		.word	1f-1b		
		.word	2f-1b
		.word	3f-1b		# This is unused
		.word	3f-1b
		
1:		movb	(r2),r1		# Test a byte
		brb	5f

2:		movw	(r2),r1		# Test a word
		brb	5f

3:		movl	(r2),r1		# Test a long
		brb	5f

4:		incl	r3		# Got machine chk => addr bad
5:		mtpr	(sp)+,$0x12
		movl	r3,r0
		ret

		.align 2
_mba_0:		.globl _mba_0
		movl	$0,mbanum
		brb	_mba
		.align 2
_mba_1:		.globl _mba_1
		movl	$1,mbanum
		brb	_mba
		.align 2
_mba_2:		.globl _mba_2
		movl	$2,mbanum
		brb	_mba
		.align 2
_mba_3:		.globl _mba_3
		movl	$3,mbanum
_mba:		pushr	$0xffff
		pushl	mbanum
		calls	$1,_mbainterrupt
		popr	$0xffff
		rei

#
# copyin(from, to, len) copies from userspace to kernelspace.
#

	.globl	_copyin
_copyin:.word	0x1c
	movl	4(ap),r0	# from
	movl	8(ap),r1	# to
	movl	12(ap),r2	# len

	movl	r0,r4
	movl	r2,r3

/* 3:	prober	$3,r3,(r4)	# Check access to all pages. */
#	beql	1f
#	cmpl	r3,$NBPG
#	bleq	2f
#	subl2	$NBPG,r3
#	addl2	$NBPG,r4
#	brb	3b

	tstl	r2
	beql	3f
2:      movb    (r0)+,(r1)+       # XXX Should be done in a faster way.
        decl    r2              
        bneq    2b
3:      clrl    r0
        ret

1:	movl	$EFAULT,r0	# Didnt work...
	ret

#
# copyout(from, to, len) in the same manner as copyin()
#

	.globl	_copyout
_copyout:.word   0x1c
        movl    4(ap),r0        # from
        movl    8(ap),r1        # to
        movl    12(ap),r2       # len

        movl    r1,r4
        movl    r2,r3

/*3:      probew  $3,r3,(r4)	# Check access to all pages. */
#        beql    1b
#        cmpl    r3,$NBPG
#        bleq    2f
#        subl2   $NBPG,r3
#        addl2   $NBPG,r4
#        brb     3b

	tstl	r2
	beql	3f
2:	movb	(r0)+,(r1)+	# XXX Should be done in a faster way.
	decl	r2
	bneq	2b
3:	clrl	r0
	ret

#
# copystr(from, to, maxlen, *copied)
# Only used in kernel mode, doesnt check accessability.
#

	.globl	_copystr
_copystr:	.word 0x7c
        movl    4(ap),r4        # from
        movl    8(ap),r5        # to
        movl    12(ap),r2       # len
	movl	16(ap),r3	# copied
#	halt

#if VAX630
        movl    r4, r1          # (3) string address == r1
        movl    r2, r0          # (2) string length == r0
        jeql    Llocc_out       # forget zero length strings
Llocc_loop:
        tstb    (r1)
        jeql    Llocc_out
        incl    r1
        sobgtr  r0,Llocc_loop
Llocc_out:
        tstl    r0              # be sure of condition codes
#else
        locc    $0, r2, (r4)    # check for null byte
#endif
	beql	1f

	subl3	r0, r2, r6	# Len to copy.
	incl	r6
	movl	r6,(r3)
#	pushl	r6
#	pushl	r5
#	pushl	r4
	movc3	r6,(r4),(r5)
#	calls	$3,_bcopy
	movl	$0,r0
	ret

1:
#	pushl	r2
#	pushl	r5
#	pushl	r4
#	calls   $3,_bcopy
	movc3	r2,(r4),(r5)
	movl	$ENAMETOOLONG, r0
	ret


_loswtch:	.globl	_loswtch
#		halt
		mtpr	_curpcb,$PR_PCBB
		svpctx
		mtpr	_nypcb,$PR_PCBB
		ldpctx
#		halt
		rei


		.globl _savectx
_savectx:
		clrl	r0
		svpctx
		ldpctx
		mtpr	_p0lr,$PR_P0LR
		mtpr	_p0br,$PR_P0BR
		mtpr	_p1lr,$PR_P1LR
		mtpr    _p1br,$PR_P1BR
		mfpr	$PR_ESP,r0	# PR_ESP == start chld pcb
		mtpr	$0,$PR_ESP	# Clear ESP, used in fault routine
		clrl	4(r0)		# Clear ESP in child
                movl    _ustat,(r0)     # New kernel sp in chld
                addl2   _uofset,68(r0)  # set fp to new stack
                movl    _ustat,r0
                movl    (sp),(r0)
                movl    4(sp),4(r0)
                rei


	.data

mbanum:		.long 0
	

/*** DATA ********************************************************************/



_pte_cmap:	.long 0 ; .globl _pte_cmap	/* Address of PTE 
						   corresponding to cmap    */

_memtest:	.long 0 ; .globl _memtest	# Memory test in progress.
