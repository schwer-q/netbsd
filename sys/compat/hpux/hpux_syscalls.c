/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.7 1995/05/10 16:45:47 christos Exp 
 */

char *hpux_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"hpux_fork",			/* 2 = hpux_fork */
	"hpux_read",			/* 3 = hpux_read */
	"hpux_write",			/* 4 = hpux_write */
	"hpux_open",			/* 5 = hpux_open */
	"close",			/* 6 = close */
	"hpux_wait",			/* 7 = hpux_wait */
	"hpux_creat",			/* 8 = hpux_creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"hpux_execv",			/* 11 = hpux_execv */
	"chdir",			/* 12 = chdir */
	"compat_hpux_6x_time",	/* 13 = compat_hpux_6x time */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"obreak",			/* 17 = obreak */
	"compat_hpux_6x_stat",	/* 18 = compat_hpux_6x stat */
	"compat_43_lseek",			/* 19 = compat_43_lseek */
	"getpid",			/* 20 = getpid */
	"#21 (unimplemented hpux_mount)",		/* 21 = unimplemented hpux_mount */
	"#22 (unimplemented hpux_umount)",		/* 22 = unimplemented hpux_umount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"compat_hpux_6x_stime",	/* 25 = compat_hpux_6x stime */
	"hpux_ptrace",			/* 26 = hpux_ptrace */
	"compat_hpux_6x_alarm",	/* 27 = compat_hpux_6x alarm */
	"compat_hpux_6x_fstat",	/* 28 = compat_hpux_6x fstat */
	"compat_hpux_6x_pause",	/* 29 = compat_hpux_6x pause */
	"compat_hpux_6x_utime",	/* 30 = compat_hpux_6x utime */
	"compat_hpux_6x_stty",	/* 31 = compat_hpux_6x stty */
	"compat_hpux_6x_gtty",	/* 32 = compat_hpux_6x gtty */
	"access",			/* 33 = access */
	"compat_hpux_6x_nice",	/* 34 = compat_hpux_6x nice */
	"compat_hpux_6x_ftime",	/* 35 = compat_hpux_6x ftime */
	"sync",			/* 36 = sync */
	"hpux_kill",			/* 37 = hpux_kill */
	"hpux_stat",			/* 38 = hpux_stat */
	"compat_hpux_6x_setpgrp",	/* 39 = compat_hpux_6x setpgrp */
	"hpux_lstat",			/* 40 = hpux_lstat */
	"hpux_dup",			/* 41 = hpux_dup */
	"pipe",			/* 42 = pipe */
	"compat_hpux_6x_times",	/* 43 = compat_hpux_6x times */
	"profil",			/* 44 = profil */
	"#45 (unimplemented hpux_ki_syscall)",		/* 45 = unimplemented hpux_ki_syscall */
	"setgid",			/* 46 = setgid */
	"getgid",			/* 47 = getgid */
	"compat_hpux_6x_ssig",	/* 48 = compat_hpux_6x ssig */
	"#49 (unimplemented reserved for USG)",		/* 49 = unimplemented reserved for USG */
	"#50 (unimplemented reserved for USG)",		/* 50 = unimplemented reserved for USG */
	"#51 (unimplemented hpux_acct)",		/* 51 = unimplemented hpux_acct */
	"#52 (unimplemented nosys)",		/* 52 = unimplemented nosys */
	"#53 (unimplemented nosys)",		/* 53 = unimplemented nosys */
	"hpux_ioctl",			/* 54 = hpux_ioctl */
	"#55 (unimplemented hpux_reboot)",		/* 55 = unimplemented hpux_reboot */
	"symlink",			/* 56 = symlink */
	"hpux_utssys",			/* 57 = hpux_utssys */
	"readlink",			/* 58 = readlink */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"hpux_fcntl",			/* 62 = hpux_fcntl */
	"hpux_ulimit",			/* 63 = hpux_ulimit */
	"#64 (unimplemented nosys)",		/* 64 = unimplemented nosys */
	"#65 (unimplemented nosys)",		/* 65 = unimplemented nosys */
	"hpux_vfork",			/* 66 = hpux_vfork */
	"vread",			/* 67 = vread */
	"vwrite",			/* 68 = vwrite */
	"#69 (unimplemented nosys)",		/* 69 = unimplemented nosys */
	"#70 (unimplemented nosys)",		/* 70 = unimplemented nosys */
	"hpux_mmap",			/* 71 = hpux_mmap */
	"#72 (unimplemented nosys)",		/* 72 = unimplemented nosys */
	"munmap",			/* 73 = munmap */
	"mprotect",			/* 74 = mprotect */
	"#75 (unimplemented nosys)",		/* 75 = unimplemented nosys */
	"#76 (unimplemented nosys)",		/* 76 = unimplemented nosys */
	"#77 (unimplemented nosys)",		/* 77 = unimplemented nosys */
	"#78 (unimplemented nosys)",		/* 78 = unimplemented nosys */
	"getgroups",			/* 79 = getgroups */
	"setgroups",			/* 80 = setgroups */
	"hpux_getpgrp2",			/* 81 = hpux_getpgrp2 */
	"hpux_setpgrp2",			/* 82 = hpux_setpgrp2 */
	"setitimer",			/* 83 = setitimer */
	"hpux_wait3",			/* 84 = hpux_wait3 */
	"#85 (unimplemented swapon)",		/* 85 = unimplemented swapon */
	"getitimer",			/* 86 = getitimer */
	"#87 (unimplemented nosys)",		/* 87 = unimplemented nosys */
	"#88 (unimplemented nosys)",		/* 88 = unimplemented nosys */
	"#89 (unimplemented nosys)",		/* 89 = unimplemented nosys */
	"dup2",			/* 90 = dup2 */
	"#91 (unimplemented nosys)",		/* 91 = unimplemented nosys */
	"hpux_fstat",			/* 92 = hpux_fstat */
	"select",			/* 93 = select */
	"#94 (unimplemented nosys)",		/* 94 = unimplemented nosys */
	"fsync",			/* 95 = fsync */
	"#96 (unimplemented nosys)",		/* 96 = unimplemented nosys */
	"#97 (unimplemented nosys)",		/* 97 = unimplemented nosys */
	"#98 (unimplemented nosys)",		/* 98 = unimplemented nosys */
	"#99 (unimplemented nosys)",		/* 99 = unimplemented nosys */
	"#100 (unimplemented nosys)",		/* 100 = unimplemented nosys */
	"#101 (unimplemented nosys)",		/* 101 = unimplemented nosys */
	"#102 (unimplemented nosys)",		/* 102 = unimplemented nosys */
	"sigreturn",			/* 103 = sigreturn */
	"#104 (unimplemented nosys)",		/* 104 = unimplemented nosys */
	"#105 (unimplemented nosys)",		/* 105 = unimplemented nosys */
	"#106 (unimplemented nosys)",		/* 106 = unimplemented nosys */
	"#107 (unimplemented nosys)",		/* 107 = unimplemented nosys */
	"hpux_sigvec",			/* 108 = hpux_sigvec */
	"hpux_sigblock",			/* 109 = hpux_sigblock */
	"hpux_sigsetmask",			/* 110 = hpux_sigsetmask */
	"hpux_sigpause",			/* 111 = hpux_sigpause */
	"compat_43_sigstack",			/* 112 = compat_43_sigstack */
	"#113 (unimplemented nosys)",		/* 113 = unimplemented nosys */
	"#114 (unimplemented nosys)",		/* 114 = unimplemented nosys */
	"#115 (unimplemented nosys)",		/* 115 = unimplemented nosys */
	"gettimeofday",			/* 116 = gettimeofday */
	"#117 (unimplemented nosys)",		/* 117 = unimplemented nosys */
	"#118 (unimplemented nosys)",		/* 118 = unimplemented nosys */
	"#119 (unimplemented hpib_io_stub)",		/* 119 = unimplemented hpib_io_stub */
	"hpux_readv",			/* 120 = hpux_readv */
	"hpux_writev",			/* 121 = hpux_writev */
	"settimeofday",			/* 122 = settimeofday */
	"fchown",			/* 123 = fchown */
	"fchmod",			/* 124 = fchmod */
	"#125 (unimplemented nosys)",		/* 125 = unimplemented nosys */
	"hpux_setresuid",			/* 126 = hpux_setresuid */
	"hpux_setresgid",			/* 127 = hpux_setresgid */
	"rename",			/* 128 = rename */
	"compat_43_truncate",			/* 129 = compat_43_truncate */
	"compat_43_ftruncate",			/* 130 = compat_43_ftruncate */
	"#131 (unimplemented nosys)",		/* 131 = unimplemented nosys */
	"hpux_sysconf",			/* 132 = hpux_sysconf */
	"#133 (unimplemented nosys)",		/* 133 = unimplemented nosys */
	"#134 (unimplemented nosys)",		/* 134 = unimplemented nosys */
	"#135 (unimplemented nosys)",		/* 135 = unimplemented nosys */
	"mkdir",			/* 136 = mkdir */
	"rmdir",			/* 137 = rmdir */
	"#138 (unimplemented nosys)",		/* 138 = unimplemented nosys */
	"#139 (unimplemented nosys)",		/* 139 = unimplemented nosys */
	"#140 (unimplemented nosys)",		/* 140 = unimplemented nosys */
	"#141 (unimplemented nosys)",		/* 141 = unimplemented nosys */
	"#142 (unimplemented nosys)",		/* 142 = unimplemented nosys */
	"#143 (unimplemented nosys)",		/* 143 = unimplemented nosys */
	"hpux_getrlimit",			/* 144 = hpux_getrlimit */
	"hpux_setrlimit",			/* 145 = hpux_setrlimit */
	"#146 (unimplemented nosys)",		/* 146 = unimplemented nosys */
	"#147 (unimplemented nosys)",		/* 147 = unimplemented nosys */
	"#148 (unimplemented nosys)",		/* 148 = unimplemented nosys */
	"#149 (unimplemented nosys)",		/* 149 = unimplemented nosys */
	"#150 (unimplemented nosys)",		/* 150 = unimplemented nosys */
	"#151 (unimplemented hpux_privgrp)",		/* 151 = unimplemented hpux_privgrp */
	"hpux_rtprio",			/* 152 = hpux_rtprio */
	"#153 (unimplemented hpux_plock)",		/* 153 = unimplemented hpux_plock */
	"hpux_netioctl",			/* 154 = hpux_netioctl */
	"hpux_lockf",			/* 155 = hpux_lockf */
