/* $NetBSD: mach_fasttraps_sysent.c,v 1.7 2003/01/18 23:38:37 thorpej Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.3 2002/12/07 19:05:12 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_fasttraps_sysent.c,v 1.7 2003/01/18 23:38:37 thorpej Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/arch/powerpc/fasttraps/mach_fasttraps_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent mach_fasttraps_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = unimplemented */
	{ 1, s(struct mach_sys_cthread_set_self_args), 0,
	    mach_sys_cthread_set_self },	/* 1 = cthread_set_self */
	{ 0, 0, 0,
	    mach_sys_cthread_self },		/* 2 = cthread_self */
	{ 0, 0, 0,
	    mach_sys_processor_facilities_used },/* 3 = processor_facilities_used */
	{ 0, 0, 0,
	    mach_sys_load_msr },		/* 4 = load_msr */
	{ 0, 0, 0,
	    sys_nosys },			/* 5 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 6 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 7 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 8 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 9 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 10 = unimplemented special_bluebox */
	{ 0, 0, 0,
	    sys_nosys },			/* 11 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 12 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 13 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 14 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 15 = unimplemented */
};

