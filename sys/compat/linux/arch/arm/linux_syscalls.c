/* $NetBSD: linux_syscalls.c,v 1.4 2002/01/23 16:14:45 bjh21 Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.4 2002/01/23 15:54:34 bjh21 Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.4 2002/01/23 16:14:45 bjh21 Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	"#0 (unimplemented)",		/* 0 = unimplemented */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"waitpid",			/* 7 = waitpid */
	"creat",			/* 8 = creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"execve",			/* 11 = execve */
	"chdir",			/* 12 = chdir */
	"time",			/* 13 = time */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"lchown16",			/* 16 = lchown16 */
	"#17 (obsolete break)",		/* 17 = obsolete break */
	"#18 (obsolete ostat)",		/* 18 = obsolete ostat */
	"lseek",			/* 19 = lseek */
	"getpid",			/* 20 = getpid */
	"#21 (unimplemented mount)",		/* 21 = unimplemented mount */
	"#22 (obsolete umount)",		/* 22 = obsolete umount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"stime",			/* 25 = stime */
	"ptrace",			/* 26 = ptrace */
	"alarm",			/* 27 = alarm */
	"#28 (obsolete ofstat)",		/* 28 = obsolete ofstat */
	"pause",			/* 29 = pause */
	"utime",			/* 30 = utime */
	"#31 (obsolete stty)",		/* 31 = obsolete stty */
	"#32 (obsolete gtty)",		/* 32 = obsolete gtty */
	"access",			/* 33 = access */
	"nice",			/* 34 = nice */
	"#35 (obsolete ftime)",		/* 35 = obsolete ftime */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"rename",			/* 38 = rename */
	"mkdir",			/* 39 = mkdir */
	"rmdir",			/* 40 = rmdir */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"times",			/* 43 = times */
	"#44 (obsolete prof)",		/* 44 = obsolete prof */
	"brk",			/* 45 = brk */
	"setgid",			/* 46 = setgid */
	"getgid",			/* 47 = getgid */
	"signal",			/* 48 = signal */
	"geteuid",			/* 49 = geteuid */
	"getegid",			/* 50 = getegid */
	"acct",			/* 51 = acct */
	"#52 (unimplemented umount)",		/* 52 = unimplemented umount */
	"#53 (obsolete lock)",		/* 53 = obsolete lock */
	"ioctl",			/* 54 = ioctl */
	"fcntl",			/* 55 = fcntl */
	"#56 (obsolete mpx)",		/* 56 = obsolete mpx */
	"setpgid",			/* 57 = setpgid */
	"#58 (obsolete ulimit)",		/* 58 = obsolete ulimit */
	"oldolduname",			/* 59 = oldolduname */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"#62 (unimplemented ustat)",		/* 62 = unimplemented ustat */
	"dup2",			/* 63 = dup2 */
	"getppid",			/* 64 = getppid */
	"getpgrp",			/* 65 = getpgrp */
	"setsid",			/* 66 = setsid */
	"sigaction",			/* 67 = sigaction */
	"siggetmask",			/* 68 = siggetmask */
	"sigsetmask",			/* 69 = sigsetmask */
	"setreuid",			/* 70 = setreuid */
	"setregid",			/* 71 = setregid */
	"sigsuspend",			/* 72 = sigsuspend */
	"sigpending",			/* 73 = sigpending */
	"sethostname",			/* 74 = sethostname */
	"setrlimit",			/* 75 = setrlimit */
	"getrlimit",			/* 76 = getrlimit */
	"getrusage",			/* 77 = getrusage */
	"gettimeofday",			/* 78 = gettimeofday */
	"settimeofday",			/* 79 = settimeofday */
	"getgroups",			/* 80 = getgroups */
	"setgroups",			/* 81 = setgroups */
	"oldselect",			/* 82 = oldselect */
	"symlink",			/* 83 = symlink */
	"oolstat",			/* 84 = oolstat */
	"readlink",			/* 85 = readlink */
#ifdef EXEC_AOUT
	"uselib",			/* 86 = uselib */
#else
	"#86 (excluded uselib)",		/* 86 = excluded uselib */
