/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from 	$Id: hpux_sysent.c,v 1.4 1994/01/07 00:43:59 mycroft Exp $
 */

#include <sys/param.h>
#include <sys/systm.h>

int	nosys();

int	nosys();
int	rexit();
int	fork();
int	hpux_read();
int	hpux_write();
int	hpux_open();
int	close();
int	hpux_wait();
int	ocreat();
int	link();
int	unlink();
int	hpux_execv();
int	chdir();
int	mknod();
int	chmod();
int	chown();
int	obreak();
int	lseek();
int	getpid();
int	notimp();
int	notimp();
int	setuid();
int	getuid();
int	hpux_ptrace();
int	saccess();
int	sync();
int	hpux_kill();
int	hpux_stat();
int	hpux_lstat();
int	hpux_dup();
int	pipe();
int	profil();
int	notimp();
int	setgid();
int	getgid();
int	notimp();
int	hpux_ioctl();
int	notimp();
int	symlink();
int	hpux_utssys();
int	readlink();
int	execve();
int	umask();
int	chroot();
int	hpux_fcntl();
int	hpux_ulimit();
int	nosys();
int	nosys();
int	vfork();
int	hpux_read();
int	hpux_write();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	notimp();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	getgroups();
int	setgroups();
int	hpux_getpgrp2();
int	hpux_setpgrp2();
int	setitimer();
int	hpux_wait3();
int	notimp();
int	getitimer();
int	nosys();
int	nosys();
int	nosys();
int	dup2();
int	nosys();
int	hpux_fstat();
int	select();
int	nosys();
int	fsync();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	sigreturn();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	hpux_sigvec();
int	hpux_sigblock();
int	hpux_sigsetmask();
int	hpux_sigpause();
int	sigstack();
int	nosys();
int	nosys();
int	nosys();
int	gettimeofday();
int	nosys();
int	nosys();
int	notimp();
int	hpux_readv();
int	hpux_writev();
int	settimeofday();
int	fchown();
int	fchmod();
int	nosys();
int	hpux_setresuid();
int	hpux_setresgid();
int	rename();
int	truncate();
int	ftruncate();
int	nosys();
int	notimp();
int	nosys();
int	nosys();
int	nosys();
int	mkdir();
int	rmdir();
int	nosys();
int	sigreturn();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	notimp();
int	hpux_rtprio();
int	notimp();
int	hpux_netioctl();
int	hpux_lockf();
#ifdef SYSVSEM
int	semget();
int	semctl();
int	semop();
#else
int	notimp();
int	notimp();
int	notimp();
#endif
#ifdef SYSVMSG
int	msgget();
int	msgctl();
int	msgsnd();
int	msgrcv();
#else
int	notimp();
int	notimp();
int	notimp();
int	notimp();
#endif
#ifdef SYSVSHM
int	shmget();
int	shmctl();
int	shmat();
int	shmdt();
#else
int	notimp();
int	notimp();
int	notimp();
int	notimp();
#endif
int	hpux_advise();
int	notimp();
int	notimp();
int	notimp();
int	nosys();
int	notimp();
int	nosys();
int	hpux_getcontext();
int	nosys();
int	nosys();
int	nosys();
int	notimp();
int	nosys();
int	notimp();
int	notimp();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	hpux_getaccess();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	notimp();
int	hpux_waitpid();
int	nosys();
int	notimp();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	nosys();
int	notimp();
int	notimp();
int	nosys();
int	nosys();
int	notimp();
int	notimp();
int	getdirentries();
int	getdomainname();
int	notimp();
int	notimp();
int	notimp();
int	setdomainname();
int	notimp();
int	notimp();
int	hpux_sigaction();
int	hpux_sigprocmask();
int	hpux_sigpending();
int	hpux_sigsuspend();

#ifdef XXX_UNUSED
#define compat(n, name) n, __CONCAT(o,name)

