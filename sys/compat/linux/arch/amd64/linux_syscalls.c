/* $NetBSD: linux_syscalls.c,v 1.6 2005/10/18 19:08:51 joerg Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.6 2005/10/18 18:37:44 joerg Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.6 2005/10/18 19:08:51 joerg Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
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
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	"read",			/* 0 = read */
	"write",			/* 1 = write */
	"open",			/* 2 = open */
	"close",			/* 3 = close */
	"stat64",			/* 4 = stat64 */
	"fstat64",			/* 5 = fstat64 */
	"lstat64",			/* 6 = lstat64 */
	"poll",			/* 7 = poll */
	"lseek",			/* 8 = lseek */
	"mmap",			/* 9 = mmap */
	"mprotect",			/* 10 = mprotect */
	"munmap",			/* 11 = munmap */
	"brk",			/* 12 = brk */
	"rt_sigaction",			/* 13 = rt_sigaction */
	"rt_sigprocmask",			/* 14 = rt_sigprocmask */
	"rt_sigreturn",			/* 15 = rt_sigreturn */
	"ioctl",			/* 16 = ioctl */
	"pread",			/* 17 = pread */
	"pwrite",			/* 18 = pwrite */
	"readv",			/* 19 = readv */
	"writev",			/* 20 = writev */
	"access",			/* 21 = access */
	"pipe",			/* 22 = pipe */
	"select",			/* 23 = select */
	"sched_yield",			/* 24 = sched_yield */
	"mremap",			/* 25 = mremap */
	"msync",			/* 26 = msync */
	"mincore",			/* 27 = mincore */
	"madvise",			/* 28 = madvise */
	"#29 (unimplemented shmget)",		/* 29 = unimplemented shmget */
	"#30 (unimplemented shmat)",		/* 30 = unimplemented shmat */
	"#31 (unimplemented shmctl)",		/* 31 = unimplemented shmctl */
	"dup",			/* 32 = dup */
	"dup2",			/* 33 = dup2 */
	"pause",			/* 34 = pause */
	"nanosleep",			/* 35 = nanosleep */
	"getitimer",			/* 36 = getitimer */
	"alarm",			/* 37 = alarm */
	"setitimer",			/* 38 = setitimer */
	"getpid",			/* 39 = getpid */
	"#40 (unimplemented sendfile)",		/* 40 = unimplemented sendfile */
	"socket",			/* 41 = socket */
	"connect",			/* 42 = connect */
	"oaccept",			/* 43 = oaccept */
	"sendto",			/* 44 = sendto */
	"recvfrom",			/* 45 = recvfrom */
	"sendmsg",			/* 46 = sendmsg */
	"recvmsg",			/* 47 = recvmsg */
	"shutdown",			/* 48 = shutdown */
	"bind",			/* 49 = bind */
	"listen",			/* 50 = listen */
	"getsockname",			/* 51 = getsockname */
	"getpeername",			/* 52 = getpeername */
	"socketpair",			/* 53 = socketpair */
	"setsockopt",			/* 54 = setsockopt */
	"getsockopt",			/* 55 = getsockopt */
	"clone",			/* 56 = clone */
	"fork",			/* 57 = fork */
	"__vfork14",			/* 58 = __vfork14 */
	"execve",			/* 59 = execve */
	"exit",			/* 60 = exit */
	"wait4",			/* 61 = wait4 */
	"kill",			/* 62 = kill */
	"uname",			/* 63 = uname */
	"#64 (unimplemented semget)",		/* 64 = unimplemented semget */
	"#65 (unimplemented semop)",		/* 65 = unimplemented semop */
	"#66 (unimplemented semctl)",		/* 66 = unimplemented semctl */
	"#67 (unimplemented shmdt)",		/* 67 = unimplemented shmdt */
	"#68 (unimplemented msgget)",		/* 68 = unimplemented msgget */
	"#69 (unimplemented msgsnd)",		/* 69 = unimplemented msgsnd */
	"#70 (unimplemented msgrcv)",		/* 70 = unimplemented msgrcv */
	"#71 (unimplemented msgctl)",		/* 71 = unimplemented msgctl */
	"fcntl",			/* 72 = fcntl */
	"flock",			/* 73 = flock */
	"fsync",			/* 74 = fsync */
	"fdatasync",			/* 75 = fdatasync */
	"truncate64",			/* 76 = truncate64 */
	"ftruncate64",			/* 77 = ftruncate64 */
	"getdents",			/* 78 = getdents */
	"__getcwd",			/* 79 = __getcwd */
	"chdir",			/* 80 = chdir */
	"fchdir",			/* 81 = fchdir */
	"rename",			/* 82 = rename */
	"mkdir",			/* 83 = mkdir */
	"rmdir",			/* 84 = rmdir */
	"creat",			/* 85 = creat */
	"link",			/* 86 = link */
	"unlink",			/* 87 = unlink */
	"symlink",			/* 88 = symlink */
	"readlink",			/* 89 = readlink */
	"chmod",			/* 90 = chmod */
	"fchmod",			/* 91 = fchmod */
	"chown",			/* 92 = chown */
	"__posix_fchown",			/* 93 = __posix_fchown */
	"lchown",			/* 94 = lchown */
	"umask",			/* 95 = umask */
	"gettimeofday",			/* 96 = gettimeofday */
	"getrlimit",			/* 97 = getrlimit */
	"getrusage",			/* 98 = getrusage */
	"sysinfo",			/* 99 = sysinfo */
	"times",			/* 100 = times */
	"ptrace",			/* 101 = ptrace */
	"getuid",			/* 102 = getuid */
	"#103 (unimplemented syslog)",		/* 103 = unimplemented syslog */
	"getgid",			/* 104 = getgid */
	"setuid",			/* 105 = setuid */
	"setgid",			/* 106 = setgid */
	"geteuid",			/* 107 = geteuid */
	"getegid",			/* 108 = getegid */
	"setpgid",			/* 109 = setpgid */
	"getppid",			/* 110 = getppid */
	"getpgrp",			/* 111 = getpgrp */
	"setsid",			/* 112 = setsid */
	"setreuid",			/* 113 = setreuid */
	"setregid",			/* 114 = setregid */
	"getgroups",			/* 115 = getgroups */
	"setgroups",			/* 116 = setgroups */
	"setresuid",			/* 117 = setresuid */
	"getresuid",			/* 118 = getresuid */
	"setresgid",			/* 119 = setresgid */
	"getresgid",			/* 120 = getresgid */
	"getpgid",			/* 121 = getpgid */
	"setfsuid",			/* 122 = setfsuid */
	"getfsuid",			/* 123 = getfsuid */
	"getsid",			/* 124 = getsid */
	"#125 (unimplemented capget)",		/* 125 = unimplemented capget */
	"#126 (unimplemented capset)",		/* 126 = unimplemented capset */
	"rt_sigpending",			/* 127 = rt_sigpending */
	"#128 (unimplemented rt_sigtimedwait)",		/* 128 = unimplemented rt_sigtimedwait */
	"rt_queueinfo",			/* 129 = rt_queueinfo */
	"rt_sigsuspend",			/* 130 = rt_sigsuspend */
	"sigaltstack",			/* 131 = sigaltstack */
	"utime",			/* 132 = utime */
	"mknod",			/* 133 = mknod */
