/*	$NetBSD: cpuconf.h,v 1.7 1997/11/06 00:42:03 thorpej Exp $	*/
#ifndef	_ALPHA_CPUCONF_H
#define	_ALPHA_CPUCONF_H
/*
 * Copyright (c) 1996 Christopher G. Demetriou.  All rights reserved.
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
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
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
/*
 * Additional reworking by Matthew Jacob for NASA/Ames Research Center.
 * Copyright (c) 1997
 */
#ifdef _KERNEL
/*
 * Platform Specific Information and Function Hooks.
 *
 * The tags family and model information are strings describing the platform.
 * 
 * The tag iobus describes the primary iobus for the platform- primarily
 * to give a hint as to where to start configuring. The likely choices
 * are one of tcasic, lca, apecs, cia, or tlsb.
 *
 */

extern struct platform {
	/*
	 * Platform Information.
	 */
	const char	*family;	/* Family Name */
	const char	*model;		/* Model (variant) Name */
	const char	*iobus;		/* Primary iobus name */

	/*
	 * Platform Specific Function Hooks
	 *	cons_init 	-	console initialization
	 *	device_register	-	boot configuration aid
	 *	iointr		-	I/O interrupt handler
	 *	clockintr	-	Clock Interrupt Handler
	 *	mcheck_handler	-	Platform Specific Machine Check Handler
	 */
	void	(*cons_init) __P((void));
	void	(*device_register) __P((struct device *, void *));
	void	(*iointr) __P((void *, unsigned long));
	void	(*clockintr) __P((void *));
	void	(*mcheck_handler) __P((unsigned long, struct trapframe *,
		unsigned long, unsigned long));
} platform;

/*
 * There is an array of functions to initialize the platform structure.
 *
 * It's responsible for filling in the family, model_name and iobus
 * tags. It may optionally fill in the cons_init, device_register and
 * mcheck_handler tags.
 *
 * The iointr tag is filled in by set_iointr (in interrupt.c).
 * The clockintr tag is filled in by cpu_initclocks (in clock.c).
 *
 * nocpu is function to call when you can't figure what platform you're on.
 * There's no return from this function.
 */

struct cpuinit {
	void	(*init) __P((void));
	const char *option;
};

#define	cpu_notsupp(st)		{ platform_not_supported, st }
#define	cpu_init(fn, opt)	{ fn, opt }

extern struct cpuinit cpuinit[];
extern int ncpuinit;
extern void platform_not_configured __P((void));
extern void platform_not_supported __P((void));

#endif /* _KERNEL */
#endif /* !_ALPHA_CPUCONF_H */