int	ohpux_time();
int	ohpux_stat();
int	ohpux_stime();
int	ohpux_alarm();
int	ohpux_fstat();
int	ohpux_pause();
int	ohpux_utime();
int	ohpux_stty();
int	ohpux_gtty();
int	ohpux_nice();
int	ohpux_ftime();
int	ohpux_setpgrp();
int	ohpux_times();
int	ohpux_ssig();
#ifdef SYSVSEM
#else
#endif
#ifdef SYSVMSG
#else
#endif
#ifdef SYSVSHM
#else
#endif

#else /* XXX_UNUSED */
#define compat(n, name) 0, nosys
#endif /* XXX_UNUSED */

struct sysent hpux_sysent[] = {
	{ 0, nosys },			/* 0 = nosys */
	{ 1, rexit },			/* 1 = exit */
	{ 0, fork },			/* 2 = fork */
	{ 3, hpux_read },			/* 3 = hpux_read */
	{ 3, hpux_write },			/* 4 = hpux_write */
	{ 3, hpux_open },			/* 5 = hpux_open */
	{ 1, close },			/* 6 = close */
	{ 1, hpux_wait },			/* 7 = hpux_wait */
	{ 2, ocreat },			/* 8 = creat */
	{ 2, link },			/* 9 = link */
	{ 1, unlink },			/* 10 = unlink */
	{ 2, hpux_execv },			/* 11 = execv */
	{ 1, chdir },			/* 12 = chdir */
	{ compat(1,hpux_time) },		/* 13 = old hpux_time */
	{ 3, mknod },			/* 14 = mknod */
	{ 2, chmod },			/* 15 = chmod */
	{ 3, chown },			/* 16 = chown */
	{ 1, obreak },			/* 17 = break */
	{ compat(2,hpux_stat) },		/* 18 = old hpux_stat */
	{ 3, lseek },			/* 19 = lseek */
	{ 0, getpid },			/* 20 = getpid */
	{ 3, notimp },			/* 21 = mount */
	{ 1, notimp },			/* 22 = umount */
	{ 1, setuid },			/* 23 = setuid */
	{ 0, getuid },			/* 24 = getuid */
	{ compat(1,hpux_stime) },		/* 25 = old hpux_stime */
	{ 4, hpux_ptrace },			/* 26 = hpux_ptrace */
	{ compat(1,hpux_alarm) },		/* 27 = old hpux_alarm */
	{ compat(2,hpux_fstat) },		/* 28 = old hpux_fstat */
	{ compat(0,hpux_pause) },		/* 29 = old hpux_pause */
	{ compat(2,hpux_utime) },		/* 30 = old hpux_utime */
	{ compat(2,hpux_stty) },		/* 31 = old hpux_stty */
	{ compat(2,hpux_gtty) },		/* 32 = old hpux_gtty */
	{ 2, saccess },			/* 33 = access */
	{ compat(1,hpux_nice) },		/* 34 = old hpux_nice */
	{ compat(1,hpux_ftime) },		/* 35 = old hpux_ftime */
	{ 0, sync },			/* 36 = sync */
	{ 2, hpux_kill },			/* 37 = hpux_kill */
	{ 2, hpux_stat },			/* 38 = hpux_stat */
	{ compat(1,hpux_setpgrp) },		/* 39 = old hpux_setpgrp */
	{ 2, hpux_lstat },			/* 40 = hpux_lstat */
	{ 1, hpux_dup },			/* 41 = hpux_dup */
	{ 0, pipe },			/* 42 = pipe */
	{ compat(1,hpux_times) },		/* 43 = old hpux_times */
	{ 4, profil },			/* 44 = profil */
	{ 4, notimp },			/* 45 = ki_syscall */
	{ 1, setgid },			/* 46 = setgid */
	{ 0, getgid },			/* 47 = getgid */
	{ compat(2,hpux_ssig) },		/* 48 = old hpux_ssig */
	{ 0, nosys },			/* 49 = reserved */
	{ 0, nosys },			/* 50 = reserved */
	{ 1, notimp },			/* 51 = acct */
	{ 0, nosys },			/* 52 = old set phys addr */
	{ 0, nosys },			/* 53 = old lock in core */
	{ 3, hpux_ioctl },			/* 54 = hpux_ioctl */
	{ 4, notimp },			/* 55 = reboot */
	{ 2, symlink },			/* 56 = symlink */
	{ 3, hpux_utssys },			/* 57 = hpux_utssys */
	{ 3, readlink },			/* 58 = readlink */
	{ 3, execve },			/* 59 = execve */
	{ 1, umask },			/* 60 = umask */
	{ 1, chroot },			/* 61 = chroot */
	{ 3, hpux_fcntl },			/* 62 = hpux_fcntl */
	{ 2, hpux_ulimit },			/* 63 = hpux_ulimit */
	{ 0, nosys },			/* 64 = nosys */
	{ 0, nosys },			/* 65 = nosys */
	{ 0, vfork },			/* 66 = vfork */
	{ 0, hpux_read },			/* 67 = hpux_read */
	{ 0, hpux_write },			/* 68 = hpux_write */
	{ 0, nosys },			/* 69 = nosys */
	{ 0, nosys },			/* 70 = nosys */
	{ 0, nosys },			/* 71 = nosys */
	{ 0, nosys },			/* 72 = nosys */
	{ 0, nosys },			/* 73 = nosys */
	{ 3, notimp },			/* 74 = mprotect */
	{ 0, nosys },			/* 75 = nosys */
	{ 0, nosys },			/* 76 = nosys */
	{ 0, nosys },			/* 77 = nosys */
	{ 0, nosys },			/* 78 = nosys */
	{ 2, getgroups },			/* 79 = getgroups */
	{ 2, setgroups },			/* 80 = setgroups */
	{ 1, hpux_getpgrp2 },			/* 81 = hpux_getpgrp2 */
	{ 2, hpux_setpgrp2 },			/* 82 = hpux_setpgrp2 */
	{ 3, setitimer },			/* 83 = setitimer */
	{ 3, hpux_wait3 },			/* 84 = hpux_wait3 */
	{ 1, notimp },			/* 85 = swapon */
	{ 2, getitimer },			/* 86 = getitimer */
	{ 0, nosys },			/* 87 = nosys */
	{ 0, nosys },			/* 88 = nosys */
	{ 0, nosys },			/* 89 = nosys */
	{ 2, dup2 },			/* 90 = dup2 */
	{ 2, nosys },			/* 91 = nosys */
	{ 2, hpux_fstat },			/* 92 = hpux_fstat */
	{ 5, select },			/* 93 = select */
	{ 0, nosys },			/* 94 = nosys */
	{ 1, fsync },			/* 95 = fsync */
	{ 0, nosys },			/* 96 = nosys */
	{ 3, nosys },			/* 97 = nosys */
	{ 2, nosys },			/* 98 = nosys */
	{ 2, nosys },			/* 99 = nosys */
	{ 0, nosys },			/* 100 = nosys */
	{ 0, nosys },			/* 101 = nosys */
	{ 0, nosys },			/* 102 = nosys */
	{ 1, sigreturn },			/* 103 = sigreturn */
	{ 2, nosys },			/* 104 = nosys */
	{ 0, nosys },			/* 105 = nosys */
	{ 0, nosys },			/* 106 = nosys */
	{ 0, nosys },			/* 107 = nosys */
	{ 3, hpux_sigvec },			/* 108 = hpux_sigvec */
	{ 1, hpux_sigblock },			/* 109 = hpux_sigblock */
	{ 1, hpux_sigsetmask },			/* 110 = hpux_sigsetmask */
	{ 1, hpux_sigpause },			/* 111 = hpux_sigpause */
	{ 2, sigstack },			/* 112 = sigstack */
	{ 0, nosys },			/* 113 = nosys */
	{ 0, nosys },			/* 114 = nosys */
	{ 0, nosys },			/* 115 = nosys */
	{ 2, gettimeofday },			/* 116 = gettimeofday */
	{ 0, nosys },			/* 117 = nosys */
	{ 0, nosys },			/* 118 = nosys */
	{ 3, notimp },			/* 119 = hpib_io_stub */
	{ 3, hpux_readv },			/* 120 = hpux_readv */
	{ 3, hpux_writev },			/* 121 = hpux_writev */
	{ 2, settimeofday },			/* 122 = settimeofday */
	{ 3, fchown },			/* 123 = fchown */
	{ 2, fchmod },			/* 124 = fchmod */
	{ 0, nosys },			/* 125 = nosys */
	{ 3, hpux_setresuid },			/* 126 = hpux_setresuid */
	{ 3, hpux_setresgid },			/* 127 = hpux_setresgid */
	{ 2, rename },			/* 128 = rename */
	{ 2, truncate },			/* 129 = truncate */
	{ 2, ftruncate },			/* 130 = ftruncate */
	{ 0, nosys },			/* 131 = nosys */
	{ 1, notimp },			/* 132 = sysconf */
	{ 0, nosys },			/* 133 = nosys */
	{ 0, nosys },			/* 134 = nosys */
	{ 0, nosys },			/* 135 = nosys */
	{ 2, mkdir },			/* 136 = mkdir */
	{ 1, rmdir },			/* 137 = rmdir */
	{ 0, nosys },			/* 138 = nosys */
	{ 0, sigreturn },			/* 139 = sigreturn */
	{ 0, nosys },			/* 140 = nosys */
	{ 0, nosys },			/* 141 = nosys */
	{ 0, nosys },			/* 142 = nosys */
	{ 0, nosys },			/* 143 = nosys */
	{ 0, nosys },			/* 144 = nosys */
	{ 0, nosys },			/* 145 = nosys */
	{ 0, nosys },			/* 146 = nosys */
	{ 0, nosys },			/* 147 = nosys */
	{ 0, nosys },			/* 148 = nosys */
	{ 0, nosys },			/* 149 = nosys */
	{ 0, nosys },			/* 150 = nosys */
	{ 3, notimp },			/* 151 = privgrp */
	{ 2, hpux_rtprio },			/* 152 = rtprio */
	{ 1, notimp },			/* 153 = plock */
	{ 2, hpux_netioctl },			/* 154 = hpux_netioctl */
	{ 4, hpux_lockf },			/* 155 = hpux_lockf */
#ifdef SYSVSEM
	{ 3, semget },			/* 156 = semget */
	{ 4, semctl },			/* 157 = semctl */
	{ 3, semop },			/* 158 = semop */
#else
	{ 3, notimp },			/* 156 = semget */
	{ 4, notimp },			/* 157 = semctl */
	{ 3, notimp },			/* 158 = semop */
#endif
#ifdef SYSVMSG
	{ 2, msgget },			/* 159 = msgget */
	{ 3, msgctl },			/* 160 = msgctl */
	{ 4, msgsnd },			/* 161 = msgsnd */
	{ 5, msgrcv },			/* 162 = msgrcv */
#else
	{ 2, notimp },			/* 159 = msgget */
	{ 3, notimp },			/* 160 = msgctl */
	{ 4, notimp },			/* 161 = msgsnd */
	{ 5, notimp },			/* 162 = msgrcv */
#endif
#ifdef SYSVSHM
	{ 3, shmget },			/* 163 = shmget */
	{ 3, shmctl },			/* 164 = shmctl */
	{ 3, shmat },			/* 165 = shmat */
	{ 1, shmdt },			/* 166 = shmdt */
#else
	{ 3, notimp },			/* 163 = shmget */
	{ 3, notimp },			/* 164 = shmctl */
	{ 3, notimp },			/* 165 = shmat */
	{ 1, notimp },			/* 166 = shmdt */
#endif
	{ 1, hpux_advise },			/* 167 = hpux_advise */
	{ 0, notimp },			/* 168 = dux_notconfigured */
	{ 3, notimp },			/* 169 = cluster */
	{ 4, notimp },			/* 170 = mkrnod */
	{ 0, nosys },			/* 171 = nosys */
	{ 0, notimp },			/* 172 = dux_notconfigured */
	{ 0, nosys },			/* 173 = nosys */
	{ 3, hpux_getcontext },			/* 174 = hpux_getcontext */
	{ 0, nosys },			/* 175 = nosys */
	{ 0, nosys },			/* 176 = nosys */
	{ 0, nosys },			/* 177 = nosys */
	{ 0, notimp },			/* 178 = lsync */
	{ 0, nosys },			/* 179 = nosys */
	{ 0, notimp },			/* 180 = mysite */
	{ 0, notimp },			/* 181 = returnzero */
	{ 0, nosys },			/* 182 = nosys */
	{ 0, nosys },			/* 183 = nosys */
	{ 0, nosys },			/* 184 = nosys */
	{ 0, nosys },			/* 185 = nosys */
	{ 3, notimp },			/* 186 = setacl */
	{ 3, notimp },			/* 187 = fsetacl */
	{ 3, notimp },			/* 188 = getacl */
	{ 3, notimp },			/* 189 = fgetacl */
	{ 6, hpux_getaccess },			/* 190 = hpux_getaccess */
	{ 0, notimp },			/* 191 = getaudid */
	{ 1, notimp },			/* 192 = setaudid */
	{ 0, notimp },			/* 193 = getaudproc */
	{ 1, notimp },			/* 194 = setaudproc */
	{ 2, notimp },			/* 195 = getevent */
	{ 2, notimp },			/* 196 = setevent */
	{ 1, notimp },			/* 197 = audwrite */
	{ 1, notimp },			/* 198 = audswitch */
	{ 4, notimp },			/* 199 = audctl */
	{ 3, hpux_waitpid },			/* 200 = hpux_waitpid */
	{ 0, nosys },			/* 201 = nosys */
	{ 2, notimp },			/* 202 = netioctl */
	{ 6, nosys },			/* 203 = nosys */
	{ 0, nosys },			/* 204 = nosys */
	{ 0, nosys },			/* 205 = nosys */
	{ 9, nosys },			/* 206 = nosys */
	{ 0, nosys },			/* 207 = nosys */
	{ 0, nosys },			/* 208 = nosys */
	{ 6, nosys },			/* 209 = nosys */
	{ 5, nosys },			/* 210 = nosys */
	{ 0, nosys },			/* 211 = nosys */
	{ 0, nosys },			/* 212 = nosys */
	{ 0, nosys },			/* 213 = nosys */
	{ 0, nosys },			/* 214 = nosys */
	{ 0, nosys },			/* 215 = nosys */
	{ 0, nosys },			/* 216 = nosys */
	{ 0, nosys },			/* 217 = nosys */
	{ 0, nosys },			/* 218 = nosys */
	{ 0, nosys },			/* 219 = nosys */
	{ 4, nosys },			/* 220 = nosys */
	{ 10, nosys },			/* 221 = nosys */
	{ 0, nosys },			/* 222 = nosys */
	{ 0, nosys },			/* 223 = nosys */
	{ 0, nosys },			/* 224 = nosys */
	{ 2, notimp },			/* 225 = pathconf */
	{ 2, notimp },			/* 226 = fpathconf */
	{ 0, nosys },			/* 227 = nosys */
	{ 0, nosys },			/* 228 = nosys */
	{ 0, notimp },			/* 229 = async_daemon */
	{ 3, notimp },			/* 230 = nfs_fcntl */
	{ 4, getdirentries },			/* 231 = getdirentries */
	{ 2, getdomainname },			/* 232 = getdomainname */
	{ 2, notimp },			/* 233 = nfs_getfh */
	{ 4, notimp },			/* 234 = vfsmount */
	{ 1, notimp },			/* 235 = nfs_svc */
	{ 2, setdomainname },			/* 236 = setdomainname */
	{ 0, notimp },			/* 237 = statfs */
	{ 0, notimp },			/* 238 = fstatfs */
	{ 3, hpux_sigaction },			/* 239 = hpux_sigaction */
	{ 3, hpux_sigprocmask },			/* 240 = hpux_sigprocmask */
	{ 1, hpux_sigpending },			/* 241 = hpux_sigpending */
	{ 1, hpux_sigsuspend },			/* 242 = hpux_sigsuspend */
};

int	nhpux_sysent = sizeof(hpux_sysent) / sizeof(hpux_sysent[0]);
