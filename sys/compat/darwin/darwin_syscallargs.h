/* $NetBSD: darwin_syscallargs.h,v 1.37 2003/12/31 02:55:04 manu Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.20 2003/12/16 16:13:59 manu Exp 
 */

#ifndef _DARWIN_SYS__SYSCALLARGS_H_
#define	_DARWIN_SYS__SYSCALLARGS_H_

#ifdef	syscallarg
#undef	syscallarg
#endif

#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

struct bsd_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(mode_t) mode;
};

struct bsd_compat_43_sys_creat_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct bsd_sys_link_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct bsd_sys_unlink_args {
	syscallarg(const char *) path;
};

struct bsd_sys_chdir_args {
	syscallarg(const char *) path;
};

struct darwin_sys_mknod_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
	syscallarg(dev_t) dev;
};

struct bsd_sys_chmod_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct bsd_sys_chown_args {
	syscallarg(const char *) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct darwin_sys_getfsstat_args {
	syscallarg(struct darwin_statfs *) buf;
	syscallarg(long) bufsize;
	syscallarg(int) flags;
};

struct bsd_sys_mount_args {
	syscallarg(const char *) type;
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(void *) data;
};

struct bsd_sys_unmount_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};
#ifdef COMPAT_43
#else
#endif

struct darwin_sys_ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(caddr_t) addr;
	syscallarg(int) data;
};

struct bsd_sys_access_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct bsd_sys_chflags_args {
	syscallarg(const char *) path;
	syscallarg(u_long) flags;
};

struct bsd_compat_43_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat43 *) ub;
};

struct bsd_compat_43_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat43 *) ub;
};
#if defined(KTRACE) || !defined(_KERNEL)
#else
#endif

struct darwin_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(struct darwin___sigaction *) nsa;
	syscallarg(struct sigaction13 *) osa;
};
#ifdef COMPAT_43
#else
#endif

struct darwin_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(sigset13_t *) set;
	syscallarg(sigset13_t *) oset;
};

struct bsd_sys_acct_args {
	syscallarg(const char *) path;
};

struct darwin_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};

struct bsd_sys_revoke_args {
	syscallarg(const char *) path;
};

struct bsd_sys_symlink_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct bsd_sys_readlink_args {
	syscallarg(const char *) path;
	syscallarg(char *) buf;
	syscallarg(size_t) count;
};

struct bsd_sys_execve_args {
	syscallarg(const char *) path;
	syscallarg(char *const *) argp;
	syscallarg(char *const *) envp;
};

struct bsd_sys_chroot_args {
	syscallarg(const char *) path;
};

struct bsd_compat_12_sys_swapon_args {
	syscallarg(const char *) name;
};

struct darwin_sys_sigreturn_args {
	syscallarg(struct darwin_ucontext *) uctx;
};

struct bsd_sys_bind_args {
	syscallarg(int) s;
	syscallarg(const struct sockaddr *) name;
	syscallarg(unsigned int) namelen;
};

struct bsd_sys_rename_args {
	syscallarg(const char *) from;
	syscallarg(const char *) to;
};

struct bsd_compat_43_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(long) length;
};

struct bsd_sys_mkfifo_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct bsd_sys_mkdir_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct bsd_sys_rmdir_args {
	syscallarg(const char *) path;
};

struct bsd_sys_utimes_args {
	syscallarg(const char *) path;
	syscallarg(const struct timeval *) tptr;
};
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
#else
#endif

struct darwin_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct darwin_statfs *) buf;
};

struct darwin_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct darwin_statfs *) buf;
};
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)

struct bsd_sys_getfh_args {
	syscallarg(const char *) fname;
	syscallarg(fhandle_t *) fhp;
};
#else
#endif

struct darwin_sys_kdebug_trace_args {
	syscallarg(int) debugid;
	syscallarg(int) arg1;
	syscallarg(int) arg2;
	syscallarg(int) arg3;
	syscallarg(int) arg4;
	syscallarg(int) arg5;
};

struct darwin_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};

struct darwin_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct stat12 *) sb;
};

struct darwin_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};

struct bsd_sys_pathconf_args {
	syscallarg(const char *) path;
	syscallarg(int) name;
};

struct darwin_sys_lseek_args {
	syscallarg(int) fd;
	syscallarg(long) off1;
	syscallarg(long) off2;
	syscallarg(int) whence;
};

