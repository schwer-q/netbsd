/* $NetBSD: linux_syscalls.c,v 1.55 2005/02/26 23:20:01 perry Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.49 2005/02/26 23:10:19 perry Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.55 2005/02/26 23:20:01 perry Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "opt_compat_osf1.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_msg.h>
#include <compat/linux/common/linux_sem.h>
#include <compat/linux/common/linux_shm.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"#5 (unimplemented)",		/* 5 = unimplemented */
	"close",			/* 6 = close */
	"wait4",			/* 7 = wait4 */
	"creat",			/* 8 = creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"#11 (unimplemented)",		/* 11 = unimplemented */
	"chdir",			/* 12 = chdir */
	"fchdir",			/* 13 = fchdir */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"__posix_chown",			/* 16 = __posix_chown */
	"brk",			/* 17 = brk */
	"#18 (unimplemented)",		/* 18 = unimplemented */
	"lseek",			/* 19 = lseek */
	"getpid_with_ppid",			/* 20 = getpid_with_ppid */
	"mount",			/* 21 = mount */
	"#22 (unimplemented umount)",		/* 22 = unimplemented umount */
	"setuid",			/* 23 = setuid */
	"getuid_with_euid",			/* 24 = getuid_with_euid */
	"#25 (unimplemented)",		/* 25 = unimplemented */
	"ptrace",			/* 26 = ptrace */
	"#27 (unimplemented)",		/* 27 = unimplemented */
	"#28 (unimplemented)",		/* 28 = unimplemented */
	"#29 (unimplemented)",		/* 29 = unimplemented */
	"#30 (unimplemented)",		/* 30 = unimplemented */
	"#31 (unimplemented)",		/* 31 = unimplemented */
	"#32 (unimplemented)",		/* 32 = unimplemented */
	"access",			/* 33 = access */
	"#34 (unimplemented)",		/* 34 = unimplemented */
	"#35 (unimplemented)",		/* 35 = unimplemented */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"#38 (unimplemented)",		/* 38 = unimplemented */
	"setpgid",			/* 39 = setpgid */
	"#40 (unimplemented)",		/* 40 = unimplemented */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"set_program_attributes",			/* 43 = set_program_attributes */
	"#44 (unimplemented)",		/* 44 = unimplemented */
	"open",			/* 45 = open */
	"#46 (unimplemented)",		/* 46 = unimplemented */
	"getgid_with_egid",			/* 47 = getgid_with_egid */
	"sigprocmask",			/* 48 = sigprocmask */
	"#49 (unimplemented)",		/* 49 = unimplemented */
	"#50 (unimplemented)",		/* 50 = unimplemented */
	"acct",			/* 51 = acct */
	"sigpending",			/* 52 = sigpending */
	"#53 (unimplemented)",		/* 53 = unimplemented */
	"ioctl",			/* 54 = ioctl */
	"#55 (unimplemented)",		/* 55 = unimplemented */
	"#56 (unimplemented)",		/* 56 = unimplemented */
	"symlink",			/* 57 = symlink */
	"readlink",			/* 58 = readlink */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"#62 (unimplemented)",		/* 62 = unimplemented */
	"getpgrp",			/* 63 = getpgrp */
	"getpagesize",			/* 64 = getpagesize */
	"#65 (unimplemented)",		/* 65 = unimplemented */
	"__vfork14",			/* 66 = __vfork14 */
	"stat",			/* 67 = stat */
	"lstat",			/* 68 = lstat */
	"#69 (unimplemented)",		/* 69 = unimplemented */
	"#70 (unimplemented)",		/* 70 = unimplemented */
	"mmap",			/* 71 = mmap */
	"#72 (unimplemented)",		/* 72 = unimplemented */
	"munmap",			/* 73 = munmap */
	"mprotect",			/* 74 = mprotect */
	"madvise",			/* 75 = madvise */
	"#76 (unimplemented vhangup)",		/* 76 = unimplemented vhangup */
	"#77 (unimplemented)",		/* 77 = unimplemented */
	"#78 (unimplemented)",		/* 78 = unimplemented */
	"getgroups",			/* 79 = getgroups */
	"setgroups",			/* 80 = setgroups */
	"#81 (unimplemented)",		/* 81 = unimplemented */
	"#82 (unimplemented setpgrp)",		/* 82 = unimplemented setpgrp */
	"setitimer",			/* 83 = setitimer */
	"#84 (unimplemented)",		/* 84 = unimplemented */
	"#85 (unimplemented)",		/* 85 = unimplemented */
	"#86 (unimplemented osf1_sys_getitimer)",		/* 86 = unimplemented osf1_sys_getitimer */
	"gethostname",			/* 87 = gethostname */
	"sethostname",			/* 88 = sethostname */
	"#89 (unimplemented getdtablesize)",		/* 89 = unimplemented getdtablesize */
	"dup2",			/* 90 = dup2 */
	"fstat",			/* 91 = fstat */
	"fcntl",			/* 92 = fcntl */
	"select",			/* 93 = select */
	"poll",			/* 94 = poll */
	"fsync",			/* 95 = fsync */
	"setpriority",			/* 96 = setpriority */
	"socket",			/* 97 = socket */
	"connect",			/* 98 = connect */
	"accept",			/* 99 = accept */
	"getpriority",			/* 100 = getpriority */
	"send",			/* 101 = send */
	"recv",			/* 102 = recv */
	"sigreturn",			/* 103 = sigreturn */
	"bind",			/* 104 = bind */
	"setsockopt",			/* 105 = setsockopt */
	"listen",			/* 106 = listen */
	"#107 (unimplemented)",		/* 107 = unimplemented */
	"#108 (unimplemented)",		/* 108 = unimplemented */
	"#109 (unimplemented)",		/* 109 = unimplemented */
	"#110 (unimplemented)",		/* 110 = unimplemented */
	"sigsuspend",			/* 111 = sigsuspend */
	"sigstack",			/* 112 = sigstack */
	"recvmsg",			/* 113 = recvmsg */
	"sendmsg",			/* 114 = sendmsg */
	"#115 (unimplemented)",		/* 115 = unimplemented */
	"gettimeofday",			/* 116 = gettimeofday */
	"getrusage",			/* 117 = getrusage */
	"getsockopt",			/* 118 = getsockopt */
	"#119 (unimplemented)",		/* 119 = unimplemented */
	"readv",			/* 120 = readv */
	"writev",			/* 121 = writev */
	"settimeofday",			/* 122 = settimeofday */
	"__posix_fchown",			/* 123 = __posix_fchown */
	"fchmod",			/* 124 = fchmod */
	"recvfrom",			/* 125 = recvfrom */
	"setreuid",			/* 126 = setreuid */
	"setregid",			/* 127 = setregid */
	"rename",			/* 128 = rename */
	"truncate",			/* 129 = truncate */
	"ftruncate",			/* 130 = ftruncate */
	"flock",			/* 131 = flock */
	"setgid",			/* 132 = setgid */
	"sendto",			/* 133 = sendto */
	"shutdown",			/* 134 = shutdown */
	"socketpair",			/* 135 = socketpair */
	"mkdir",			/* 136 = mkdir */
	"rmdir",			/* 137 = rmdir */
	"utimes",			/* 138 = utimes */
	"#139 (unimplemented)",		/* 139 = unimplemented */
	"#140 (unimplemented)",		/* 140 = unimplemented */
	"getpeername",			/* 141 = getpeername */
	"#142 (unimplemented)",		/* 142 = unimplemented */
	"#143 (unimplemented)",		/* 143 = unimplemented */
	"getrlimit",			/* 144 = getrlimit */
	"setrlimit",			/* 145 = setrlimit */
	"#146 (unimplemented)",		/* 146 = unimplemented */
	"setsid",			/* 147 = setsid */
	"#148 (unimplemented quotactl)",		/* 148 = unimplemented quotactl */
	"#149 (unimplemented)",		/* 149 = unimplemented */
	"getsockname",			/* 150 = getsockname */
	"#151 (unimplemented)",		/* 151 = unimplemented */
	"#152 (unimplemented)",		/* 152 = unimplemented */
	"#153 (unimplemented)",		/* 153 = unimplemented */
	"#154 (unimplemented)",		/* 154 = unimplemented */
	"#155 (unimplemented)",		/* 155 = unimplemented */
	"sigaction",			/* 156 = sigaction */
	"#157 (unimplemented)",		/* 157 = unimplemented */
	"#158 (unimplemented)",		/* 158 = unimplemented */
	"getdirentries",			/* 159 = getdirentries */
	"statfs",			/* 160 = statfs */
	"fstatfs",			/* 161 = fstatfs */
	"#162 (unimplemented)",		/* 162 = unimplemented */
	"#163 (unimplemented)",		/* 163 = unimplemented */
	"#164 (unimplemented)",		/* 164 = unimplemented */
	"getdomainname",			/* 165 = getdomainname */
	"setdomainname",			/* 166 = setdomainname */
	"#167 (unimplemented)",		/* 167 = unimplemented */
	"#168 (unimplemented)",		/* 168 = unimplemented */
	"#169 (unimplemented)",		/* 169 = unimplemented */
	"#170 (unimplemented)",		/* 170 = unimplemented */
	"#171 (unimplemented)",		/* 171 = unimplemented */
	"#172 (unimplemented)",		/* 172 = unimplemented */
	"#173 (unimplemented)",		/* 173 = unimplemented */
	"#174 (unimplemented)",		/* 174 = unimplemented */
	"#175 (unimplemented)",		/* 175 = unimplemented */
	"#176 (unimplemented)",		/* 176 = unimplemented */
	"#177 (unimplemented)",		/* 177 = unimplemented */
	"#178 (unimplemented)",		/* 178 = unimplemented */
	"#179 (unimplemented)",		/* 179 = unimplemented */
	"#180 (unimplemented)",		/* 180 = unimplemented */
	"#181 (unimplemented)",		/* 181 = unimplemented */
	"#182 (unimplemented)",		/* 182 = unimplemented */
	"#183 (unimplemented)",		/* 183 = unimplemented */
	"#184 (unimplemented)",		/* 184 = unimplemented */
	"#185 (unimplemented)",		/* 185 = unimplemented */
	"#186 (unimplemented)",		/* 186 = unimplemented */
	"#187 (unimplemented)",		/* 187 = unimplemented */
	"#188 (unimplemented)",		/* 188 = unimplemented */
	"#189 (unimplemented)",		/* 189 = unimplemented */
	"#190 (unimplemented)",		/* 190 = unimplemented */
	"ugetrlimit",			/* 191 = ugetrlimit */
	"#192 (unimplemented mmap2)",		/* 192 = unimplemented mmap2 */
	"#193 (unimplemented)",		/* 193 = unimplemented */
	"#194 (unimplemented)",		/* 194 = unimplemented */
	"#195 (unimplemented)",		/* 195 = unimplemented */
	"#196 (unimplemented)",		/* 196 = unimplemented */
	"#197 (unimplemented)",		/* 197 = unimplemented */
	"#198 (unimplemented)",		/* 198 = unimplemented */
	"#199 (unimplemented osf1_sys_swapon)",		/* 199 = unimplemented osf1_sys_swapon */
