/*	$NetBSD: intr.h,v 1.1 2000/08/12 22:58:22 wdk Exp $	*/

/*
 * Copyright (c) 1998 Jonathan Stone.  All rights reserved.
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
 *	This product includes software developed by Jonathan Stone for
 *      the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#ifndef _MACHINE_INTR_H_
#define _MACHINE_INTR_H_

#define IPL_NONE	0	/* disable only this interrupt */
#define IPL_BIO		1	/* disable block I/O interrupts */
#define IPL_NET		2	/* disable network interrupts */
#define IPL_TTY		3	/* disable terminal interrupts */
#define IPL_CLOCK	4	/* disable clock interrupts */
#define IPL_STATCLOCK	5	/* disable profiling interrupts */
#define IPL_SERIAL	6	/* disable serial hardware interrupts */
#define IPL_HIGH	7	/* disable all interrupts */

#define	IPL_SOFTSERIAL	0	/* serial software interrupts */
#define	IPL_SOFTNET	1	/* network software interrupts */
#define	IPL_SOFTCLOCK	2	/* clock software interrupts */
#define	IPL_NSOFT	3

#define	IPL_SOFTNAMES {							\
	"serial",							\
	"net",								\
	"clock",							\
}

#ifdef _KERNEL
#ifndef _LOCORE
#include <mips/cpuregs.h>

extern int _splraise __P((int));
extern int _spllower __P((int));
extern int _splset __P((int));
extern int _splget __P((void));
extern void _splnone __P((void));
extern void _setsoftintr __P((int));
extern void _clrsoftintr __P((int));

/*
 * software simulated interrupt
 */
#define SIR_NET		0x01
#define SIR_SERIAL	0x02

#define setsoft(x)	do {			\
	extern u_int ssir;			\
	int s;					\
						\
	s = splhigh();				\
	ssir |= (x);				\
	_setsoftintr(MIPS_SOFT_INT_MASK_1);	\
	splx(s);				\
} while (0)

#define setsoftclock()	_setsoftintr(MIPS_SOFT_INT_MASK_0)
#define setsoftnet()	setsoft(SIR_NET)
#define setsoftserial()	setsoft(SIR_SERIAL)

/*
 * nesting interrupt masks.
 */
#define MIPS_INT_MASK_SPL_SOFT0	MIPS_SOFT_INT_MASK_0
#define MIPS_INT_MASK_SPL_SOFT1	(MIPS_SOFT_INT_MASK_1|MIPS_INT_MASK_SPL_SOFT0)
#define MIPS_INT_MASK_SPL0	(MIPS_INT_MASK_0|MIPS_INT_MASK_SPL_SOFT1)
#define MIPS_INT_MASK_SPL1	(MIPS_INT_MASK_1|MIPS_INT_MASK_SPL0)
#define MIPS_INT_MASK_SPL2	(MIPS_INT_MASK_2|MIPS_INT_MASK_SPL1)
#define MIPS_INT_MASK_SPL3	(MIPS_INT_MASK_3|MIPS_INT_MASK_SPL2)
#define MIPS_INT_MASK_SPL4	(MIPS_INT_MASK_4|MIPS_INT_MASK_SPL3)
#define MIPS_INT_MASK_SPL5	(MIPS_INT_MASK_5|MIPS_INT_MASK_SPL4)

#define spl0()		(void)_spllower(0)
#define splx(s)		(void)_splset(s)
#define splbio()	_splraise(MIPS_INT_MASK_SPL1)
#define splnet()	_splraise(MIPS_INT_MASK_SPL0)
#define spltty()	_splraise(MIPS_INT_MASK_SPL0)
#define splimp()	_splraise(MIPS_INT_MASK_SPL0)
#define splclock()	_splraise(MIPS_INT_MASK_SPL2)
#define splstatclock()	_splraise(MIPS_INT_MASK_SPL2)
#define splhigh()	_splraise(MIPS_INT_MASK_SPL2)

#define splsoftclock()	_splraise(MIPS_INT_MASK_SPL_SOFT0)
#define splsoftnet()	_splraise(MIPS_INT_MASK_SPL_SOFT1)
#define spllowersoftclock() _spllower(MIPS_INT_MASK_SPL_SOFT0)

/* handle i/o device interrupts */
extern void mipsco_intr __P((u_int, u_int, u_int, u_int));

extern void (*enable_intr) __P((void));
extern void (*disable_intr) __P((void));

#endif /* !_LOCORE */
#endif /* _KERNEL */
#endif /* _MACHINE_INTR_H_ */
