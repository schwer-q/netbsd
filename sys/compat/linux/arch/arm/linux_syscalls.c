/* $NetBSD: linux_syscalls.c,v 1.21 2005/02/26 23:20:01 perry Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.20 2005/02/26 23:10:19 perry Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.21 2005/02/26 23:20:01 perry Exp $");

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
	"nosys",			/* 0 = nosys */
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
	"ugetrlimit",			/* 191 = ugetrlimit */
	"mmap2",			/* 192 = mmap2 */
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
	"getdents64",			/* 217 = getdents64 */
	"#218 (unimplemented pivot_root)",		/* 218 = unimplemented pivot_root */
	"mincore",			/* 219 = mincore */
	"madvise",			/* 220 = madvise */
	"fcntl64",			/* 221 = fcntl64 */
	"#222 (unimplemented / * for tux * /)",		/* 222 = unimplemented / * for tux * / */
	"#223 (unimplemented / * unused * /)",		/* 223 = unimplemented / * unused * / */
	"#224 (unimplemented gettid)",		/* 224 = unimplemented gettid */
	"#225 (unimplemented readahead)",		/* 225 = unimplemented readahead */
	"#226 (unimplemented setxattr)",		/* 226 = unimplemented setxattr */
	"#227 (unimplemented lsetxattr)",		/* 227 = unimplemented lsetxattr */
	"#228 (unimplemented fsetxattr)",		/* 228 = unimplemented fsetxattr */
	"#229 (unimplemented getxattr)",		/* 229 = unimplemented getxattr */
	"#230 (unimplemented lgetxattr)",		/* 230 = unimplemented lgetxattr */
	"#231 (unimplemented fgetxattr)",		/* 231 = unimplemented fgetxattr */
	"#232 (unimplemented listxattr)",		/* 232 = unimplemented listxattr */
	"#233 (unimplemented llistxattr)",		/* 233 = unimplemented llistxattr */
	"#234 (unimplemented flistxattr)",		/* 234 = unimplemented flistxattr */
	"#235 (unimplemented removexattr)",		/* 235 = unimplemented removexattr */
	"#236 (unimplemented lremovexattr)",		/* 236 = unimplemented lremovexattr */
	"#237 (unimplemented fremovexattr)",		/* 237 = unimplemented fremovexattr */
	"#238 (unimplemented tkill)",		/* 238 = unimplemented tkill */
	"#239 (unimplemented sendfile64)",		/* 239 = unimplemented sendfile64 */
	"#240 (unimplemented futex)",		/* 240 = unimplemented futex */
	"#241 (unimplemented sched_setaffinity)",		/* 241 = unimplemented sched_setaffinity */
	"#242 (unimplemented sched_getaffinity)",		/* 242 = unimplemented sched_getaffinity */
	"#243 (unimplemented io_setup)",		/* 243 = unimplemented io_setup */
	"#244 (unimplemented io_destroy)",		/* 244 = unimplemented io_destroy */
	"#245 (unimplemented io_getevents)",		/* 245 = unimplemented io_getevents */
	"#246 (unimplemented io_submit)",		/* 246 = unimplemented io_submit */
	"#247 (unimplemented io_cancel)",		/* 247 = unimplemented io_cancel */
	"exit_group",			/* 248 = exit_group */
	"#249 (unimplemented lookup_dcookie)",		/* 249 = unimplemented lookup_dcookie */
	"#250 (unimplemented epoll_create)",		/* 250 = unimplemented epoll_create */
	"#251 (unimplemented epoll_ctl)",		/* 251 = unimplemented epoll_ctl */
	"#252 (unimplemented epoll_wait)",		/* 252 = unimplemented epoll_wait */
	"#253 (unimplemented remap_file_pages)",		/* 253 = unimplemented remap_file_pages */
	"#254 (unimplemented / * for set_thread_area * /)",		/* 254 = unimplemented / * for set_thread_area * / */
	"#255 (unimplemented / * for get_thread_area * /)",		/* 255 = unimplemented / * for get_thread_area * / */
	"#256 (unimplemented / * for set_tid_address * /)",		/* 256 = unimplemented / * for set_tid_address * / */
	"#257 (unimplemented timer_create)",		/* 257 = unimplemented timer_create */
	"#258 (unimplemented timer_settime)",		/* 258 = unimplemented timer_settime */
	"#259 (unimplemented timer_gettime)",		/* 259 = unimplemented timer_gettime */
	"#260 (unimplemented timer_getoverrun)",		/* 260 = unimplemented timer_getoverrun */
	"#261 (unimplemented timer_delete)",		/* 261 = unimplemented timer_delete */
	"#262 (unimplemented clock_settime)",		/* 262 = unimplemented clock_settime */
	"#263 (unimplemented clock_gettime)",		/* 263 = unimplemented clock_gettime */
	"#264 (unimplemented clock_getres)",		/* 264 = unimplemented clock_getres */
	"#265 (unimplemented clock_nanosleep)",		/* 265 = unimplemented clock_nanosleep */
	"statfs64",			/* 266 = statfs64 */
	"fstatfs64",			/* 267 = fstatfs64 */
	"#268 (unimplemented tgkill)",		/* 268 = unimplemented tgkill */
	"#269 (unimplemented utimes)",		/* 269 = unimplemented utimes */
	"#270 (unimplemented fadvise64_64)",		/* 270 = unimplemented fadvise64_64 */
	"#271 (unimplemented pciconfig_iobase)",		/* 271 = unimplemented pciconfig_iobase */
	"#272 (unimplemented pciconfig_read)",		/* 272 = unimplemented pciconfig_read */
	"#273 (unimplemented pciconfig_write)",		/* 273 = unimplemented pciconfig_write */
	"#274 (unimplemented)",		/* 274 = unimplemented */
	"#275 (unimplemented)",		/* 275 = unimplemented */
	"#276 (unimplemented)",		/* 276 = unimplemented */
	"#277 (unimplemented)",		/* 277 = unimplemented */
	"#278 (unimplemented)",		/* 278 = unimplemented */
	"#279 (unimplemented)",		/* 279 = unimplemented */
	"#280 (unimplemented)",		/* 280 = unimplemented */
	"#281 (unimplemented)",		/* 281 = unimplemented */
	"#282 (unimplemented)",		/* 282 = unimplemented */
	"#283 (unimplemented)",		/* 283 = unimplemented */
	"#284 (unimplemented)",		/* 284 = unimplemented */
	"#285 (unimplemented)",		/* 285 = unimplemented */
	"#286 (unimplemented)",		/* 286 = unimplemented */
	"#287 (unimplemented)",		/* 287 = unimplemented */
	"#288 (unimplemented)",		/* 288 = unimplemented */
	"#289 (unimplemented)",		/* 289 = unimplemented */
	"#290 (unimplemented)",		/* 290 = unimplemented */
	"#291 (unimplemented)",		/* 291 = unimplemented */
	"#292 (unimplemented)",		/* 292 = unimplemented */
	"#293 (unimplemented)",		/* 293 = unimplemented */
	"#294 (unimplemented)",		/* 294 = unimplemented */
	"#295 (unimplemented)",		/* 295 = unimplemented */
	"#296 (unimplemented)",		/* 296 = unimplemented */
	"#297 (unimplemented)",		/* 297 = unimplemented */
	"#298 (unimplemented)",		/* 298 = unimplemented */
	"#299 (unimplemented)",		/* 299 = unimplemented */
	"#300 (unimplemented)",		/* 300 = unimplemented */
	"#301 (unimplemented)",		/* 301 = unimplemented */
	"#302 (unimplemented)",		/* 302 = unimplemented */
	"#303 (unimplemented)",		/* 303 = unimplemented */
	"#304 (unimplemented)",		/* 304 = unimplemented */
	"#305 (unimplemented)",		/* 305 = unimplemented */
	"#306 (unimplemented)",		/* 306 = unimplemented */
	"#307 (unimplemented)",		/* 307 = unimplemented */
	"#308 (unimplemented)",		/* 308 = unimplemented */
	"#309 (unimplemented)",		/* 309 = unimplemented */
	"#310 (unimplemented)",		/* 310 = unimplemented */
	"#311 (unimplemented)",		/* 311 = unimplemented */
	"#312 (unimplemented)",		/* 312 = unimplemented */
	"#313 (unimplemented)",		/* 313 = unimplemented */
	"#314 (unimplemented)",		/* 314 = unimplemented */
	"#315 (unimplemented)",		/* 315 = unimplemented */
	"#316 (unimplemented)",		/* 316 = unimplemented */
	"#317 (unimplemented)",		/* 317 = unimplemented */
	"#318 (unimplemented)",		/* 318 = unimplemented */
	"#319 (unimplemented)",		/* 319 = unimplemented */
	"#320 (unimplemented)",		/* 320 = unimplemented */
	"#321 (unimplemented)",		/* 321 = unimplemented */
	"#322 (unimplemented)",		/* 322 = unimplemented */
	"#323 (unimplemented)",		/* 323 = unimplemented */
	"#324 (unimplemented)",		/* 324 = unimplemented */
	"#325 (unimplemented)",		/* 325 = unimplemented */
	"#326 (unimplemented)",		/* 326 = unimplemented */
	"#327 (unimplemented)",		/* 327 = unimplemented */
	"#328 (unimplemented)",		/* 328 = unimplemented */
	"#329 (unimplemented)",		/* 329 = unimplemented */
	"#330 (unimplemented)",		/* 330 = unimplemented */
	"#331 (unimplemented)",		/* 331 = unimplemented */
	"#332 (unimplemented)",		/* 332 = unimplemented */
	"#333 (unimplemented)",		/* 333 = unimplemented */
	"#334 (unimplemented)",		/* 334 = unimplemented */
	"#335 (unimplemented)",		/* 335 = unimplemented */
	"#336 (unimplemented)",		/* 336 = unimplemented */
	"#337 (unimplemented)",		/* 337 = unimplemented */
	"#338 (unimplemented)",		/* 338 = unimplemented */
	"#339 (unimplemented)",		/* 339 = unimplemented */
	"#340 (unimplemented)",		/* 340 = unimplemented */
	"#341 (unimplemented)",		/* 341 = unimplemented */
	"#342 (unimplemented)",		/* 342 = unimplemented */
	"#343 (unimplemented)",		/* 343 = unimplemented */
	"#344 (unimplemented)",		/* 344 = unimplemented */
	"#345 (unimplemented)",		/* 345 = unimplemented */
	"#346 (unimplemented)",		/* 346 = unimplemented */
	"#347 (unimplemented)",		/* 347 = unimplemented */
	"#348 (unimplemented)",		/* 348 = unimplemented */
	"#349 (unimplemented)",		/* 349 = unimplemented */
	"#350 (unimplemented)",		/* 350 = unimplemented */
	"#351 (unimplemented)",		/* 351 = unimplemented */
	"#352 (unimplemented)",		/* 352 = unimplemented */
	"#353 (unimplemented)",		/* 353 = unimplemented */
	"#354 (unimplemented)",		/* 354 = unimplemented */
	"#355 (unimplemented)",		/* 355 = unimplemented */
	"#356 (unimplemented)",		/* 356 = unimplemented */
	"#357 (unimplemented)",		/* 357 = unimplemented */
	"#358 (unimplemented)",		/* 358 = unimplemented */
	"#359 (unimplemented)",		/* 359 = unimplemented */
	"#360 (unimplemented)",		/* 360 = unimplemented */
	"#361 (unimplemented)",		/* 361 = unimplemented */
	"#362 (unimplemented)",		/* 362 = unimplemented */
	"#363 (unimplemented)",		/* 363 = unimplemented */
	"#364 (unimplemented)",		/* 364 = unimplemented */
	"#365 (unimplemented)",		/* 365 = unimplemented */
	"#366 (unimplemented)",		/* 366 = unimplemented */
	"#367 (unimplemented)",		/* 367 = unimplemented */
	"#368 (unimplemented)",		/* 368 = unimplemented */
	"#369 (unimplemented)",		/* 369 = unimplemented */
	"#370 (unimplemented)",		/* 370 = unimplemented */
	"#371 (unimplemented)",		/* 371 = unimplemented */
	"#372 (unimplemented)",		/* 372 = unimplemented */
	"#373 (unimplemented)",		/* 373 = unimplemented */
	"#374 (unimplemented)",		/* 374 = unimplemented */
	"#375 (unimplemented)",		/* 375 = unimplemented */
	"#376 (unimplemented)",		/* 376 = unimplemented */
	"#377 (unimplemented)",		/* 377 = unimplemented */
	"#378 (unimplemented)",		/* 378 = unimplemented */
	"#379 (unimplemented)",		/* 379 = unimplemented */
	"#380 (unimplemented)",		/* 380 = unimplemented */
	"#381 (unimplemented)",		/* 381 = unimplemented */
	"#382 (unimplemented)",		/* 382 = unimplemented */
	"#383 (unimplemented)",		/* 383 = unimplemented */
	"#384 (unimplemented / * base * /)",		/* 384 = unimplemented / * base * / */
	"breakpoint",			/* 385 = breakpoint */
	"cacheflush",			/* 386 = cacheflush */
	"#387 (unimplemented usr26)",		/* 387 = unimplemented usr26 */
	"#388 (unimplemented usr32)",		/* 388 = unimplemented usr32 */
};
