/* $NetBSD: linux_syscalls.c,v 1.73 2009/01/15 23:50:35 njoly Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.67 2009/01/15 23:48:50 njoly Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.73 2009/01/15 23:50:35 njoly Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
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
#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_msg.h>
#include <compat/linux/common/linux_sem.h>
#include <compat/linux/common/linux_shm.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"#5 (unimplemented)",
	/*   6 */	"close",
	/*   7 */	"wait4",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"#11 (unimplemented)",
	/*  12 */	"chdir",
	/*  13 */	"fchdir",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"__posix_chown",
	/*  17 */	"brk",
	/*  18 */	"#18 (unimplemented)",
	/*  19 */	"lseek",
	/*  20 */	"getpid_with_ppid",
	/*  21 */	"mount",
	/*  22 */	"#22 (unimplemented umount)",
	/*  23 */	"setuid",
	/*  24 */	"getuid_with_euid",
	/*  25 */	"#25 (unimplemented)",
	/*  26 */	"ptrace",
	/*  27 */	"#27 (unimplemented)",
	/*  28 */	"#28 (unimplemented)",
	/*  29 */	"#29 (unimplemented)",
	/*  30 */	"#30 (unimplemented)",
	/*  31 */	"#31 (unimplemented)",
	/*  32 */	"#32 (unimplemented)",
	/*  33 */	"access",
	/*  34 */	"#34 (unimplemented)",
	/*  35 */	"#35 (unimplemented)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"#38 (unimplemented)",
	/*  39 */	"setpgid",
	/*  40 */	"#40 (unimplemented)",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"set_program_attributes",
	/*  44 */	"#44 (unimplemented)",
	/*  45 */	"open",
	/*  46 */	"#46 (unimplemented)",
	/*  47 */	"getgid_with_egid",
	/*  48 */	"sigprocmask",
	/*  49 */	"#49 (unimplemented)",
	/*  50 */	"#50 (unimplemented)",
	/*  51 */	"acct",
	/*  52 */	"sigpending",
	/*  53 */	"#53 (unimplemented)",
	/*  54 */	"ioctl",
	/*  55 */	"#55 (unimplemented)",
	/*  56 */	"#56 (unimplemented)",
	/*  57 */	"symlink",
	/*  58 */	"readlink",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"#62 (unimplemented)",
	/*  63 */	"getpgrp",
	/*  64 */	"getpagesize",
	/*  65 */	"#65 (unimplemented)",
	/*  66 */	"__vfork14",
	/*  67 */	"stat",
	/*  68 */	"lstat",
	/*  69 */	"#69 (unimplemented)",
	/*  70 */	"#70 (unimplemented)",
	/*  71 */	"mmap",
	/*  72 */	"#72 (unimplemented)",
	/*  73 */	"munmap",
	/*  74 */	"mprotect",
	/*  75 */	"madvise",
	/*  76 */	"#76 (unimplemented vhangup)",
	/*  77 */	"#77 (unimplemented)",
	/*  78 */	"#78 (unimplemented)",
	/*  79 */	"getgroups",
	/*  80 */	"setgroups",
	/*  81 */	"#81 (unimplemented)",
	/*  82 */	"#82 (unimplemented setpgrp)",
	/*  83 */	"setitimer",
	/*  84 */	"#84 (unimplemented)",
	/*  85 */	"#85 (unimplemented)",
	/*  86 */	"#86 (unimplemented osf1_sys_getitimer)",
	/*  87 */	"gethostname",
	/*  88 */	"sethostname",
	/*  89 */	"#89 (unimplemented getdtablesize)",
	/*  90 */	"dup2",
	/*  91 */	"fstat",
	/*  92 */	"fcntl",
	/*  93 */	"select",
	/*  94 */	"poll",
	/*  95 */	"fsync",
	/*  96 */	"setpriority",
	/*  97 */	"socket",
	/*  98 */	"connect",
	/*  99 */	"accept",
	/* 100 */	"getpriority",
	/* 101 */	"send",
	/* 102 */	"recv",
	/* 103 */	"sigreturn",
	/* 104 */	"bind",
	/* 105 */	"setsockopt",
	/* 106 */	"listen",
	/* 107 */	"#107 (unimplemented)",
	/* 108 */	"#108 (unimplemented)",
	/* 109 */	"#109 (unimplemented)",
	/* 110 */	"#110 (unimplemented)",
	/* 111 */	"sigsuspend",
	/* 112 */	"sigstack",
	/* 113 */	"recvmsg",
	/* 114 */	"sendmsg",
	/* 115 */	"#115 (unimplemented)",
	/* 116 */	"gettimeofday",
	/* 117 */	"getrusage",
	/* 118 */	"getsockopt",
	/* 119 */	"#119 (unimplemented)",
	/* 120 */	"readv",
	/* 121 */	"writev",
	/* 122 */	"settimeofday",
	/* 123 */	"__posix_fchown",
	/* 124 */	"fchmod",
	/* 125 */	"recvfrom",
	/* 126 */	"setreuid",
	/* 127 */	"setregid",
	/* 128 */	"__posix_rename",
	/* 129 */	"truncate",
	/* 130 */	"ftruncate",
	/* 131 */	"flock",
	/* 132 */	"setgid",
	/* 133 */	"sendto",
	/* 134 */	"shutdown",
	/* 135 */	"socketpair",
	/* 136 */	"mkdir",
	/* 137 */	"rmdir",
	/* 138 */	"utimes",
	/* 139 */	"#139 (unimplemented)",
	/* 140 */	"#140 (unimplemented)",
	/* 141 */	"getpeername",
	/* 142 */	"#142 (unimplemented)",
	/* 143 */	"#143 (unimplemented)",
	/* 144 */	"getrlimit",
	/* 145 */	"setrlimit",
	/* 146 */	"#146 (unimplemented)",
	/* 147 */	"setsid",
	/* 148 */	"#148 (unimplemented quotactl)",
	/* 149 */	"#149 (unimplemented)",
	/* 150 */	"getsockname",
	/* 151 */	"#151 (unimplemented)",
	/* 152 */	"#152 (unimplemented)",
	/* 153 */	"#153 (unimplemented)",
	/* 154 */	"#154 (unimplemented)",
	/* 155 */	"#155 (unimplemented)",
	/* 156 */	"sigaction",
	/* 157 */	"#157 (unimplemented)",
	/* 158 */	"#158 (unimplemented)",
	/* 159 */	"getdirentries",
	/* 160 */	"statfs",
	/* 161 */	"fstatfs",
	/* 162 */	"#162 (unimplemented)",
	/* 163 */	"#163 (unimplemented)",
	/* 164 */	"#164 (unimplemented)",
	/* 165 */	"getdomainname",
	/* 166 */	"setdomainname",
	/* 167 */	"#167 (unimplemented)",
	/* 168 */	"#168 (unimplemented)",
	/* 169 */	"#169 (unimplemented)",
	/* 170 */	"#170 (unimplemented)",
	/* 171 */	"#171 (unimplemented)",
	/* 172 */	"#172 (unimplemented)",
	/* 173 */	"#173 (unimplemented)",
	/* 174 */	"#174 (unimplemented)",
	/* 175 */	"#175 (unimplemented)",
	/* 176 */	"#176 (unimplemented)",
	/* 177 */	"#177 (unimplemented)",
	/* 178 */	"#178 (unimplemented)",
	/* 179 */	"#179 (unimplemented)",
	/* 180 */	"#180 (unimplemented)",
	/* 181 */	"#181 (unimplemented)",
	/* 182 */	"#182 (unimplemented)",
	/* 183 */	"#183 (unimplemented)",
	/* 184 */	"#184 (unimplemented)",
	/* 185 */	"#185 (unimplemented)",
	/* 186 */	"#186 (unimplemented)",
	/* 187 */	"#187 (unimplemented)",
	/* 188 */	"#188 (unimplemented)",
	/* 189 */	"#189 (unimplemented)",
	/* 190 */	"#190 (unimplemented)",
	/* 191 */	"ugetrlimit",
	/* 192 */	"#192 (unimplemented mmap2)",
	/* 193 */	"#193 (unimplemented)",
	/* 194 */	"#194 (unimplemented)",
	/* 195 */	"#195 (unimplemented)",
	/* 196 */	"#196 (unimplemented)",
	/* 197 */	"#197 (unimplemented)",
	/* 198 */	"#198 (unimplemented)",
	/* 199 */	"#199 (unimplemented osf1_sys_swapon)",
