/* $NetBSD: mach_fasttraps_syscall.h,v 1.9 2005/02/26 23:20:01 perry Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.5 2005/02/26 23:10:20 perry Exp 
 */

/* syscall: "cthread_set_self" ret: "void" args: "mach_cproc_t" */
#define	MACH_FASTTRAPS_SYS_cthread_set_self	1

/* syscall: "cthread_self" ret: "mach_cproc_t" args: */
#define	MACH_FASTTRAPS_SYS_cthread_self	2

/* syscall: "processor_facilities_used" ret: "int" args: */
#define	MACH_FASTTRAPS_SYS_processor_facilities_used	3

/* syscall: "load_msr" ret: "void" args: */
#define	MACH_FASTTRAPS_SYS_load_msr	4

#define	MACH_FASTTRAPS_SYS_MAXSYSCALL	16
#define	MACH_FASTTRAPS_SYS_NSYSENT	16
