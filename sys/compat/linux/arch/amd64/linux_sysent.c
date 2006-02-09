/* $NetBSD: linux_sysent.c,v 1.10 2006/02/09 19:18:56 manu Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.9 2005/11/23 16:14:57 manu Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_sysent.c,v 1.10 2006/02/09 19:18:56 manu Exp $");

#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_sem.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent linux_sysent[] = {
	{ 3, s(struct sys_read_args), 0,
	    sys_read },				/* 0 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 1 = write */
	{ 3, s(struct linux_sys_open_args), 0,
	    linux_sys_open },			/* 2 = open */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 3 = close */
	{ 2, s(struct linux_sys_stat64_args), 0,
	    linux_sys_stat64 },			/* 4 = stat64 */
	{ 2, s(struct linux_sys_fstat64_args), 0,
	    linux_sys_fstat64 },		/* 5 = fstat64 */
	{ 2, s(struct linux_sys_lstat64_args), 0,
	    linux_sys_lstat64 },		/* 6 = lstat64 */
	{ 3, s(struct sys_poll_args), 0,
	    sys_poll },				/* 7 = poll */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 8 = lseek */
	{ 6, s(struct linux_sys_mmap_args), 0,
	    linux_sys_mmap },			/* 9 = mmap */
	{ 3, s(struct linux_sys_mprotect_args), 0,
	    linux_sys_mprotect },		/* 10 = mprotect */
	{ 2, s(struct sys_munmap_args), 0,
	    sys_munmap },			/* 11 = munmap */
	{ 1, s(struct linux_sys_brk_args), 0,
	    linux_sys_brk },			/* 12 = brk */
	{ 4, s(struct linux_sys_rt_sigaction_args), 0,
	    linux_sys_rt_sigaction },		/* 13 = rt_sigaction */
	{ 4, s(struct linux_sys_rt_sigprocmask_args), 0,
	    linux_sys_rt_sigprocmask },		/* 14 = rt_sigprocmask */
	{ 0, 0, 0,
	    linux_sys_rt_sigreturn },		/* 15 = rt_sigreturn */
	{ 3, s(struct linux_sys_ioctl_args), 0,
	    linux_sys_ioctl },			/* 16 = ioctl */
	{ 4, s(struct linux_sys_pread_args), 0,
	    linux_sys_pread },			/* 17 = pread */
	{ 4, s(struct linux_sys_pwrite_args), 0,
	    linux_sys_pwrite },			/* 18 = pwrite */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 19 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 20 = writev */
	{ 2, s(struct linux_sys_access_args), 0,
	    linux_sys_access },			/* 21 = access */
	{ 1, s(struct linux_sys_pipe_args), 0,
	    linux_sys_pipe },			/* 22 = pipe */
	{ 5, s(struct linux_sys_select_args), 0,
	    linux_sys_select },			/* 23 = select */
	{ 0, 0, 0,
	    linux_sys_sched_yield },		/* 24 = sched_yield */
	{ 4, s(struct linux_sys_mremap_args), 0,
	    linux_sys_mremap },			/* 25 = mremap */
	{ 3, s(struct linux_sys_msync_args), 0,
	    linux_sys_msync },			/* 26 = msync */
	{ 3, s(struct sys_mincore_args), 0,
	    sys_mincore },			/* 27 = mincore */
	{ 3, s(struct sys_madvise_args), 0,
	    sys_madvise },			/* 28 = madvise */
#ifdef SYSVSHM
	{ 3, s(struct linux_sys_shmget_args), 0,
	    linux_sys_shmget },			/* 29 = shmget */
	{ 4, s(struct linux_sys_shmat_args), 0,
	    linux_sys_shmat },			/* 30 = shmat */
	{ 3, s(struct linux_sys_shmctl_args), 0,
	    linux_sys_shmctl },			/* 31 = shmctl */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 29 = unimplemented shmget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 30 = unimplemented shmat */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 31 = unimplemented shmctl */