#endif
	"swapon",			/* 87 = swapon */
	"reboot",			/* 88 = reboot */
	"readdir",			/* 89 = readdir */
	"old_mmap",			/* 90 = old_mmap */
	"munmap",			/* 91 = munmap */
	"truncate",			/* 92 = truncate */
	"ftruncate",			/* 93 = ftruncate */
	"fchmod",			/* 94 = fchmod */
	"fchown16",			/* 95 = fchown16 */
	"getpriority",			/* 96 = getpriority */
	"setpriority",			/* 97 = setpriority */
	"profil",			/* 98 = profil */
	"statfs",			/* 99 = statfs */
	"fstatfs",			/* 100 = fstatfs */
	"#101 (unimplemented)",		/* 101 = unimplemented */
	"socketcall",			/* 102 = socketcall */
	"#103 (unimplemented syslog)",		/* 103 = unimplemented syslog */
	"setitimer",			/* 104 = setitimer */
	"getitimer",			/* 105 = getitimer */
	"stat",			/* 106 = stat */
	"lstat",			/* 107 = lstat */
	"fstat",			/* 108 = fstat */
	"olduname",			/* 109 = olduname */
	"#110 (unimplemented)",		/* 110 = unimplemented */
	"#111 (unimplemented vhangup)",		/* 111 = unimplemented vhangup */
	"#112 (unimplemented idle)",		/* 112 = unimplemented idle */
	"#113 (unimplemented syscall)",		/* 113 = unimplemented syscall */
	"wait4",			/* 114 = wait4 */
	"swapoff",			/* 115 = swapoff */
	"sysinfo",			/* 116 = sysinfo */
	"ipc",			/* 117 = ipc */
	"fsync",			/* 118 = fsync */
	"sigreturn",			/* 119 = sigreturn */
	"clone",			/* 120 = clone */
	"setdomainname",			/* 121 = setdomainname */
	"uname",			/* 122 = uname */
	"#123 (unimplemented modify_ldt)",		/* 123 = unimplemented modify_ldt */
	"#124 (unimplemented adjtimex)",		/* 124 = unimplemented adjtimex */
	"mprotect",			/* 125 = mprotect */
	"sigprocmask",			/* 126 = sigprocmask */
	"#127 (unimplemented create_module)",		/* 127 = unimplemented create_module */
	"#128 (unimplemented init_module)",		/* 128 = unimplemented init_module */
	"#129 (unimplemented delete_module)",		/* 129 = unimplemented delete_module */
	"#130 (unimplemented get_kernel_syms)",		/* 130 = unimplemented get_kernel_syms */
	"#131 (unimplemented quotactl)",		/* 131 = unimplemented quotactl */
	"getpgid",			/* 132 = getpgid */
	"fchdir",			/* 133 = fchdir */
	"#134 (unimplemented bdflush)",		/* 134 = unimplemented bdflush */
	"#135 (unimplemented sysfs)",		/* 135 = unimplemented sysfs */
	"personality",			/* 136 = personality */
	"#137 (unimplemented afs_syscall)",		/* 137 = unimplemented afs_syscall */
	"setfsuid",			/* 138 = setfsuid */
	"getfsuid",			/* 139 = getfsuid */
	"llseek",			/* 140 = llseek */
	"getdents",			/* 141 = getdents */
	"select",			/* 142 = select */
	"flock",			/* 143 = flock */
	"msync",			/* 144 = msync */
	"readv",			/* 145 = readv */
	"writev",			/* 146 = writev */
	"getsid",			/* 147 = getsid */
	"fdatasync",			/* 148 = fdatasync */
	"__sysctl",			/* 149 = __sysctl */
	"mlock",			/* 150 = mlock */
	"munlock",			/* 151 = munlock */
	"mlockall",			/* 152 = mlockall */
	"munlockall",			/* 153 = munlockall */
	"sched_setparam",			/* 154 = sched_setparam */
	"sched_getparam",			/* 155 = sched_getparam */
	"sched_setscheduler",			/* 156 = sched_setscheduler */
	"sched_getscheduler",			/* 157 = sched_getscheduler */
	"sched_yield",			/* 158 = sched_yield */
	"sched_get_priority_max",			/* 159 = sched_get_priority_max */
	"sched_get_priority_min",			/* 160 = sched_get_priority_min */
	"#161 (unimplemented sched_rr_get_interval)",		/* 161 = unimplemented sched_rr_get_interval */
	"nanosleep",			/* 162 = nanosleep */
	"mremap",			/* 163 = mremap */
	"setresuid",			/* 164 = setresuid */
	"getresuid",			/* 165 = getresuid */
	"#166 (unimplemented)",		/* 166 = unimplemented */
	"#167 (unimplemented query_module)",		/* 167 = unimplemented query_module */
	"poll",			/* 168 = poll */
	"#169 (unimplemented nfsservctl)",		/* 169 = unimplemented nfsservctl */
	"setresgid",			/* 170 = setresgid */
	"getresgid",			/* 171 = getresgid */
	"#172 (unimplemented prctl)",		/* 172 = unimplemented prctl */
	"#173 (unimplemented rt_sigreturn)",		/* 173 = unimplemented rt_sigreturn */
	"rt_sigaction",			/* 174 = rt_sigaction */
	"rt_sigprocmask",			/* 175 = rt_sigprocmask */
	"rt_sigpending",			/* 176 = rt_sigpending */
	"#177 (unimplemented rt_sigtimedwait)",		/* 177 = unimplemented rt_sigtimedwait */
	"rt_queueinfo",			/* 178 = rt_queueinfo */
	"rt_sigsuspend",			/* 179 = rt_sigsuspend */
	"pread",			/* 180 = pread */
	"pwrite",			/* 181 = pwrite */
	"chown16",			/* 182 = chown16 */
	"__getcwd",			/* 183 = __getcwd */
	"#184 (unimplemented capget)",		/* 184 = unimplemented capget */
	"#185 (unimplemented capset)",		/* 185 = unimplemented capset */
	"sigaltstack",			/* 186 = sigaltstack */
	"#187 (unimplemented sendfile)",		/* 187 = unimplemented sendfile */
	"#188 (unimplemented getpmsg)",		/* 188 = unimplemented getpmsg */
	"#189 (unimplemented putpmsg)",		/* 189 = unimplemented putpmsg */
	"vfork",			/* 190 = vfork */
	"#191 (unimplemented getrlimit)",		/* 191 = unimplemented getrlimit */
	"#192 (unimplemented mmap2)",		/* 192 = unimplemented mmap2 */
	"truncate64",			/* 193 = truncate64 */
	"ftruncate64",			/* 194 = ftruncate64 */
	"stat64",			/* 195 = stat64 */
	"lstat64",			/* 196 = lstat64 */
	"fstat64",			/* 197 = fstat64 */
	"lchown",			/* 198 = lchown */
	"getuid32",			/* 199 = getuid32 */
	"getgid32",			/* 200 = getgid32 */
	"geteuid32",			/* 201 = geteuid32 */
	"getegid32",			/* 202 = getegid32 */
	"setreuid32",			/* 203 = setreuid32 */
	"setregid32",			/* 204 = setregid32 */
	"getgroups32",			/* 205 = getgroups32 */
	"setgroups32",			/* 206 = setgroups32 */
	"fchown32",			/* 207 = fchown32 */
	"setresuid32",			/* 208 = setresuid32 */
	"getresuid32",			/* 209 = getresuid32 */
	"setresgid32",			/* 210 = setresgid32 */
	"getresgid32",			/* 211 = getresgid32 */
	"chown32",			/* 212 = chown32 */
	"setuid32",			/* 213 = setuid32 */
	"setgid32",			/* 214 = setgid32 */
	"setfsuid32",			/* 215 = setfsuid32 */
	"getfsuid32",			/* 216 = getfsuid32 */
	"#217 (unimplemented getdents64)",		/* 217 = unimplemented getdents64 */
	"#218 (unimplemented pivot_root)",		/* 218 = unimplemented pivot_root */
	"#219 (unimplemented mincore)",		/* 219 = unimplemented mincore */
	"#220 (unimplemented madvise)",		/* 220 = unimplemented madvise */
	"#221 (unimplemented fcntl64)",		/* 221 = unimplemented fcntl64 */
	"#222 (unimplemented)",		/* 222 = unimplemented */
	"#223 (unimplemented)",		/* 223 = unimplemented */
	"#224 (unimplemented)",		/* 224 = unimplemented */
	"#225 (unimplemented)",		/* 225 = unimplemented */
	"#226 (unimplemented)",		/* 226 = unimplemented */
	"#227 (unimplemented)",		/* 227 = unimplemented */
	"#228 (unimplemented)",		/* 228 = unimplemented */
	"#229 (unimplemented)",		/* 229 = unimplemented */
	"#230 (unimplemented)",		/* 230 = unimplemented */
	"#231 (unimplemented)",		/* 231 = unimplemented */
	"#232 (unimplemented)",		/* 232 = unimplemented */
	"#233 (unimplemented)",		/* 233 = unimplemented */
	"#234 (unimplemented)",		/* 234 = unimplemented */
	"#235 (unimplemented)",		/* 235 = unimplemented */
	"#236 (unimplemented)",		/* 236 = unimplemented */
	"#237 (unimplemented)",		/* 237 = unimplemented */
	"#238 (unimplemented)",		/* 238 = unimplemented */
	"#239 (unimplemented)",		/* 239 = unimplemented */
	"#240 (unimplemented)",		/* 240 = unimplemented */
	"#241 (unimplemented)",		/* 241 = unimplemented */
	"#242 (unimplemented)",		/* 242 = unimplemented */
	"#243 (unimplemented)",		/* 243 = unimplemented */
	"#244 (unimplemented)",		/* 244 = unimplemented */
	"#245 (unimplemented)",		/* 245 = unimplemented */
	"#246 (unimplemented)",		/* 246 = unimplemented */
	"#247 (unimplemented)",		/* 247 = unimplemented */
	"#248 (unimplemented)",		/* 248 = unimplemented */
	"#249 (unimplemented)",		/* 249 = unimplemented */
	"#250 (unimplemented)",		/* 250 = unimplemented */
	"#251 (unimplemented)",		/* 251 = unimplemented */
	"#252 (unimplemented)",		/* 252 = unimplemented */
	"#253 (unimplemented)",		/* 253 = unimplemented */
	"#254 (unimplemented)",		/* 254 = unimplemented */
	"#255 (unimplemented)",		/* 255 = unimplemented */
	"#256 (unimplemented)",		/* 256 = unimplemented */
	"#257 (unimplemented breakpoint)",		/* 257 = unimplemented breakpoint */
	"cacheflush",			/* 258 = cacheflush */
	"#259 (unimplemented usr26)",		/* 259 = unimplemented usr26 */
	"#260 (unimplemented usr32)",		/* 260 = unimplemented usr32 */
};