struct bsd_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};

struct darwin_sys___sysctl_args {
	syscallarg(int *) name;
	syscallarg(u_int) namelen;
	syscallarg(void *) oldp;
	syscallarg(size_t *) oldlenp;
	syscallarg(void *) newp;
	syscallarg(size_t) newlen;
};

struct bsd_sys_undelete_args {
	syscallarg(const char *) path;
};

struct darwin_sys_getattrlist_args {
	syscallarg(const char *) path;
	syscallarg(struct darwin_attrlist *) alist;
	syscallarg(void *) attributes;
	syscallarg(size_t) buflen;
	syscallarg(unsigned long) options;
};

struct darwin_sys_load_shared_file_args {
	syscallarg(char *) filename;
	syscallarg(caddr_t) addr;
	syscallarg(u_long) len;
	syscallarg(caddr_t *) base;
	syscallarg(int) count;
	syscallarg(mach_sf_mapping_t *) mappings;
	syscallarg(int *) flags;
};

struct darwin_sys_pthread_exit_args {
	syscallarg(void *) value_ptr;
};

/*
 * System call prototypes.
 */

int	sys_exit(struct lwp *, void *, register_t *);

int	darwin_sys_fork(struct lwp *, void *, register_t *);

int	sys_read(struct lwp *, void *, register_t *);

int	sys_write(struct lwp *, void *, register_t *);

int	bsd_sys_open(struct lwp *, void *, register_t *);

int	sys_close(struct lwp *, void *, register_t *);

int	sys_wait4(struct lwp *, void *, register_t *);

int	bsd_compat_43_sys_creat(struct lwp *, void *, register_t *);

int	bsd_sys_link(struct lwp *, void *, register_t *);

int	bsd_sys_unlink(struct lwp *, void *, register_t *);

int	bsd_sys_chdir(struct lwp *, void *, register_t *);

int	sys_fchdir(struct lwp *, void *, register_t *);

int	darwin_sys_mknod(struct lwp *, void *, register_t *);

int	bsd_sys_chmod(struct lwp *, void *, register_t *);

int	bsd_sys_chown(struct lwp *, void *, register_t *);

int	sys_obreak(struct lwp *, void *, register_t *);

int	darwin_sys_getfsstat(struct lwp *, void *, register_t *);

int	compat_43_sys_lseek(struct lwp *, void *, register_t *);

int	darwin_sys_getpid(struct lwp *, void *, register_t *);

int	bsd_sys_mount(struct lwp *, void *, register_t *);

int	bsd_sys_unmount(struct lwp *, void *, register_t *);

int	sys_setuid(struct lwp *, void *, register_t *);

#ifdef COMPAT_43
int	sys_getuid_with_euid(struct lwp *, void *, register_t *);

#else
int	sys_getuid(struct lwp *, void *, register_t *);

#endif
int	sys_geteuid(struct lwp *, void *, register_t *);

int	darwin_sys_ptrace(struct lwp *, void *, register_t *);

int	sys_recvmsg(struct lwp *, void *, register_t *);

int	sys_sendmsg(struct lwp *, void *, register_t *);

int	sys_recvfrom(struct lwp *, void *, register_t *);

int	sys_accept(struct lwp *, void *, register_t *);

int	sys_getpeername(struct lwp *, void *, register_t *);

int	sys_getsockname(struct lwp *, void *, register_t *);

int	bsd_sys_access(struct lwp *, void *, register_t *);

int	bsd_sys_chflags(struct lwp *, void *, register_t *);

int	sys_fchflags(struct lwp *, void *, register_t *);

int	sys_sync(struct lwp *, void *, register_t *);

int	sys_kill(struct lwp *, void *, register_t *);

int	bsd_compat_43_sys_stat(struct lwp *, void *, register_t *);

int	sys_getppid(struct lwp *, void *, register_t *);

int	bsd_compat_43_sys_lstat(struct lwp *, void *, register_t *);

int	sys_dup(struct lwp *, void *, register_t *);

int	sys_pipe(struct lwp *, void *, register_t *);

int	sys_getegid(struct lwp *, void *, register_t *);

int	sys_profil(struct lwp *, void *, register_t *);

#if defined(KTRACE) || !defined(_KERNEL)
int	sys_ktrace(struct lwp *, void *, register_t *);