#endif
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 32 = dup */
	{ 2, s(struct sys_dup2_args), 0,
	    sys_dup2 },				/* 33 = dup2 */
	{ 0, 0, 0,
	    linux_sys_pause },			/* 34 = pause */
	{ 2, s(struct sys_nanosleep_args), 0,
	    sys_nanosleep },			/* 35 = nanosleep */
	{ 2, s(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 36 = getitimer */
	{ 1, s(struct linux_sys_alarm_args), 0,
	    linux_sys_alarm },			/* 37 = alarm */
	{ 3, s(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 38 = setitimer */
	{ 0, 0, SYCALL_MPSAFE | 0,
	    sys_getpid },			/* 39 = getpid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 40 = unimplemented sendfile */
	{ 3, s(struct linux_sys_socket_args), 0,
	    linux_sys_socket },			/* 41 = socket */
	{ 3, s(struct linux_sys_connect_args), 0,
	    linux_sys_connect },		/* 42 = connect */
	{ 3, s(struct linux_sys_accept_args), 0,
	    linux_sys_accept },			/* 43 = oaccept */
	{ 6, s(struct linux_sys_sendto_args), 0,
	    linux_sys_sendto },			/* 44 = sendto */
	{ 6, s(struct linux_sys_recvfrom_args), 0,
	    linux_sys_recvfrom },		/* 45 = recvfrom */
	{ 3, s(struct linux_sys_sendmsg_args), 0,
	    linux_sys_sendmsg },		/* 46 = sendmsg */
	{ 3, s(struct linux_sys_recvmsg_args), 0,
	    linux_sys_recvmsg },		/* 47 = recvmsg */
	{ 2, s(struct sys_shutdown_args), 0,
	    sys_shutdown },			/* 48 = shutdown */
	{ 3, s(struct linux_sys_bind_args), 0,
	    linux_sys_bind },			/* 49 = bind */
	{ 2, s(struct sys_listen_args), 0,
	    sys_listen },			/* 50 = listen */
	{ 3, s(struct linux_sys_getsockname_args), 0,
	    linux_sys_getsockname },		/* 51 = getsockname */
	{ 3, s(struct linux_sys_getpeername_args), 0,
	    linux_sys_getpeername },		/* 52 = getpeername */
	{ 4, s(struct linux_sys_socketpair_args), 0,
	    linux_sys_socketpair },		/* 53 = socketpair */
	{ 5, s(struct linux_sys_setsockopt_args), 0,
	    linux_sys_setsockopt },		/* 54 = setsockopt */
	{ 5, s(struct linux_sys_getsockopt_args), 0,
	    linux_sys_getsockopt },		/* 55 = getsockopt */
	{ 4, s(struct linux_sys_clone_args), 0,
	    linux_sys_clone },			/* 56 = clone */
	{ 0, 0, 0,
	    sys_fork },				/* 57 = fork */
	{ 0, 0, 0,
	    sys___vfork14 },			/* 58 = __vfork14 */
	{ 3, s(struct linux_sys_execve_args), 0,
	    linux_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 60 = exit */
	{ 4, s(struct linux_sys_wait4_args), 0,
	    linux_sys_wait4 },			/* 61 = wait4 */
	{ 2, s(struct linux_sys_kill_args), 0,
	    linux_sys_kill },			/* 62 = kill */
	{ 1, s(struct linux_sys_uname_args), 0,
	    linux_sys_uname },			/* 63 = uname */
#ifdef SYSVSEM
	{ 3, s(struct sys_semget_args), 0,
	    sys_semget },			/* 64 = semget */
	{ 3, s(struct sys_semop_args), 0,
	    sys_semop },			/* 65 = semop */
	{ 4, s(struct linux_sys_semctl_args), 0,
	    linux_sys_semctl },			/* 66 = semctl */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 64 = unimplemented semget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 65 = unimplemented semop */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 66 = unimplemented semctl */
#endif
#ifdef SYSVSHM
	{ 1, s(struct sys_shmdt_args), 0,
	    sys_shmdt },			/* 67 = shmdt */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 67 = unimplemented shmdt */
#endif
#ifdef SYSVMSG
	{ 2, s(struct sys_msgget_args), 0,
	    sys_msgget },			/* 68 = msgget */
	{ 4, s(struct sys_msgsnd_args), 0,
	    sys_msgsnd },			/* 69 = msgsnd */
	{ 5, s(struct sys_msgrcv_args), 0,
	    sys_msgrcv },			/* 70 = msgrcv */
	{ 3, s(struct linux_sys_msgctl_args), 0,
	    linux_sys_msgctl },			/* 71 = msgctl */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 68 = unimplemented msgget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 69 = unimplemented msgsnd */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 70 = unimplemented msgrcv */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 71 = unimplemented msgctl */
#endif
	{ 3, s(struct linux_sys_fcntl_args), 0,
	    linux_sys_fcntl },			/* 72 = fcntl */
	{ 2, s(struct sys_flock_args), 0,
	    sys_flock },			/* 73 = flock */
	{ 1, s(struct sys_fsync_args), 0,
	    sys_fsync },			/* 74 = fsync */
	{ 1, s(struct linux_sys_fdatasync_args), 0,
	    linux_sys_fdatasync },		/* 75 = fdatasync */
	{ 2, s(struct linux_sys_truncate64_args), 0,
	    linux_sys_truncate64 },		/* 76 = truncate64 */
	{ 2, s(struct linux_sys_ftruncate64_args), 0,
	    linux_sys_ftruncate64 },		/* 77 = ftruncate64 */
	{ 3, s(struct linux_sys_getdents_args), 0,
	    linux_sys_getdents },		/* 78 = getdents */
	{ 2, s(struct sys___getcwd_args), 0,
	    sys___getcwd },			/* 79 = __getcwd */
	{ 1, s(struct linux_sys_chdir_args), 0,
	    linux_sys_chdir },			/* 80 = chdir */
	{ 1, s(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 81 = fchdir */
	{ 2, s(struct linux_sys_rename_args), 0,
	    linux_sys_rename },			/* 82 = rename */
	{ 2, s(struct linux_sys_mkdir_args), 0,
	    linux_sys_mkdir },			/* 83 = mkdir */
	{ 1, s(struct linux_sys_rmdir_args), 0,
	    linux_sys_rmdir },			/* 84 = rmdir */
	{ 2, s(struct linux_sys_creat_args), 0,
	    linux_sys_creat },			/* 85 = creat */
	{ 2, s(struct linux_sys_link_args), 0,
	    linux_sys_link },			/* 86 = link */
	{ 1, s(struct linux_sys_unlink_args), 0,
	    linux_sys_unlink },			/* 87 = unlink */
	{ 2, s(struct linux_sys_symlink_args), 0,
	    linux_sys_symlink },		/* 88 = symlink */
	{ 3, s(struct linux_sys_readlink_args), 0,
	    linux_sys_readlink },		/* 89 = readlink */
	{ 2, s(struct linux_sys_chmod_args), 0,
	    linux_sys_chmod },			/* 90 = chmod */
	{ 2, s(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 91 = fchmod */
	{ 3, s(struct linux_sys_chown_args), 0,
	    linux_sys_chown },			/* 92 = chown */
	{ 3, s(struct sys___posix_fchown_args), 0,
	    sys___posix_fchown },		/* 93 = __posix_fchown */
	{ 3, s(struct linux_sys_lchown_args), 0,
	    linux_sys_lchown },			/* 94 = lchown */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 95 = umask */
	{ 2, s(struct linux_sys_gettimeofday_args), 0,
	    linux_sys_gettimeofday },		/* 96 = gettimeofday */
	{ 2, s(struct linux_sys_getrlimit_args), 0,
	    linux_sys_getrlimit },		/* 97 = getrlimit */
	{ 2, s(struct sys_getrusage_args), 0,
	    sys_getrusage },			/* 98 = getrusage */
	{ 1, s(struct linux_sys_sysinfo_args), 0,
	    linux_sys_sysinfo },		/* 99 = sysinfo */
	{ 1, s(struct linux_sys_times_args), 0,
	    linux_sys_times },			/* 100 = times */
	{ 4, s(struct linux_sys_ptrace_args), 0,
	    linux_sys_ptrace },			/* 101 = ptrace */
	{ 0, 0, 0,
	    sys_getuid },			/* 102 = getuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 103 = unimplemented syslog */
	{ 0, 0, 0,
	    sys_getgid },			/* 104 = getgid */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 105 = setuid */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 106 = setgid */
	{ 0, 0, 0,
	    sys_geteuid },			/* 107 = geteuid */
	{ 0, 0, 0,
	    sys_getegid },			/* 108 = getegid */
	{ 2, s(struct sys_setpgid_args), 0,
	    sys_setpgid },			/* 109 = setpgid */
	{ 0, 0, 0,
	    sys_getppid },			/* 110 = getppid */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 111 = getpgrp */
	{ 0, 0, 0,
	    sys_setsid },			/* 112 = setsid */
	{ 2, s(struct sys_setreuid_args), 0,
	    sys_setreuid },			/* 113 = setreuid */
	{ 2, s(struct sys_setregid_args), 0,
	    sys_setregid },			/* 114 = setregid */
	{ 2, s(struct sys_getgroups_args), 0,
	    sys_getgroups },			/* 115 = getgroups */
	{ 2, s(struct sys_setgroups_args), 0,
	    sys_setgroups },			/* 116 = setgroups */
	{ 3, s(struct linux_sys_setresuid_args), 0,
	    linux_sys_setresuid },		/* 117 = setresuid */
	{ 3, s(struct linux_sys_getresuid_args), 0,
	    linux_sys_getresuid },		/* 118 = getresuid */
	{ 3, s(struct linux_sys_setresgid_args), 0,
	    linux_sys_setresgid },		/* 119 = setresgid */
	{ 3, s(struct linux_sys_getresgid_args), 0,
	    linux_sys_getresgid },		/* 120 = getresgid */
	{ 1, s(struct linux_sys_getpgid_args), 0,
	    linux_sys_getpgid },		/* 121 = getpgid */
	{ 1, s(struct linux_sys_setfsuid_args), 0,
	    linux_sys_setfsuid },		/* 122 = setfsuid */
	{ 0, 0, 0,
	    linux_sys_getfsuid },		/* 123 = getfsuid */
	{ 1, s(struct sys_getsid_args), 0,
	    sys_getsid },			/* 124 = getsid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 125 = unimplemented capget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 126 = unimplemented capset */
	{ 2, s(struct linux_sys_rt_sigpending_args), 0,
	    linux_sys_rt_sigpending },		/* 127 = rt_sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 128 = unimplemented rt_sigtimedwait */
	{ 3, s(struct linux_sys_rt_queueinfo_args), 0,
	    linux_sys_rt_queueinfo },		/* 129 = rt_queueinfo */
	{ 2, s(struct linux_sys_rt_sigsuspend_args), 0,
	    linux_sys_rt_sigsuspend },		/* 130 = rt_sigsuspend */
	{ 2, s(struct linux_sys_sigaltstack_args), 0,
	    linux_sys_sigaltstack },		/* 131 = sigaltstack */
	{ 2, s(struct linux_sys_utime_args), 0,
	    linux_sys_utime },			/* 132 = utime */
	{ 3, s(struct linux_sys_mknod_args), 0,
	    linux_sys_mknod },			/* 133 = mknod */
#ifdef EXEC_AOUT
	{ 1, s(struct linux_sys_uselib_args), 0,
	    linux_sys_uselib },			/* 134 = uselib */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 134 = unimplemented sys_uselib */
#endif
	{ 1, s(struct linux_sys_personality_args), 0,
	    linux_sys_personality },		/* 135 = personality */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 136 = unimplemented ustat */
	{ 3, s(struct linux_sys_statfs64_args), 0,
	    linux_sys_statfs64 },		/* 137 = statfs64 */
	{ 3, s(struct linux_sys_fstatfs64_args), 0,
	    linux_sys_fstatfs64 },		/* 138 = fstatfs64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 139 = unimplemented sysfs */
	{ 2, s(struct sys_getpriority_args), 0,
	    sys_getpriority },			/* 140 = getpriority */
	{ 3, s(struct sys_setpriority_args), 0,
	    sys_setpriority },			/* 141 = setpriority */
	{ 2, s(struct linux_sys_sched_setparam_args), 0,
	    linux_sys_sched_setparam },		/* 142 = sched_setparam */
	{ 2, s(struct linux_sys_sched_getparam_args), 0,
	    linux_sys_sched_getparam },		/* 143 = sched_getparam */
	{ 3, s(struct linux_sys_sched_setscheduler_args), 0,
	    linux_sys_sched_setscheduler },	/* 144 = sched_setscheduler */
	{ 1, s(struct linux_sys_sched_getscheduler_args), 0,
	    linux_sys_sched_getscheduler },	/* 145 = sched_getscheduler */
	{ 1, s(struct linux_sys_sched_get_priority_max_args), 0,
	    linux_sys_sched_get_priority_max },	/* 146 = sched_get_priority_max */
	{ 1, s(struct linux_sys_sched_get_priority_min_args), 0,
	    linux_sys_sched_get_priority_min },	/* 147 = sched_get_priority_min */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 148 = unimplemented sys_sched_rr_get_interval */
	{ 2, s(struct sys_mlock_args), 0,
	    sys_mlock },			/* 149 = mlock */
	{ 2, s(struct sys_munlock_args), 0,
	    sys_munlock },			/* 150 = munlock */
	{ 1, s(struct sys_mlockall_args), 0,
	    sys_mlockall },			/* 151 = mlockall */
	{ 0, 0, 0,
	    sys_munlockall },			/* 152 = munlockall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 153 = unimplemented vhangup */
	{ 3, s(struct linux_sys_modify_ldt_args), 0,
	    linux_sys_modify_ldt },		/* 154 = modify_ldt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 155 = unimplemented pivot_root */
	{ 1, s(struct linux_sys___sysctl_args), 0,
	    linux_sys___sysctl },		/* 156 = __sysctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 157 = unimplemented prctl */
	{ 2, s(struct linux_sys_arch_prctl_args), 0,
	    linux_sys_arch_prctl },		/* 158 = arch_prctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 159 = unimplemented adjtimex */
	{ 2, s(struct linux_sys_setrlimit_args), 0,
	    linux_sys_setrlimit },		/* 160 = setrlimit */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 161 = chroot */
	{ 0, 0, 0,
	    sys_sync },				/* 162 = sync */
	{ 1, s(struct sys_acct_args), 0,
	    sys_acct },				/* 163 = acct */
	{ 2, s(struct linux_sys_settimeofday_args), 0,
	    linux_sys_settimeofday },		/* 164 = settimeofday */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 165 = unimplemented mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 166 = unimplemented umount2 */
	{ 1, s(struct linux_sys_swapon_args), 0,
	    linux_sys_swapon },			/* 167 = swapon */
	{ 1, s(struct linux_sys_swapoff_args), 0,
	    linux_sys_swapoff },		/* 168 = swapoff */
	{ 4, s(struct linux_sys_reboot_args), 0,
	    linux_sys_reboot },			/* 169 = reboot */
	{ 2, s(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 170 = sethostname */
	{ 2, s(struct linux_sys_setdomainname_args), 0,
	    linux_sys_setdomainname },		/* 171 = setdomainname */
	{ 1, s(struct linux_sys_iopl_args), 0,
	    linux_sys_iopl },			/* 172 = iopl */
	{ 3, s(struct linux_sys_ioperm_args), 0,
	    linux_sys_ioperm },			/* 173 = ioperm */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 174 = unimplemented create_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 175 = unimplemented init_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 176 = unimplemented delete_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 177 = unimplemented get_kernel_syms */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 178 = unimplemented query_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 179 = unimplemented quotactl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 180 = unimplemented nfsservctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 181 = unimplemented getpmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 182 = unimplemented putpmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 183 = unimplemented afs_syscall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 184 = unimplemented tuxcall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 185 = unimplemented security */
	{ 0, 0, 0,
	    linux_sys_gettid },			/* 186 = gettid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 187 = unimplemented readahead */
	{ 5, s(struct linux_sys_setxattr_args), 0,
	    linux_sys_setxattr },		/* 188 = setxattr */
	{ 5, s(struct linux_sys_lsetxattr_args), 0,
	    linux_sys_lsetxattr },		/* 189 = lsetxattr */
	{ 5, s(struct linux_sys_fsetxattr_args), 0,
	    linux_sys_fsetxattr },		/* 190 = fsetxattr */
	{ 4, s(struct linux_sys_getxattr_args), 0,
	    linux_sys_getxattr },		/* 191 = getxattr */
	{ 4, s(struct linux_sys_lgetxattr_args), 0,
	    linux_sys_lgetxattr },		/* 192 = lgetxattr */
	{ 4, s(struct linux_sys_fgetxattr_args), 0,
	    linux_sys_fgetxattr },		/* 193 = fgetxattr */
	{ 3, s(struct linux_sys_listxattr_args), 0,
	    linux_sys_listxattr },		/* 194 = listxattr */
	{ 3, s(struct linux_sys_llistxattr_args), 0,
	    linux_sys_llistxattr },		/* 195 = llistxattr */
	{ 3, s(struct linux_sys_flistxattr_args), 0,
	    linux_sys_flistxattr },		/* 196 = flistxattr */
	{ 2, s(struct linux_sys_removexattr_args), 0,
	    linux_sys_removexattr },		/* 197 = removexattr */
	{ 2, s(struct linux_sys_lremovexattr_args), 0,
	    linux_sys_lremovexattr },		/* 198 = lremovexattr */
	{ 2, s(struct linux_sys_fremovexattr_args), 0,
	    linux_sys_fremovexattr },		/* 199 = fremovexattr */
	{ 2, s(struct linux_sys_tkill_args), 0,
	    linux_sys_tkill },			/* 200 = tkill */
	{ 1, s(struct linux_sys_time_args), 0,
	    linux_sys_time },			/* 201 = time */
	{ 6, s(struct linux_sys_futex_args), 0,
	    linux_sys_futex },			/* 202 = futex */
	{ 3, s(struct linux_sys_sched_setaffinity_args), 0,
	    linux_sys_sched_setaffinity },	/* 203 = sched_setaffinity */
	{ 3, s(struct linux_sys_sched_getaffinity_args), 0,
	    linux_sys_sched_getaffinity },	/* 204 = sched_getaffinity */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 205 = unimplemented set_thread_area */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 206 = unimplemented io_setup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 207 = unimplemented io_destroy */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 208 = unimplemented io_getevents */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 209 = unimplemented io_submit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 210 = unimplemented io_cancel */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 211 = unimplemented get_thread_area */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 212 = unimplemented lookup_dcookie */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 213 = unimplemented epoll_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 214 = unimplemented epoll_ctl_old */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 215 = unimplemented epoll_wait_old */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 216 = unimplemented remap_file_pages */
	{ 3, s(struct linux_sys_getdents64_args), 0,
	    linux_sys_getdents64 },		/* 217 = getdents64 */
	{ 1, s(struct linux_sys_set_tid_address_args), 0,
	    linux_sys_set_tid_address },	/* 218 = set_tid_address */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 219 = unimplemented restart_syscall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 220 = unimplemented semtimedop */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 221 = unimplemented fadvise64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 222 = unimplemented timer_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 223 = unimplemented timer_settime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 224 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 225 = unimplemented timer_getoverrun */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 226 = unimplemented timer_delete */
	{ 2, s(struct linux_sys_clock_settime_args), 0,
	    linux_sys_clock_settime },		/* 227 = clock_settime */
	{ 2, s(struct linux_sys_clock_gettime_args), 0,
	    linux_sys_clock_gettime },		/* 228 = clock_gettime */
	{ 2, s(struct linux_sys_clock_getres_args), 0,
	    linux_sys_clock_getres },		/* 229 = clock_getres */
	{ 4, s(struct linux_sys_clock_nanosleep_args), 0,
	    linux_sys_clock_nanosleep },	/* 230 = clock_nanosleep */
	{ 1, s(struct linux_sys_exit_group_args), 0,
	    linux_sys_exit_group },		/* 231 = exit_group */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 232 = unimplemented epoll_wait */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 233 = unimplemented epoll_ctl */
	{ 3, s(struct linux_sys_tgkill_args), 0,
	    linux_sys_tgkill },			/* 234 = tgkill */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 235 = unimplemented utimes */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 236 = unimplemented vserver */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 237 = nosys */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 238 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 239 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 240 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 241 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 242 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 243 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 244 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 245 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 246 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 247 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 248 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 249 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 250 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 251 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 252 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 253 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 254 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 255 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 256 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 257 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 258 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 259 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 260 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 261 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 262 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 263 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 264 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 265 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 266 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 267 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 268 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 269 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 270 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 271 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 272 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 273 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 274 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 275 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 276 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 277 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 278 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 279 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 280 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 281 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 282 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 283 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 284 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 285 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 286 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 287 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 288 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 289 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 290 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 291 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 292 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 293 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 294 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 295 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 296 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 297 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 298 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 299 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 300 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 301 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 302 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 303 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 304 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 305 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 306 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 307 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 308 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 309 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 310 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 311 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 312 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 313 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 314 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 315 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 316 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 317 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 318 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 319 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 320 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 321 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 322 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 323 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 324 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 325 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 326 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 327 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 328 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 329 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 330 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 331 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 332 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 333 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 334 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 335 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 336 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 337 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 338 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 339 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 340 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 341 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 342 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 343 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 344 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 345 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 346 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 347 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 348 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 349 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 350 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 351 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 352 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 353 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 354 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 355 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 356 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 357 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 358 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 359 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 360 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 361 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 362 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 363 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 364 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 365 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 366 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 367 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 368 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 369 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 370 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 371 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 372 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 373 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 374 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 375 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 376 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 377 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 378 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 379 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 380 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 381 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 382 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 383 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 384 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 385 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 386 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 387 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 388 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 389 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 390 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 391 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 392 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 393 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 394 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 395 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 396 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 397 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 398 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 399 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 400 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 401 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 402 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 403 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 404 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 405 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 406 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 407 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 408 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 409 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 410 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 411 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 412 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 413 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 414 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 415 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 416 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 417 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 418 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 419 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 420 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 421 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 422 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 423 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 424 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 425 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 426 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 427 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 428 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 429 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 430 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 431 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 432 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 433 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 434 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 435 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 436 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 437 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 438 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 439 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 440 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 441 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 442 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 443 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 444 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 445 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 446 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 447 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 448 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 449 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 450 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 451 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 452 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 453 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 454 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 455 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 456 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 457 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 458 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 459 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 460 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 461 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 462 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 463 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 464 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 465 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 466 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 467 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 468 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 469 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 470 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 471 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 472 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 473 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 474 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 475 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 476 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 477 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 478 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 479 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 480 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 481 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 482 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 483 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 484 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 485 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 486 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 487 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 488 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 489 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 490 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 491 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 492 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 493 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 494 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 495 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 496 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 497 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 498 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 499 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 500 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 501 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 502 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 503 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 504 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 505 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 506 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 507 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 508 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 509 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 510 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 511 = filler */
};