#ifdef SYSVMSG
	"msgctl",			/* 200 = msgctl */
	"msgget",			/* 201 = msgget */
	"msgrcv",			/* 202 = msgrcv */
	"msgsnd",			/* 203 = msgsnd */
#else
	"#200 (unimplemented msgctl)",		/* 200 = unimplemented msgctl */
	"#201 (unimplemented msgget)",		/* 201 = unimplemented msgget */
	"#202 (unimplemented msgrcv)",		/* 202 = unimplemented msgrcv */
	"#203 (unimplemented msgsnd)",		/* 203 = unimplemented msgsnd */
#endif
#ifdef SYSVSEM
	"semctl",			/* 204 = semctl */
	"semget",			/* 205 = semget */
	"semop",			/* 206 = semop */
#else
	"#204 (unimplemented semctl)",		/* 204 = unimplemented semctl */
	"#205 (unimplemented semget)",		/* 205 = unimplemented semget */
	"#206 (unimplemented semop)",		/* 206 = unimplemented semop */
#endif
	"olduname",			/* 207 = olduname */
	"__posix_lchown",			/* 208 = __posix_lchown */
#ifdef SYSVSHM
	"shmat",			/* 209 = shmat */
	"shmctl",			/* 210 = shmctl */
	"shmdt",			/* 211 = shmdt */
	"shmget",			/* 212 = shmget */
