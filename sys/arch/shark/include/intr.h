/*	$NetBSD: intr.h,v 1.5 2007/02/18 07:13:17 matt Exp $	*/

/*
 * Copyright (c) 1997 Mark Brinicombe.
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
 *	This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ARM32_INTR_H_
#define _ARM32_INTR_H_

/* Define the various Interrupt Priority Levels */

/* Hardware Interrupt Priority Levels are not mutually exclusive. */

#define IPL_NONE	0
#define	IPL_SOFTCLOCK	1
#define	IPL_SOFTNET	2
#define IPL_BIO		3	/* block I/O */
#define IPL_NET		4	/* network */
#define	IPL_SOFTSERIAL	5
#define IPL_TTY		6	/* terminal */
#define IPL_VM		7	/* memory allocation */
#define IPL_AUDIO	8	/* audio */
#define IPL_CLOCK	9	/* clock */
#define	IPL_STATCLOCK	10
#define IPL_SERIAL	11	/* serial */
#define IPL_HIGH	12	/*  */
#define	IPL_LOCK	IPL_HIGH
#define	IPL_SCHED	IPL_HIGH

#define IPL_LEVELS	13

#define	IST_UNUSABLE	-1	/* interrupt cannot be used */
#define	IST_NONE	0	/* none (dummy) */
#define	IST_PULSE	1	/* pulsed */
#define	IST_EDGE	2	/* edge-triggered */
#define	IST_LEVEL	3	/* level-triggered */

#include <machine/irqhandler.h>
#include <arm/arm32/psl.h>

#endif	/* _ARM32_INTR_H */
