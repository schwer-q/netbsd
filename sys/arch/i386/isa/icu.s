/*-
 * Copyright (c) 1993, 1994 Charles Hannum.
 * Copyright (c) 1989, 1990 William F. Jolitz.
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
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *	must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
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
 *	from: @(#)icu.s	7.2 (Berkeley) 5/21/91
 *	$Id: icu.s,v 1.23 1994/02/22 23:36:09 mycroft Exp $
 */

/*
 * AT/386
 * Vector interrupt control section
 */

#include <net/netisr.h>

/*
 * All spl levels include astmask; this forces cpl to be non-zero when
 * splx()ing from nested spl levels, and thus soft interrupts do not
 * get executed.  Logically, all spl levels are `above' soft interupts.
 */

#ifdef notyet
#include "sio.h"
#else
#define NSIO 0
#endif

	.data
	.globl	_cpl
_cpl:
	.long	-1		# current priority (all off)
	.globl	_highmask
_highmask:
	.long	-1		# XXX needed for braindead config
	.globl  _ipending
_ipending:
	.long   0
	.globl	_imen
_imen:
	.long	0xffff		# interrupt mask enable (all off)
	.globl	_clockmask
_clockmask:
	.long	1
	.globl	_ttymask
_ttymask:
	.long	0
	.globl	_biomask
_biomask:
	.long	0
	.globl	_netmask
_netmask:
	.long	0
	.globl	_impmask
_impmask:
	.long	0
	.globl	_astmask
_astmask:
	.long	0x80000000

vec:
	.long	INTRLOCAL(vec0), INTRLOCAL(vec1), INTRLOCAL(vec2)
	.long	INTRLOCAL(vec3), INTRLOCAL(vec4), INTRLOCAL(vec5)
	.long	INTRLOCAL(vec6), INTRLOCAL(vec7), INTRLOCAL(vec8)
	.long	INTRLOCAL(vec9), INTRLOCAL(vec10), INTRLOCAL(vec11)
	.long	INTRLOCAL(vec12), INTRLOCAL(vec13), INTRLOCAL(vec14)
	.long	INTRLOCAL(vec15)

#define	GENSPL(name, mask, event) \
	.globl  _spl/**/name ; \
	ALIGN_TEXT ; \
_spl/**/name: ; \
	movl	_cpl,%eax ; \
	movl	%eax,%edx ; \
	orl	mask,%edx ; \
	movl	%edx,_cpl ; \
	ret

#define	FASTSPL(mask) \
	movl	mask,_cpl

#define	FASTSPL_VARMASK(varmask) \
	movl	varmask,%eax ; \
	movl	%eax,_cpl

	.text

	ALIGN_TEXT
INTRLOCAL(unpend_v):
	bsfl    %eax,%eax               # slow, but not worth optimizing
	btrl    %eax,_ipending
	jnc     INTRLOCAL(unpend_v_next) # some intr cleared the in-memory bit
	movl    Vresume(,%eax,4),%eax
	testl   %eax,%eax
	jz      INTRLOCAL(noresume)
	jmp     %eax
  
	ALIGN_TEXT
/*
 * XXX - must be some fastintr, need to register those too.
 */
INTRLOCAL(noresume):
#if NSIO > 0
	call    _softsio1
#endif
INTRLOCAL(unpend_v_next):
	movl	_cpl,%eax
	movl	%eax,%edx
	notl	%eax
	andl	_ipending,%eax
	jz	INTRLOCAL(none_to_unpend)
	jmp	INTRLOCAL(unpend_v)

/*
 * Handle return from interrupt after device handler finishes
 */
	ALIGN_TEXT
doreti:
	addl	$4,%esp	# discard unit arg
	popl	%eax	# get previous priority
/*
 * Now interrupt frame is a trap frame!
 *
 * XXX - setting up the interrupt frame to be almost a stack frame is mostly
 * a waste of time.
 */
	movl	%eax,_cpl
	movl	%eax,%edx
	notl	%eax
	andl	_ipending,%eax
	jnz	INTRLOCAL(unpend_v)
INTRLOCAL(none_to_unpend):
	testl   %edx,%edx	# returning to zero priority?
	jz	2f
	popl	%es
	popl	%ds
	popal
	addl	$8,%esp
	iret


	.globl	_netisr,_sir

#define DONET(s, c, event) ; \
	.globl  c ; \
	btrl	$s,_netisr ; \
	jnc	1f ; \
	call	c ; \
1:

	ALIGN_TEXT
2:
/*
 * XXX - might need extra locking while testing reg copy of netisr, but
 * interrupt routines setting it would not cause any new problems (since we
 * don't loop, fresh bits will not be processed until the next doreti or spl0)
 */
	btrl	$SIR_NET,_sir
	jnc	test_clock
	FASTSPL_VARMASK(_netmask)
	DONET(NETISR_RAW, _rawintr, 5)