#else
	"#209 (unimplemented shmat)",		/* 209 = unimplemented shmat */
	"#210 (unimplemented shmctl)",		/* 210 = unimplemented shmctl */
	"#211 (unimplemented shmdt)",		/* 211 = unimplemented shmdt */
	"#212 (unimplemented shmget)",		/* 212 = unimplemented shmget */
#endif
	"#213 (unimplemented)",		/* 213 = unimplemented */
	"#214 (unimplemented)",		/* 214 = unimplemented */
	"#215 (unimplemented)",		/* 215 = unimplemented */
	"#216 (unimplemented)",		/* 216 = unimplemented */
	"msync",			/* 217 = msync */
	"#218 (unimplemented)",		/* 218 = unimplemented */
	"#219 (unimplemented)",		/* 219 = unimplemented */
	"#220 (unimplemented)",		/* 220 = unimplemented */
	"#221 (unimplemented)",		/* 221 = unimplemented */
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
	"getpgid",			/* 233 = getpgid */
	"getsid",			/* 234 = getsid */
	"#235 (unimplemented sigaltstack)",		/* 235 = unimplemented sigaltstack */
	"#236 (unimplemented)",		/* 236 = unimplemented */
	"#237 (unimplemented)",		/* 237 = unimplemented */
	"#238 (unimplemented)",		/* 238 = unimplemented */
	"#239 (unimplemented)",		/* 239 = unimplemented */
	"#240 (unimplemented)",		/* 240 = unimplemented */
	"sysinfo",			/* 241 = sysinfo */
	"#242 (unimplemented)",		/* 242 = unimplemented */
	"#243 (unimplemented)",		/* 243 = unimplemented */
	"#244 (unimplemented osf1_sys_proplist_syscall)",		/* 244 = unimplemented osf1_sys_proplist_syscall */
	"#245 (unimplemented)",		/* 245 = unimplemented */
	"#246 (unimplemented)",		/* 246 = unimplemented */
	"#247 (unimplemented)",		/* 247 = unimplemented */
	"#248 (unimplemented)",		/* 248 = unimplemented */
	"#249 (unimplemented)",		/* 249 = unimplemented */
	"#250 (unimplemented)",		/* 250 = unimplemented */
	"usleep_thread",			/* 251 = usleep_thread */
	"#252 (unimplemented)",		/* 252 = unimplemented */
	"#253 (unimplemented)",		/* 253 = unimplemented */
	"#254 (unimplemented)",		/* 254 = unimplemented */
	"#255 (unimplemented sysfs)",		/* 255 = unimplemented sysfs */
	"getsysinfo",			/* 256 = getsysinfo */
	"setsysinfo",			/* 257 = setsysinfo */
	"#258 (unimplemented)",		/* 258 = unimplemented */
	"#259 (unimplemented)",		/* 259 = unimplemented */
	"#260 (unimplemented)",		/* 260 = unimplemented */
	"fdatasync",			/* 261 = fdatasync */
	"#262 (unimplemented)",		/* 262 = unimplemented */
	"#263 (unimplemented)",		/* 263 = unimplemented */
	"#264 (unimplemented)",		/* 264 = unimplemented */
	"#265 (unimplemented)",		/* 265 = unimplemented */
	"#266 (unimplemented)",		/* 266 = unimplemented */
	"#267 (unimplemented)",		/* 267 = unimplemented */
	"#268 (unimplemented)",		/* 268 = unimplemented */
	"#269 (unimplemented)",		/* 269 = unimplemented */
	"#270 (unimplemented)",		/* 270 = unimplemented */
	"#271 (unimplemented)",		/* 271 = unimplemented */
	"#272 (unimplemented)",		/* 272 = unimplemented */
	"#273 (unimplemented)",		/* 273 = unimplemented */
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
	"#300 (unimplemented bdflush)",		/* 300 = unimplemented bdflush */
	"#301 (unimplemented sethae)",		/* 301 = unimplemented sethae */
	"#302 (unimplemented mount)",		/* 302 = unimplemented mount */
	"#303 (unimplemented old_adjtimex)",		/* 303 = unimplemented old_adjtimex */
	"swapoff",			/* 304 = swapoff */
	"getdents",			/* 305 = getdents */
	"#306 (unimplemented alpha_create_module)",		/* 306 = unimplemented alpha_create_module */
	"#307 (unimplemented init_module)",		/* 307 = unimplemented init_module */
	"#308 (unimplemented delete_module)",		/* 308 = unimplemented delete_module */
	"#309 (unimplemented get_kernel_syms)",		/* 309 = unimplemented get_kernel_syms */
	"#310 (unimplemented syslog)",		/* 310 = unimplemented syslog */
	"reboot",			/* 311 = reboot */
	"clone",			/* 312 = clone */