#ifdef SYSVSEM
	"semget",			/* 156 = semget */
	"__semctl",			/* 157 = __semctl */
	"semop",			/* 158 = semop */
#else
	"#156 (unimplemented semget)",		/* 156 = unimplemented semget */
	"#157 (unimplemented semctl)",		/* 157 = unimplemented semctl */
	"#158 (unimplemented semop)",		/* 158 = unimplemented semop */
#endif
#ifdef SYSVMSG
	"msgget",			/* 159 = msgget */
	"msgctl",			/* 160 = msgctl */
	"msgsnd",			/* 161 = msgsnd */
	"msgrcv",			/* 162 = msgrcv */
#else
	"#159 (unimplemented msgget)",		/* 159 = unimplemented msgget */
	"#160 (unimplemented msgctl)",		/* 160 = unimplemented msgctl */
	"#161 (unimplemented msgsnd)",		/* 161 = unimplemented msgsnd */
	"#162 (unimplemented msgrcv)",		/* 162 = unimplemented msgrcv */
#endif
#ifdef SYSVSHM
	"shmget",			/* 163 = shmget */
	"hpux_shmctl",			/* 164 = hpux_shmctl */
	"shmat",			/* 165 = shmat */
	"shmdt",			/* 166 = shmdt */
#else
	"#163 (unimplemented shmget)",		/* 163 = unimplemented shmget */
	"#164 (unimplemented shmctl)",		/* 164 = unimplemented shmctl */
	"#165 (unimplemented shmat)",		/* 165 = unimplemented shmat */
	"#166 (unimplemented shmdt)",		/* 166 = unimplemented shmdt */
