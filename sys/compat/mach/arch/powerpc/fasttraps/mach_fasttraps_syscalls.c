/* $NetBSD: mach_fasttraps_syscalls.c,v 1.7 2003/01/18 23:38:37 thorpej Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.3 2002/12/07 19:05:12 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_fasttraps_syscalls.c,v 1.7 2003/01/18 23:38:37 thorpej Exp $");

#if defined(_KERNEL_OPT)
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/arch/powerpc/fasttraps/mach_fasttraps_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const mach_fasttraps_syscallnames[] = {
	"#0 (unimplemented)",		/* 0 = unimplemented */
	"cthread_set_self",			/* 1 = cthread_set_self */
	"cthread_self",			/* 2 = cthread_self */
	"processor_facilities_used",			/* 3 = processor_facilities_used */
	"load_msr",			/* 4 = load_msr */
	"#5 (unimplemented)",		/* 5 = unimplemented */
	"#6 (unimplemented)",		/* 6 = unimplemented */
	"#7 (unimplemented)",		/* 7 = unimplemented */
	"#8 (unimplemented)",		/* 8 = unimplemented */
	"#9 (unimplemented)",		/* 9 = unimplemented */
	"#10 (unimplemented special_bluebox)",		/* 10 = unimplemented special_bluebox */
	"#11 (unimplemented)",		/* 11 = unimplemented */
	"#12 (unimplemented)",		/* 12 = unimplemented */
	"#13 (unimplemented)",		/* 13 = unimplemented */
	"#14 (unimplemented)",		/* 14 = unimplemented */
	"#15 (unimplemented)",		/* 15 = unimplemented */
};