#ifdef EXEC_AOUT
	"uselib",			/* 313 = uselib */
#else
	"#313 (unimplemented sys_uselib)",		/* 313 = unimplemented sys_uselib */
#endif
	"mlock",			/* 314 = mlock */
	"munlock",			/* 315 = munlock */
	"mlockall",			/* 316 = mlockall */
	"munlockall",			/* 317 = munlockall */
	"sysinfo",			/* 318 = sysinfo */
	"__sysctl",			/* 319 = __sysctl */
	"#320 (unimplemented idle)",		/* 320 = unimplemented idle */
	"#321 (unimplemented oldumount)",		/* 321 = unimplemented oldumount */
	"swapon",			/* 322 = swapon */
	"times",			/* 323 = times */
	"personality",			/* 324 = personality */
	"setfsuid",			/* 325 = setfsuid */
	"#326 (unimplemented setfsgid)",		/* 326 = unimplemented setfsgid */
	"#327 (unimplemented ustat)",		/* 327 = unimplemented ustat */
	"statfs",			/* 328 = statfs */
	"fstatfs",			/* 329 = fstatfs */
	"sched_setparam",			/* 330 = sched_setparam */
	"sched_getparam",			/* 331 = sched_getparam */
	"sched_setscheduler",			/* 332 = sched_setscheduler */
	"sched_getscheduler",			/* 333 = sched_getscheduler */
	"sched_yield",			/* 334 = sched_yield */
	"sched_get_priority_max",			/* 335 = sched_get_priority_max */
	"sched_get_priority_min",			/* 336 = sched_get_priority_min */
	"#337 (unimplemented sched_rr_get_interval)",		/* 337 = unimplemented sched_rr_get_interval */
	"#338 (unimplemented afs_syscall)",		/* 338 = unimplemented afs_syscall */
	"uname",			/* 339 = uname */
	"nanosleep",			/* 340 = nanosleep */
	"mremap",			/* 341 = mremap */
	"#342 (unimplemented nfsservctl)",		/* 342 = unimplemented nfsservctl */
	"setresuid",			/* 343 = setresuid */
	"getresuid",			/* 344 = getresuid */
	"#345 (unimplemented pciconfig_read)",		/* 345 = unimplemented pciconfig_read */
	"#346 (unimplemented pciconfig_write)",		/* 346 = unimplemented pciconfig_write */
	"#347 (unimplemented query_module)",		/* 347 = unimplemented query_module */
	"#348 (unimplemented prctl)",		/* 348 = unimplemented prctl */
	"pread",			/* 349 = pread */
	"pwrite",			/* 350 = pwrite */
	"rt_sigreturn",			/* 351 = rt_sigreturn */
	"rt_sigaction",			/* 352 = rt_sigaction */
	"rt_sigprocmask",			/* 353 = rt_sigprocmask */
	"rt_sigpending",			/* 354 = rt_sigpending */
	"#355 (unimplemented rt_sigtimedwait)",		/* 355 = unimplemented rt_sigtimedwait */
	"rt_queueinfo",			/* 356 = rt_queueinfo */
	"rt_sigsuspend",			/* 357 = rt_sigsuspend */
	"select",			/* 358 = select */
	"gettimeofday",			/* 359 = gettimeofday */
	"settimeofday",			/* 360 = settimeofday */
	"getitimer",			/* 361 = getitimer */
	"setitimer",			/* 362 = setitimer */
	"utimes",			/* 363 = utimes */
	"getrusage",			/* 364 = getrusage */
	"wait4",			/* 365 = wait4 */
	"#366 (unimplemented adjtimex)",		/* 366 = unimplemented adjtimex */
	"__getcwd",			/* 367 = __getcwd */
	"#368 (unimplemented capget)",		/* 368 = unimplemented capget */
	"#369 (unimplemented capset)",		/* 369 = unimplemented capset */
	"#370 (unimplemented sendfile)",		/* 370 = unimplemented sendfile */
	"#371 (unimplemented setresgid)",		/* 371 = unimplemented setresgid */
	"#372 (unimplemented getresgid)",		/* 372 = unimplemented getresgid */
	"#373 (unimplemented sys_dipc)",		/* 373 = unimplemented sys_dipc */
	"#374 (unimplemented pivot_root)",		/* 374 = unimplemented pivot_root */
	"mincore",			/* 375 = mincore */
	"#376 (unimplemented pciconfig_iobase)",		/* 376 = unimplemented pciconfig_iobase */
	"getdents64",			/* 377 = getdents64 */
	"#378 (unimplemented gettid)",		/* 378 = unimplemented gettid */
	"#379 (unimplemented readahead)",		/* 379 = unimplemented readahead */
	"#380 (unimplemented / * unused * /)",		/* 380 = unimplemented / * unused * / */
	"#381 (unimplemented tkill)",		/* 381 = unimplemented tkill */
	"#382 (unimplemented setxattr)",		/* 382 = unimplemented setxattr */
	"#383 (unimplemented lsetxattr)",		/* 383 = unimplemented lsetxattr */
	"#384 (unimplemented fsetxattr)",		/* 384 = unimplemented fsetxattr */
	"#385 (unimplemented getxattr)",		/* 385 = unimplemented getxattr */
	"#386 (unimplemented lgetxattr)",		/* 386 = unimplemented lgetxattr */
	"#387 (unimplemented fgetxattr)",		/* 387 = unimplemented fgetxattr */
	"#388 (unimplemented listxattr)",		/* 388 = unimplemented listxattr */
	"#389 (unimplemented llistxattr)",		/* 389 = unimplemented llistxattr */
	"#390 (unimplemented flistxattr)",		/* 390 = unimplemented flistxattr */
	"#391 (unimplemented removexattr)",		/* 391 = unimplemented removexattr */
	"#392 (unimplemented lremovexattr)",		/* 392 = unimplemented lremovexattr */
	"#393 (unimplemented fremovexattr)",		/* 393 = unimplemented fremovexattr */
	"#394 (unimplemented futex)",		/* 394 = unimplemented futex */
	"#395 (unimplemented sched_setaffinity)",		/* 395 = unimplemented sched_setaffinity */
	"#396 (unimplemented sched_getaffinity)",		/* 396 = unimplemented sched_getaffinity */
	"#397 (unimplemented tuxcall)",		/* 397 = unimplemented tuxcall */
	"#398 (unimplemented io_setup)",		/* 398 = unimplemented io_setup */
	"#399 (unimplemented io_destroy)",		/* 399 = unimplemented io_destroy */
	"#400 (unimplemented io_getevents)",		/* 400 = unimplemented io_getevents */
	"#401 (unimplemented io_submit)",		/* 401 = unimplemented io_submit */
	"#402 (unimplemented io_cancel)",		/* 402 = unimplemented io_cancel */
	"#403 (unimplemented / * unused * /)",		/* 403 = unimplemented / * unused * / */
	"#404 (unimplemented / * unused * /)",		/* 404 = unimplemented / * unused * / */
	"exit_group",			/* 405 = exit_group */
	"#406 (unimplemented lookup_dcookie)",		/* 406 = unimplemented lookup_dcookie */
	"#407 (unimplemented sys_epoll_create)",		/* 407 = unimplemented sys_epoll_create */
	"#408 (unimplemented sys_epoll_ctl)",		/* 408 = unimplemented sys_epoll_ctl */
	"#409 (unimplemented sys_epoll_wait)",		/* 409 = unimplemented sys_epoll_wait */
	"#410 (unimplemented remap_file_pages)",		/* 410 = unimplemented remap_file_pages */
	"#411 (unimplemented set_tid_address)",		/* 411 = unimplemented set_tid_address */
	"#412 (unimplemented restart_syscall)",		/* 412 = unimplemented restart_syscall */
	"#413 (unimplemented fadvise64)",		/* 413 = unimplemented fadvise64 */
	"#414 (unimplemented timer_create)",		/* 414 = unimplemented timer_create */
	"#415 (unimplemented timer_settime)",		/* 415 = unimplemented timer_settime */
	"#416 (unimplemented timer_gettime)",		/* 416 = unimplemented timer_gettime */
	"#417 (unimplemented timer_getoverrun)",		/* 417 = unimplemented timer_getoverrun */
	"#418 (unimplemented timer_delete)",		/* 418 = unimplemented timer_delete */
	"#419 (unimplemented clock_settime)",		/* 419 = unimplemented clock_settime */
	"#420 (unimplemented clock_gettime)",		/* 420 = unimplemented clock_gettime */
	"#421 (unimplemented clock_getres)",		/* 421 = unimplemented clock_getres */
	"#422 (unimplemented clock_nanosleep)",		/* 422 = unimplemented clock_nanosleep */
	"#423 (unimplemented semtimedop)",		/* 423 = unimplemented semtimedop */
	"#424 (unimplemented tgkill)",		/* 424 = unimplemented tgkill */
	"#425 (unimplemented stat64)",		/* 425 = unimplemented stat64 */
	"#426 (unimplemented lstat64)",		/* 426 = unimplemented lstat64 */
	"#427 (unimplemented fstat64)",		/* 427 = unimplemented fstat64 */
	"#428 (unimplemented vserver)",		/* 428 = unimplemented vserver */
	"#429 (unimplemented mbind)",		/* 429 = unimplemented mbind */
	"#430 (unimplemented get_mempolicy)",		/* 430 = unimplemented get_mempolicy */
	"#431 (unimplemented set_mempolicy)",		/* 431 = unimplemented set_mempolicy */
	"#432 (unimplemented mq_open)",		/* 432 = unimplemented mq_open */
	"#433 (unimplemented mq_unlink)",		/* 433 = unimplemented mq_unlink */
	"#434 (unimplemented mq_timedsend)",		/* 434 = unimplemented mq_timedsend */
	"#435 (unimplemented mq_timedreceive)",		/* 435 = unimplemented mq_timedreceive */
	"#436 (unimplemented mq_notify)",		/* 436 = unimplemented mq_notify */
	"#437 (unimplemented mq_getsetattr)",		/* 437 = unimplemented mq_getsetattr */
};