#endif
	"hpux_advise",			/* 167 = hpux_advise */
	"#168 (unimplemented nsp_init)",		/* 168 = unimplemented nsp_init */
	"#169 (unimplemented cluster)",		/* 169 = unimplemented cluster */
	"#170 (unimplemented mkrnod)",		/* 170 = unimplemented mkrnod */
	"#171 (unimplemented nosys)",		/* 171 = unimplemented nosys */
	"#172 (unimplemented unsp_open)",		/* 172 = unimplemented unsp_open */
	"#173 (unimplemented nosys)",		/* 173 = unimplemented nosys */
	"hpux_getcontext",			/* 174 = hpux_getcontext */
	"#175 (unimplemented nosys)",		/* 175 = unimplemented nosys */
	"#176 (unimplemented nosys)",		/* 176 = unimplemented nosys */
	"#177 (unimplemented nosys)",		/* 177 = unimplemented nosys */
	"#178 (unimplemented lsync)",		/* 178 = unimplemented lsync */
	"#179 (unimplemented nosys)",		/* 179 = unimplemented nosys */
	"#180 (unimplemented mysite)",		/* 180 = unimplemented mysite */
	"#181 (unimplemented sitels)",		/* 181 = unimplemented sitels */
	"#182 (unimplemented nosys)",		/* 182 = unimplemented nosys */
	"#183 (unimplemented nosys)",		/* 183 = unimplemented nosys */
	"#184 (unimplemented dskless_stats)",		/* 184 = unimplemented dskless_stats */
	"#185 (unimplemented nosys)",		/* 185 = unimplemented nosys */
	"#186 (unimplemented setacl)",		/* 186 = unimplemented setacl */
	"#187 (unimplemented fsetacl)",		/* 187 = unimplemented fsetacl */
	"#188 (unimplemented getacl)",		/* 188 = unimplemented getacl */
	"#189 (unimplemented fgetacl)",		/* 189 = unimplemented fgetacl */
	"hpux_getaccess",			/* 190 = hpux_getaccess */
	"#191 (unimplemented getaudid)",		/* 191 = unimplemented getaudid */
	"#192 (unimplemented setaudid)",		/* 192 = unimplemented setaudid */
	"#193 (unimplemented getaudproc)",		/* 193 = unimplemented getaudproc */
	"#194 (unimplemented setaudproc)",		/* 194 = unimplemented setaudproc */
	"#195 (unimplemented getevent)",		/* 195 = unimplemented getevent */
	"#196 (unimplemented setevent)",		/* 196 = unimplemented setevent */
	"#197 (unimplemented audwrite)",		/* 197 = unimplemented audwrite */
	"#198 (unimplemented audswitch)",		/* 198 = unimplemented audswitch */
	"#199 (unimplemented audctl)",		/* 199 = unimplemented audctl */
	"hpux_waitpid",			/* 200 = hpux_waitpid */
	"#201 (unimplemented nosys)",		/* 201 = unimplemented nosys */
	"#202 (unimplemented nosys)",		/* 202 = unimplemented nosys */
	"#203 (unimplemented nosys)",		/* 203 = unimplemented nosys */
	"#204 (unimplemented nosys)",		/* 204 = unimplemented nosys */
	"#205 (unimplemented nosys)",		/* 205 = unimplemented nosys */
	"#206 (unimplemented nosys)",		/* 206 = unimplemented nosys */
	"#207 (unimplemented nosys)",		/* 207 = unimplemented nosys */
	"#208 (unimplemented nosys)",		/* 208 = unimplemented nosys */
	"#209 (unimplemented nosys)",		/* 209 = unimplemented nosys */
	"#210 (unimplemented nosys)",		/* 210 = unimplemented nosys */
	"#211 (unimplemented nosys)",		/* 211 = unimplemented nosys */
	"#212 (unimplemented nosys)",		/* 212 = unimplemented nosys */
	"#213 (unimplemented nosys)",		/* 213 = unimplemented nosys */
	"#214 (unimplemented nosys)",		/* 214 = unimplemented nosys */
	"#215 (unimplemented nosys)",		/* 215 = unimplemented nosys */
	"#216 (unimplemented nosys)",		/* 216 = unimplemented nosys */
	"#217 (unimplemented nosys)",		/* 217 = unimplemented nosys */
	"#218 (unimplemented nosys)",		/* 218 = unimplemented nosys */
	"#219 (unimplemented nosys)",		/* 219 = unimplemented nosys */
	"#220 (unimplemented nosys)",		/* 220 = unimplemented nosys */
	"#221 (unimplemented nosys)",		/* 221 = unimplemented nosys */
	"#222 (unimplemented nosys)",		/* 222 = unimplemented nosys */
	"#223 (unimplemented nosys)",		/* 223 = unimplemented nosys */
	"#224 (unimplemented nosys)",		/* 224 = unimplemented nosys */
	"pathconf",			/* 225 = pathconf */
	"fpathconf",			/* 226 = fpathconf */
	"#227 (unimplemented nosys)",		/* 227 = unimplemented nosys */
	"#228 (unimplemented nosys)",		/* 228 = unimplemented nosys */
	"#229 (unimplemented hpux_async_daemon)",		/* 229 = unimplemented hpux_async_daemon */
	"#230 (unimplemented hpux_nfs_fcntl)",		/* 230 = unimplemented hpux_nfs_fcntl */
	"compat_43_getdirentries",			/* 231 = compat_43_getdirentries */
	"compat_09_getdomainname",			/* 232 = compat_09_getdomainname */
	"#233 (unimplemented hpux_nfs_getfh)",		/* 233 = unimplemented hpux_nfs_getfh */
	"#234 (unimplemented hpux_vfsmount)",		/* 234 = unimplemented hpux_vfsmount */
	"#235 (unimplemented hpux_nfs_svc)",		/* 235 = unimplemented hpux_nfs_svc */
	"compat_09_setdomainname",			/* 236 = compat_09_setdomainname */
	"#237 (unimplemented hpux_statfs)",		/* 237 = unimplemented hpux_statfs */
	"#238 (unimplemented hpux_fstatfs)",		/* 238 = unimplemented hpux_fstatfs */
	"hpux_sigaction",			/* 239 = hpux_sigaction */
	"hpux_sigprocmask",			/* 240 = hpux_sigprocmask */
	"hpux_sigpending",			/* 241 = hpux_sigpending */
	"hpux_sigsuspend",			/* 242 = hpux_sigsuspend */
	"#243 (unimplemented hpux_fsctl)",		/* 243 = unimplemented hpux_fsctl */
	"#244 (unimplemented nosys)",		/* 244 = unimplemented nosys */
	"#245 (unimplemented hpux_pstat)",		/* 245 = unimplemented hpux_pstat */
	"#246 (unimplemented nosys)",		/* 246 = unimplemented nosys */
	"#247 (unimplemented nosys)",		/* 247 = unimplemented nosys */
	"#248 (unimplemented nosys)",		/* 248 = unimplemented nosys */
	"#249 (unimplemented nosys)",		/* 249 = unimplemented nosys */
	"#250 (unimplemented nosys)",		/* 250 = unimplemented nosys */
	"#251 (unimplemented nosys)",		/* 251 = unimplemented nosys */
	"#252 (unimplemented nosys)",		/* 252 = unimplemented nosys */
	"#253 (unimplemented nosys)",		/* 253 = unimplemented nosys */
	"#254 (unimplemented nosys)",		/* 254 = unimplemented nosys */
	"#255 (unimplemented nosys)",		/* 255 = unimplemented nosys */
	"#256 (unimplemented nosys)",		/* 256 = unimplemented nosys */
	"#257 (unimplemented nosys)",		/* 257 = unimplemented nosys */
	"#258 (unimplemented nosys)",		/* 258 = unimplemented nosys */
	"#259 (unimplemented nosys)",		/* 259 = unimplemented nosys */
	"#260 (unimplemented nosys)",		/* 260 = unimplemented nosys */
	"#261 (unimplemented nosys)",		/* 261 = unimplemented nosys */
	"#262 (unimplemented nosys)",		/* 262 = unimplemented nosys */
	"#263 (unimplemented nosys)",		/* 263 = unimplemented nosys */
	"#264 (unimplemented nosys)",		/* 264 = unimplemented nosys */
	"#265 (unimplemented nosys)",		/* 265 = unimplemented nosys */
	"#266 (unimplemented nosys)",		/* 266 = unimplemented nosys */
	"#267 (unimplemented nosys)",		/* 267 = unimplemented nosys */
	"compat_43_getdtablesize",			/* 268 = compat_43_getdtablesize */
	"#269 (unimplemented nosys)",		/* 269 = unimplemented nosys */
	"#270 (unimplemented nosys)",		/* 270 = unimplemented nosys */
	"#271 (unimplemented nosys)",		/* 271 = unimplemented nosys */
	"fchdir",			/* 272 = fchdir */
	"#273 (unimplemented nosys)",		/* 273 = unimplemented nosys */
	"#274 (unimplemented nosys)",		/* 274 = unimplemented nosys */
	"compat_43_accept",			/* 275 = compat_43_accept */
	"bind",			/* 276 = bind */
	"connect",			/* 277 = connect */
	"compat_43_getpeername",			/* 278 = compat_43_getpeername */
	"compat_43_getsockname",			/* 279 = compat_43_getsockname */
	"getsockopt",			/* 280 = getsockopt */
	"listen",			/* 281 = listen */
	"compat_43_recv",			/* 282 = compat_43_recv */
	"compat_43_recvfrom",			/* 283 = compat_43_recvfrom */
	"compat_43_recvmsg",			/* 284 = compat_43_recvmsg */
	"compat_43_send",			/* 285 = compat_43_send */
	"compat_43_sendmsg",			/* 286 = compat_43_sendmsg */
	"sendto",			/* 287 = sendto */
	"hpux_setsockopt2",			/* 288 = hpux_setsockopt2 */
	"shutdown",			/* 289 = shutdown */
	"socket",			/* 290 = socket */
	"socketpair",			/* 291 = socketpair */
	"#292 (unimplemented nosys)",		/* 292 = unimplemented nosys */
	"#293 (unimplemented nosys)",		/* 293 = unimplemented nosys */
	"#294 (unimplemented nosys)",		/* 294 = unimplemented nosys */
	"#295 (unimplemented nosys)",		/* 295 = unimplemented nosys */
	"#296 (unimplemented nosys)",		/* 296 = unimplemented nosys */
	"#297 (unimplemented nosys)",		/* 297 = unimplemented nosys */
	"#298 (unimplemented nosys)",		/* 298 = unimplemented nosys */
	"#299 (unimplemented nosys)",		/* 299 = unimplemented nosys */
	"#300 (unimplemented nosys)",		/* 300 = unimplemented nosys */
	"#301 (unimplemented nosys)",		/* 301 = unimplemented nosys */
	"#302 (unimplemented nosys)",		/* 302 = unimplemented nosys */
	"#303 (unimplemented nosys)",		/* 303 = unimplemented nosys */
	"#304 (unimplemented nosys)",		/* 304 = unimplemented nosys */
	"#305 (unimplemented nosys)",		/* 305 = unimplemented nosys */
	"#306 (unimplemented nosys)",		/* 306 = unimplemented nosys */
	"#307 (unimplemented nosys)",		/* 307 = unimplemented nosys */
	"#308 (unimplemented nosys)",		/* 308 = unimplemented nosys */
	"#309 (unimplemented nosys)",		/* 309 = unimplemented nosys */
	"#310 (unimplemented nosys)",		/* 310 = unimplemented nosys */
	"#311 (unimplemented nosys)",		/* 311 = unimplemented nosys */
#ifdef SYSVSEM
	"nsemctl",			/* 312 = nsemctl */
#else
	"#312 (unimplemented semctl)",		/* 312 = unimplemented semctl */
#endif
#ifdef SYSVMSG
	"nmsgctl",			/* 313 = nmsgctl */
#else
	"#313 (unimplemented msgctl)",		/* 313 = unimplemented msgctl */
#endif
#ifdef SYSVSHM
	"hpux_nshmctl",			/* 314 = hpux_nshmctl */
#else
	"#314 (unimplemented shmctl)",		/* 314 = unimplemented shmctl */
#endif
};