#ifdef SYSVMSG
	/* 200 */	"msgctl",
	/* 201 */	"msgget",
	/* 202 */	"msgrcv",
	/* 203 */	"msgsnd",
#else
	/* 200 */	"#200 (unimplemented msgctl)",
	/* 201 */	"#201 (unimplemented msgget)",
	/* 202 */	"#202 (unimplemented msgrcv)",
	/* 203 */	"#203 (unimplemented msgsnd)",
#endif
#ifdef SYSVSEM
	/* 204 */	"semctl",
	/* 205 */	"semget",
	/* 206 */	"semop",
#else
	/* 204 */	"#204 (unimplemented semctl)",
	/* 205 */	"#205 (unimplemented semget)",
	/* 206 */	"#206 (unimplemented semop)",
#endif
	/* 207 */	"olduname",
	/* 208 */	"__posix_lchown",
#ifdef SYSVSHM
	/* 209 */	"shmat",
	/* 210 */	"shmctl",
	/* 211 */	"shmdt",
	/* 212 */	"shmget",
#else
	/* 209 */	"#209 (unimplemented shmat)",
	/* 210 */	"#210 (unimplemented shmctl)",
	/* 211 */	"#211 (unimplemented shmdt)",
	/* 212 */	"#212 (unimplemented shmget)",