#ifdef INET
	DONET(NETISR_IP, _ipintr, 6)
#endif
#ifdef IMP
	DONET(NETISR_IMP, _impintr, 7)
#endif
#ifdef NS
	DONET(NETISR_NS, _nsintr, 8)
#endif
#ifdef ISO
	DONET(NETISR_ISO, _clnlintr, 25)
#endif
	FASTSPL($0)
test_clock:
	btrl	$SIR_CLOCK,_sir
	jnc	test_ast
	FASTSPL_VARMASK(_astmask)
	pushl   $0		# XXX previous cpl (probably not used)
	pushl   $0x7f		# XXX dummy unit number
	pushl	%esp
	call	_softclock
	addl	$12,%esp	# XXX discard dummies
	FASTSPL($0)
test_ast:
	btrl	$SIR_AST,_sir		# signal handling, rescheduling, ...
	jnc	2f
	testb   $SEL_RPL_MASK,TF_CS(%esp)
					# to non-kernel (i.e., user)?
	jz	2f			# nope, leave
	call	_trap
2:
	popl	%es
	popl	%ds
	popal
	addl	$8,%esp
	iret

/*
 * Interrupt priority mechanism
 *	-- soft splXX masks with group mechanism (cpl)
 *	-- h/w masks for currently active or unused interrupts (imen)
 *	-- ipending = active interrupts currently masked by cpl
 */

	GENSPL(tty, _ttymask, 18)
	GENSPL(bio, _biomask, 12)
	GENSPL(net, _netmask, 16)
	GENSPL(imp, _impmask, 15)
	GENSPL(high, $-1, 14)
	GENSPL(clock, _clockmask, 13)
	GENSPL(softclock, _astmask, 17)

	.globl _spl0
	ALIGN_TEXT
_spl0:
in_spl0:
	movl	_cpl,%eax
	pushl   %eax	# save old priority
	btrl	$SIR_NET,_sir
	jnc	INTRLOCAL(over_net_stuff_for_spl0)
	FASTSPL_VARMASK(_netmask)
	DONET(NETISR_RAW, _rawintr, 20)
#ifdef INET
	DONET(NETISR_IP, _ipintr, 21)
#endif
#ifdef IMP
	DONET(NETISR_IMP, _impintr, 26)
#endif
#ifdef NS
	DONET(NETISR_NS, _nsintr, 27)
#endif
#ifdef ISO
	DONET(NETISR_ISO, _clnlintr, 28)
#endif
INTRLOCAL(over_net_stuff_for_spl0):
	FASTSPL($0)
	movl	_ipending,%eax
	testl   %eax,%eax
	jnz	INTRLOCAL(unpend_V)
	popl	%eax		# return old priority
	ret
	
	.globl _splx
	ALIGN_TEXT
_splx:
	movl	4(%esp),%eax	# new priority
	testl   %eax,%eax
	jz	in_spl0		# going to "zero level" is special
	pushl	_cpl		# save old priority
	movl	%eax,_cpl	# set new priority
	notl	%eax
	andl	_ipending,%eax
	jnz	INTRLOCAL(unpend_V)
	popl	%eax		# return old priority
	ret

	ALIGN_TEXT
INTRLOCAL(unpend_V):
	bsfl    %eax,%eax
	btrl    %eax,_ipending
	jnc     INTRLOCAL(unpend_V_next)
	movl    Vresume(,%eax,4),%edx
	testl   %edx,%edx
	jz      INTRLOCAL(noresumeV)
/*
 * We would prefer to call the intr handler directly here but that doesn't
 * work for badly behaved handlers that want the interrupt frame.  Also,
 * there's a problem determining the unit number.  We should change the
 * interface so that the unit number is not determined at config time.
 */
	jmp     *vec(,%eax,4)

	ALIGN_TEXT
/*
 * XXX - must be some fastintr, need to register those too.
 */
INTRLOCAL(noresumeV):
#if NSIO > 0
	call    _softsio1
#endif
INTRLOCAL(unpend_V_next):
	movl	_cpl,%eax
	notl	%eax
	andl	_ipending,%eax
	jnz	INTRLOCAL(unpend_V)
	popl	%eax
	ret

#define BUILD_VEC(irq_num) \
	ALIGN_TEXT ; \
INTRLOCAL(vec/**/irq_num): ; \
	int     $(ICU_OFFSET + irq_num) ; \
	popl	%eax ; \
	ret

	BUILD_VEC(0)
	BUILD_VEC(1)
	BUILD_VEC(2)
	BUILD_VEC(3)
	BUILD_VEC(4)
	BUILD_VEC(5)
	BUILD_VEC(6)
	BUILD_VEC(7)
	BUILD_VEC(8)
	BUILD_VEC(9)
	BUILD_VEC(10)
	BUILD_VEC(11)
	BUILD_VEC(12)
	BUILD_VEC(13)
	BUILD_VEC(14)
	BUILD_VEC(15)