#else
#endif
int	darwin_sys_sigaction(struct lwp *, void *, register_t *);

#ifdef COMPAT_43
int	sys_getgid_with_egid(struct lwp *, void *, register_t *);

#else
int	sys_getgid(struct lwp *, void *, register_t *);

#endif
int	darwin_sys_sigprocmask(struct lwp *, void *, register_t *);

int	sys___getlogin(struct lwp *, void *, register_t *);

int	sys___setlogin(struct lwp *, void *, register_t *);

int	bsd_sys_acct(struct lwp *, void *, register_t *);

int	compat_13_sys_sigpending(struct lwp *, void *, register_t *);

int	compat_13_sys_sigaltstack(struct lwp *, void *, register_t *);

int	darwin_sys_ioctl(struct lwp *, void *, register_t *);

int	sys_reboot(struct lwp *, void *, register_t *);

int	bsd_sys_revoke(struct lwp *, void *, register_t *);

int	bsd_sys_symlink(struct lwp *, void *, register_t *);

int	bsd_sys_readlink(struct lwp *, void *, register_t *);

int	bsd_sys_execve(struct lwp *, void *, register_t *);

int	sys_umask(struct lwp *, void *, register_t *);

int	bsd_sys_chroot(struct lwp *, void *, register_t *);

int	compat_43_sys_fstat(struct lwp *, void *, register_t *);

int	compat_43_sys_getpagesize(struct lwp *, void *, register_t *);

int	compat_12_sys_msync(struct lwp *, void *, register_t *);

int	darwin_sys_vfork(struct lwp *, void *, register_t *);

int	sys_sbrk(struct lwp *, void *, register_t *);

int	sys_sstk(struct lwp *, void *, register_t *);

int	sys_mmap(struct lwp *, void *, register_t *);

int	sys_ovadvise(struct lwp *, void *, register_t *);

int	sys_munmap(struct lwp *, void *, register_t *);

int	sys_mprotect(struct lwp *, void *, register_t *);

int	sys_madvise(struct lwp *, void *, register_t *);

int	sys_mincore(struct lwp *, void *, register_t *);

int	sys_getgroups(struct lwp *, void *, register_t *);

int	sys_setgroups(struct lwp *, void *, register_t *);

int	sys_getpgrp(struct lwp *, void *, register_t *);

int	sys_setpgid(struct lwp *, void *, register_t *);

int	sys_setitimer(struct lwp *, void *, register_t *);

int	compat_43_sys_wait(struct lwp *, void *, register_t *);

int	bsd_compat_12_sys_swapon(struct lwp *, void *, register_t *);

int	sys_getitimer(struct lwp *, void *, register_t *);

int	compat_43_sys_gethostname(struct lwp *, void *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, void *, register_t *);

int	compat_43_sys_getdtablesize(struct lwp *, void *, register_t *);

int	sys_dup2(struct lwp *, void *, register_t *);

int	sys_fcntl(struct lwp *, void *, register_t *);

int	sys_select(struct lwp *, void *, register_t *);

int	sys_fsync(struct lwp *, void *, register_t *);

int	sys_setpriority(struct lwp *, void *, register_t *);

int	sys_socket(struct lwp *, void *, register_t *);

int	sys_connect(struct lwp *, void *, register_t *);

int	compat_43_sys_accept(struct lwp *, void *, register_t *);

int	sys_getpriority(struct lwp *, void *, register_t *);

int	compat_43_sys_send(struct lwp *, void *, register_t *);

int	compat_43_sys_recv(struct lwp *, void *, register_t *);

int	darwin_sys_sigreturn(struct lwp *, void *, register_t *);

int	bsd_sys_bind(struct lwp *, void *, register_t *);

int	sys_setsockopt(struct lwp *, void *, register_t *);

int	sys_listen(struct lwp *, void *, register_t *);

int	compat_43_sys_sigvec(struct lwp *, void *, register_t *);

int	compat_43_sys_sigblock(struct lwp *, void *, register_t *);

int	compat_43_sys_sigsetmask(struct lwp *, void *, register_t *);

int	compat_13_sys_sigsuspend(struct lwp *, void *, register_t *);

int	compat_43_sys_sigstack(struct lwp *, void *, register_t *);

int	compat_43_sys_recvmsg(struct lwp *, void *, register_t *);

int	compat_43_sys_sendmsg(struct lwp *, void *, register_t *);