#endif
	/* 213 */	"#213 (unimplemented)",
	/* 214 */	"#214 (unimplemented)",
	/* 215 */	"#215 (unimplemented)",
	/* 216 */	"#216 (unimplemented)",
	/* 217 */	"__msync13",
	/* 218 */	"#218 (unimplemented)",
	/* 219 */	"#219 (unimplemented)",
	/* 220 */	"#220 (unimplemented)",
	/* 221 */	"#221 (unimplemented)",
	/* 222 */	"#222 (unimplemented)",
	/* 223 */	"#223 (unimplemented)",
	/* 224 */	"#224 (unimplemented)",
	/* 225 */	"#225 (unimplemented)",
	/* 226 */	"#226 (unimplemented)",
	/* 227 */	"#227 (unimplemented)",
	/* 228 */	"#228 (unimplemented)",
	/* 229 */	"#229 (unimplemented)",
	/* 230 */	"#230 (unimplemented)",
	/* 231 */	"#231 (unimplemented)",
	/* 232 */	"#232 (unimplemented)",
	/* 233 */	"getpgid",
	/* 234 */	"getsid",
	/* 235 */	"#235 (unimplemented sigaltstack)",
	/* 236 */	"#236 (unimplemented)",
	/* 237 */	"#237 (unimplemented)",
	/* 238 */	"#238 (unimplemented)",
	/* 239 */	"#239 (unimplemented)",
	/* 240 */	"#240 (unimplemented)",
	/* 241 */	"sysinfo",
	/* 242 */	"#242 (unimplemented)",
	/* 243 */	"#243 (unimplemented)",
	/* 244 */	"#244 (unimplemented osf1_sys_proplist_syscall)",
	/* 245 */	"#245 (unimplemented)",
	/* 246 */	"#246 (unimplemented)",
	/* 247 */	"#247 (unimplemented)",
	/* 248 */	"#248 (unimplemented)",
	/* 249 */	"#249 (unimplemented)",
	/* 250 */	"#250 (unimplemented)",
	/* 251 */	"usleep_thread",
	/* 252 */	"#252 (unimplemented)",
	/* 253 */	"#253 (unimplemented)",
	/* 254 */	"#254 (unimplemented)",
	/* 255 */	"#255 (unimplemented sysfs)",
	/* 256 */	"getsysinfo",
	/* 257 */	"setsysinfo",
	/* 258 */	"#258 (unimplemented)",
	/* 259 */	"#259 (unimplemented)",
	/* 260 */	"#260 (unimplemented)",
	/* 261 */	"fdatasync",
	/* 262 */	"#262 (unimplemented)",
	/* 263 */	"#263 (unimplemented)",
	/* 264 */	"#264 (unimplemented)",
	/* 265 */	"#265 (unimplemented)",
	/* 266 */	"#266 (unimplemented)",
	/* 267 */	"#267 (unimplemented)",
	/* 268 */	"#268 (unimplemented)",
	/* 269 */	"#269 (unimplemented)",
	/* 270 */	"#270 (unimplemented)",
	/* 271 */	"#271 (unimplemented)",
	/* 272 */	"#272 (unimplemented)",
	/* 273 */	"#273 (unimplemented)",
	/* 274 */	"#274 (unimplemented)",
	/* 275 */	"#275 (unimplemented)",
	/* 276 */	"#276 (unimplemented)",
	/* 277 */	"#277 (unimplemented)",
	/* 278 */	"#278 (unimplemented)",
	/* 279 */	"#279 (unimplemented)",
	/* 280 */	"#280 (unimplemented)",
	/* 281 */	"#281 (unimplemented)",
	/* 282 */	"#282 (unimplemented)",
	/* 283 */	"#283 (unimplemented)",
	/* 284 */	"#284 (unimplemented)",
	/* 285 */	"#285 (unimplemented)",
	/* 286 */	"#286 (unimplemented)",
	/* 287 */	"#287 (unimplemented)",
	/* 288 */	"#288 (unimplemented)",
	/* 289 */	"#289 (unimplemented)",
	/* 290 */	"#290 (unimplemented)",
	/* 291 */	"#291 (unimplemented)",
	/* 292 */	"#292 (unimplemented)",
	/* 293 */	"#293 (unimplemented)",
	/* 294 */	"#294 (unimplemented)",
	/* 295 */	"#295 (unimplemented)",
	/* 296 */	"#296 (unimplemented)",
	/* 297 */	"#297 (unimplemented)",
	/* 298 */	"#298 (unimplemented)",
	/* 299 */	"#299 (unimplemented)",
	/* 300 */	"#300 (unimplemented bdflush)",
	/* 301 */	"#301 (unimplemented sethae)",
	/* 302 */	"#302 (unimplemented mount)",
	/* 303 */	"#303 (unimplemented old_adjtimex)",
	/* 304 */	"swapoff",
	/* 305 */	"getdents",
	/* 306 */	"#306 (unimplemented alpha_create_module)",
	/* 307 */	"#307 (unimplemented init_module)",
	/* 308 */	"#308 (unimplemented delete_module)",
	/* 309 */	"#309 (unimplemented get_kernel_syms)",
	/* 310 */	"#310 (unimplemented syslog)",
	/* 311 */	"reboot",
	/* 312 */	"clone",