#ifdef EXEC_AOUT
	"uselib",			/* 134 = uselib */
#else
	"#134 (unimplemented sys_uselib)",		/* 134 = unimplemented sys_uselib */
#endif
	"personality",			/* 135 = personality */
	"#136 (unimplemented ustat)",		/* 136 = unimplemented ustat */
	"statfs64",			/* 137 = statfs64 */
	"fstatfs64",			/* 138 = fstatfs64 */
	"#139 (unimplemented sysfs)",		/* 139 = unimplemented sysfs */
	"getpriority",			/* 140 = getpriority */
	"setpriority",			/* 141 = setpriority */
	"sched_setparam",			/* 142 = sched_setparam */
	"sched_getparam",			/* 143 = sched_getparam */
	"sched_setscheduler",			/* 144 = sched_setscheduler */
	"sched_getscheduler",			/* 145 = sched_getscheduler */
	"sched_get_priority_max",			/* 146 = sched_get_priority_max */
	"sched_get_priority_min",			/* 147 = sched_get_priority_min */
	"#148 (unimplemented sys_sched_rr_get_interval)",		/* 148 = unimplemented sys_sched_rr_get_interval */
	"mlock",			/* 149 = mlock */
	"munlock",			/* 150 = munlock */
	"mlockall",			/* 151 = mlockall */
	"munlockall",			/* 152 = munlockall */
	"#153 (unimplemented vhangup)",		/* 153 = unimplemented vhangup */
	"modify_ldt",			/* 154 = modify_ldt */
	"#155 (unimplemented pivot_root)",		/* 155 = unimplemented pivot_root */
	"__sysctl",			/* 156 = __sysctl */
	"#157 (unimplemented prctl)",		/* 157 = unimplemented prctl */
	"arch_prctl",			/* 158 = arch_prctl */
	"#159 (unimplemented adjtimex)",		/* 159 = unimplemented adjtimex */
	"setrlimit",			/* 160 = setrlimit */
	"chroot",			/* 161 = chroot */
	"sync",			/* 162 = sync */
	"acct",			/* 163 = acct */
	"settimeofday",			/* 164 = settimeofday */
	"#165 (unimplemented mount)",		/* 165 = unimplemented mount */
	"#166 (unimplemented umount2)",		/* 166 = unimplemented umount2 */
	"swapon",			/* 167 = swapon */
	"swapoff",			/* 168 = swapoff */
	"reboot",			/* 169 = reboot */
	"sethostname",			/* 170 = sethostname */
	"setdomainname",			/* 171 = setdomainname */
	"iopl",			/* 172 = iopl */
	"ioperm",			/* 173 = ioperm */
	"#174 (unimplemented create_module)",		/* 174 = unimplemented create_module */
	"#175 (unimplemented init_module)",		/* 175 = unimplemented init_module */
	"#176 (unimplemented delete_module)",		/* 176 = unimplemented delete_module */
	"#177 (unimplemented get_kernel_syms)",		/* 177 = unimplemented get_kernel_syms */
	"#178 (unimplemented query_module)",		/* 178 = unimplemented query_module */
	"#179 (unimplemented quotactl)",		/* 179 = unimplemented quotactl */
	"#180 (unimplemented nfsservctl)",		/* 180 = unimplemented nfsservctl */
	"#181 (unimplemented getpmsg)",		/* 181 = unimplemented getpmsg */
	"#182 (unimplemented putpmsg)",		/* 182 = unimplemented putpmsg */
	"#183 (unimplemented afs_syscall)",		/* 183 = unimplemented afs_syscall */
	"#184 (unimplemented tuxcall)",		/* 184 = unimplemented tuxcall */
	"#185 (unimplemented security)",		/* 185 = unimplemented security */
	"#186 (unimplemented gettid)",		/* 186 = unimplemented gettid */
	"#187 (unimplemented readahead)",		/* 187 = unimplemented readahead */
	"setxattr",			/* 188 = setxattr */
	"lsetxattr",			/* 189 = lsetxattr */
	"fsetxattr",			/* 190 = fsetxattr */
	"getxattr",			/* 191 = getxattr */
	"lgetxattr",			/* 192 = lgetxattr */
	"fgetxattr",			/* 193 = fgetxattr */
	"listxattr",			/* 194 = listxattr */
	"llistxattr",			/* 195 = llistxattr */
	"flistxattr",			/* 196 = flistxattr */
	"removexattr",			/* 197 = removexattr */
	"lremovexattr",			/* 198 = lremovexattr */
	"fremovexattr",			/* 199 = fremovexattr */
	"#200 (unimplemented tkill)",		/* 200 = unimplemented tkill */
	"time",			/* 201 = time */
	"#202 (unimplemented futex)",		/* 202 = unimplemented futex */
	"#203 (unimplemented sched_setaffinity)",		/* 203 = unimplemented sched_setaffinity */
	"#204 (unimplemented sched_getaffinity)",		/* 204 = unimplemented sched_getaffinity */
	"#205 (unimplemented set_thread_area)",		/* 205 = unimplemented set_thread_area */
	"#206 (unimplemented io_setup)",		/* 206 = unimplemented io_setup */
	"#207 (unimplemented io_destroy)",		/* 207 = unimplemented io_destroy */
	"#208 (unimplemented io_getevents)",		/* 208 = unimplemented io_getevents */
	"#209 (unimplemented io_submit)",		/* 209 = unimplemented io_submit */
	"#210 (unimplemented io_cancel)",		/* 210 = unimplemented io_cancel */
	"#211 (unimplemented get_thread_area)",		/* 211 = unimplemented get_thread_area */
	"#212 (unimplemented lookup_dcookie)",		/* 212 = unimplemented lookup_dcookie */
	"#213 (unimplemented epoll_create)",		/* 213 = unimplemented epoll_create */
	"#214 (unimplemented epoll_ctl_old)",		/* 214 = unimplemented epoll_ctl_old */
	"#215 (unimplemented epoll_wait_old)",		/* 215 = unimplemented epoll_wait_old */
	"#216 (unimplemented remap_file_pages)",		/* 216 = unimplemented remap_file_pages */
	"getdents64",			/* 217 = getdents64 */
	"set_tid_address",			/* 218 = set_tid_address */
	"#219 (unimplemented restart_syscall)",		/* 219 = unimplemented restart_syscall */
	"#220 (unimplemented semtimedop)",		/* 220 = unimplemented semtimedop */
	"#221 (unimplemented fadvise64)",		/* 221 = unimplemented fadvise64 */
	"#222 (unimplemented timer_create)",		/* 222 = unimplemented timer_create */
	"#223 (unimplemented timer_settime)",		/* 223 = unimplemented timer_settime */
	"#224 (unimplemented timer_gettime)",		/* 224 = unimplemented timer_gettime */
	"#225 (unimplemented timer_getoverrun)",		/* 225 = unimplemented timer_getoverrun */
	"#226 (unimplemented timer_delete)",		/* 226 = unimplemented timer_delete */
	"clock_settime",			/* 227 = clock_settime */
	"clock_gettime",			/* 228 = clock_gettime */
	"clock_getres",			/* 229 = clock_getres */
	"clock_nanosleep",			/* 230 = clock_nanosleep */
	"exit_group",			/* 231 = exit_group */
	"#232 (unimplemented epoll_wait)",		/* 232 = unimplemented epoll_wait */
	"#233 (unimplemented epoll_ctl)",		/* 233 = unimplemented epoll_ctl */
	"#234 (unimplemented tgkill)",		/* 234 = unimplemented tgkill */
	"#235 (unimplemented utimes)",		/* 235 = unimplemented utimes */
	"#236 (unimplemented vserver)",		/* 236 = unimplemented vserver */
	"nosys",			/* 237 = nosys */
};