int	sys_gettimeofday(struct lwp *, void *, register_t *);

int	sys_getrusage(struct lwp *, void *, register_t *);

int	sys_getsockopt(struct lwp *, void *, register_t *);

int	sys_readv(struct lwp *, void *, register_t *);

int	sys_writev(struct lwp *, void *, register_t *);

int	sys_settimeofday(struct lwp *, void *, register_t *);

int	sys_fchown(struct lwp *, void *, register_t *);

int	sys_fchmod(struct lwp *, void *, register_t *);

int	compat_43_sys_recvfrom(struct lwp *, void *, register_t *);

int	sys_setreuid(struct lwp *, void *, register_t *);

int	sys_setregid(struct lwp *, void *, register_t *);

int	bsd_sys_rename(struct lwp *, void *, register_t *);

int	bsd_compat_43_sys_truncate(struct lwp *, void *, register_t *);

int	compat_43_sys_ftruncate(struct lwp *, void *, register_t *);

int	sys_flock(struct lwp *, void *, register_t *);

int	bsd_sys_mkfifo(struct lwp *, void *, register_t *);

int	sys_sendto(struct lwp *, void *, register_t *);

int	sys_shutdown(struct lwp *, void *, register_t *);

int	sys_socketpair(struct lwp *, void *, register_t *);

int	bsd_sys_mkdir(struct lwp *, void *, register_t *);

int	bsd_sys_rmdir(struct lwp *, void *, register_t *);

int	bsd_sys_utimes(struct lwp *, void *, register_t *);

int	sys_adjtime(struct lwp *, void *, register_t *);

int	compat_43_sys_getpeername(struct lwp *, void *, register_t *);

int	compat_43_sys_gethostid(struct lwp *, void *, register_t *);

int	compat_43_sys_getrlimit(struct lwp *, void *, register_t *);

int	compat_43_sys_setrlimit(struct lwp *, void *, register_t *);

int	compat_43_sys_killpg(struct lwp *, void *, register_t *);

int	sys_setsid(struct lwp *, void *, register_t *);

int	compat_43_sys_getsockname(struct lwp *, void *, register_t *);

#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	sys_nfssvc(struct lwp *, void *, register_t *);

#else
#endif
int	compat_43_sys_getdirentries(struct lwp *, void *, register_t *);

int	darwin_sys_statfs(struct lwp *, void *, register_t *);

int	darwin_sys_fstatfs(struct lwp *, void *, register_t *);

#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	bsd_sys_getfh(struct lwp *, void *, register_t *);

#else
#endif
int	compat_09_sys_getdomainname(struct lwp *, void *, register_t *);

int	compat_09_sys_setdomainname(struct lwp *, void *, register_t *);

int	darwin_sys_kdebug_trace(struct lwp *, void *, register_t *);

int	sys_setgid(struct lwp *, void *, register_t *);

int	sys_setegid(struct lwp *, void *, register_t *);

int	sys_seteuid(struct lwp *, void *, register_t *);

int	darwin_sys_stat(struct lwp *, void *, register_t *);

int	darwin_sys_fstat(struct lwp *, void *, register_t *);

int	darwin_sys_lstat(struct lwp *, void *, register_t *);

int	bsd_sys_pathconf(struct lwp *, void *, register_t *);

int	sys_fpathconf(struct lwp *, void *, register_t *);

int	sys_getrlimit(struct lwp *, void *, register_t *);

int	sys_setrlimit(struct lwp *, void *, register_t *);

int	compat_12_sys_getdirentries(struct lwp *, void *, register_t *);

int	darwin_sys_lseek(struct lwp *, void *, register_t *);

int	bsd_sys_truncate(struct lwp *, void *, register_t *);

int	sys_ftruncate(struct lwp *, void *, register_t *);

int	darwin_sys___sysctl(struct lwp *, void *, register_t *);

int	sys_mlock(struct lwp *, void *, register_t *);

int	sys_munlock(struct lwp *, void *, register_t *);

int	bsd_sys_undelete(struct lwp *, void *, register_t *);

int	darwin_sys_getattrlist(struct lwp *, void *, register_t *);

int	darwin_sys_load_shared_file(struct lwp *, void *, register_t *);

int	darwin_sys_pthread_exit(struct lwp *, void *, register_t *);

#endif /* _DARWIN_SYS__SYSCALLARGS_H_ */