#ifdef EXEC_AOUT
	/* 313 */	"uselib",
#else
	/* 313 */	"#313 (unimplemented sys_uselib)",
#endif
	/* 314 */	"mlock",
	/* 315 */	"munlock",
	/* 316 */	"mlockall",
	/* 317 */	"munlockall",
	/* 318 */	"sysinfo",
	/* 319 */	"__sysctl",
	/* 320 */	"#320 (unimplemented idle)",
	/* 321 */	"#321 (unimplemented oldumount)",
	/* 322 */	"swapon",
	/* 323 */	"times",
	/* 324 */	"personality",
	/* 325 */	"setfsuid",
	/* 326 */	"setfsgid",
	/* 327 */	"#327 (unimplemented ustat)",
	/* 328 */	"statfs",
	/* 329 */	"fstatfs",
	/* 330 */	"sched_setparam",
	/* 331 */	"sched_getparam",
	/* 332 */	"sched_setscheduler",
	/* 333 */	"sched_getscheduler",
	/* 334 */	"sched_yield",
	/* 335 */	"sched_get_priority_max",
	/* 336 */	"sched_get_priority_min",
	/* 337 */	"#337 (unimplemented sched_rr_get_interval)",
	/* 338 */	"#338 (unimplemented afs_syscall)",
	/* 339 */	"uname",
	/* 340 */	"nanosleep",
	/* 341 */	"mremap",
	/* 342 */	"#342 (unimplemented nfsservctl)",
	/* 343 */	"setresuid",
	/* 344 */	"getresuid",
	/* 345 */	"#345 (unimplemented pciconfig_read)",
	/* 346 */	"#346 (unimplemented pciconfig_write)",
	/* 347 */	"#347 (unimplemented query_module)",
	/* 348 */	"#348 (unimplemented prctl)",
	/* 349 */	"pread",
	/* 350 */	"pwrite",
	/* 351 */	"rt_sigreturn",
	/* 352 */	"rt_sigaction",
	/* 353 */	"rt_sigprocmask",
	/* 354 */	"rt_sigpending",
	/* 355 */	"#355 (unimplemented rt_sigtimedwait)",
	/* 356 */	"rt_queueinfo",
	/* 357 */	"rt_sigsuspend",
	/* 358 */	"select",
	/* 359 */	"gettimeofday",
	/* 360 */	"settimeofday",
	/* 361 */	"getitimer",
	/* 362 */	"setitimer",
	/* 363 */	"utimes",
	/* 364 */	"getrusage",
	/* 365 */	"wait4",
	/* 366 */	"#366 (unimplemented adjtimex)",
	/* 367 */	"__getcwd",
	/* 368 */	"#368 (unimplemented capget)",
	/* 369 */	"#369 (unimplemented capset)",
	/* 370 */	"#370 (unimplemented sendfile)",
	/* 371 */	"#371 (unimplemented setresgid)",
	/* 372 */	"#372 (unimplemented getresgid)",
	/* 373 */	"#373 (unimplemented sys_dipc)",
	/* 374 */	"#374 (unimplemented pivot_root)",
	/* 375 */	"mincore",
	/* 376 */	"#376 (unimplemented pciconfig_iobase)",
	/* 377 */	"getdents64",
	/* 378 */	"#378 (unimplemented gettid)",
	/* 379 */	"#379 (unimplemented readahead)",
	/* 380 */	"#380 (unimplemented / * unused * /)",
	/* 381 */	"#381 (unimplemented tkill)",
	/* 382 */	"setxattr",
	/* 383 */	"lsetxattr",
	/* 384 */	"fsetxattr",
	/* 385 */	"getxattr",
	/* 386 */	"lgetxattr",
	/* 387 */	"fgetxattr",
	/* 388 */	"listxattr",
	/* 389 */	"llistxattr",
	/* 390 */	"flistxattr",
	/* 391 */	"removexattr",
	/* 392 */	"lremovexattr",
	/* 393 */	"fremovexattr",
	/* 394 */	"#394 (unimplemented futex)",
	/* 395 */	"#395 (unimplemented sched_setaffinity)",
	/* 396 */	"#396 (unimplemented sched_getaffinity)",
	/* 397 */	"#397 (unimplemented tuxcall)",
	/* 398 */	"#398 (unimplemented io_setup)",
	/* 399 */	"#399 (unimplemented io_destroy)",
	/* 400 */	"#400 (unimplemented io_getevents)",
	/* 401 */	"#401 (unimplemented io_submit)",
	/* 402 */	"#402 (unimplemented io_cancel)",
	/* 403 */	"#403 (unimplemented / * unused * /)",
	/* 404 */	"#404 (unimplemented / * unused * /)",
	/* 405 */	"exit_group",
	/* 406 */	"#406 (unimplemented lookup_dcookie)",
	/* 407 */	"#407 (unimplemented sys_epoll_create)",
	/* 408 */	"#408 (unimplemented sys_epoll_ctl)",
	/* 409 */	"#409 (unimplemented sys_epoll_wait)",
	/* 410 */	"#410 (unimplemented remap_file_pages)",
	/* 411 */	"#411 (unimplemented set_tid_address)",
	/* 412 */	"#412 (unimplemented restart_syscall)",
	/* 413 */	"#413 (unimplemented fadvise64)",
	/* 414 */	"#414 (unimplemented timer_create)",
	/* 415 */	"#415 (unimplemented timer_settime)",
	/* 416 */	"#416 (unimplemented timer_gettime)",
	/* 417 */	"#417 (unimplemented timer_getoverrun)",
	/* 418 */	"#418 (unimplemented timer_delete)",
	/* 419 */	"clock_settime",
	/* 420 */	"clock_gettime",
	/* 421 */	"clock_getres",
	/* 422 */	"clock_nanosleep",
	/* 423 */	"#423 (unimplemented semtimedop)",
	/* 424 */	"#424 (unimplemented tgkill)",
	/* 425 */	"#425 (unimplemented stat64)",
	/* 426 */	"#426 (unimplemented lstat64)",
	/* 427 */	"#427 (unimplemented fstat64)",
	/* 428 */	"#428 (unimplemented vserver)",
	/* 429 */	"#429 (unimplemented mbind)",
	/* 430 */	"#430 (unimplemented get_mempolicy)",
	/* 431 */	"#431 (unimplemented set_mempolicy)",
	/* 432 */	"#432 (unimplemented mq_open)",
	/* 433 */	"#433 (unimplemented mq_unlink)",
	/* 434 */	"#434 (unimplemented mq_timedsend)",
	/* 435 */	"#435 (unimplemented mq_timedreceive)",
	/* 436 */	"#436 (unimplemented mq_notify)",
	/* 437 */	"#437 (unimplemented mq_getsetattr)",
};
