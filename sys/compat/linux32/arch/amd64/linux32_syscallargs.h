/* $NetBSD: linux32_syscallargs.h,v 1.22 2007/11/11 18:28:18 christos Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.19 2007/11/07 00:24:30 njoly Exp
 */

#ifndef _LINUX32_SYS_SYSCALLARGS_H_
#define	_LINUX32_SYS_SYSCALLARGS_H_

#define	LINUX32_SYS_MAXSYSARGS	8

#undef	syscallarg
#define	syscallarg(x)							\
	union {								\
		register32_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register32_t) < sizeof (x))	\
				? 0					\
				: sizeof (register32_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= LINUX32_SYS_MAXSYSARGS * sizeof (register32_t) ? 1 : -1];

struct linux32_sys_open_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};
check_syscall_args(linux32_sys_open)

struct linux32_sys_waitpid_args {
	syscallarg(int) pid;
	syscallarg(netbsd32_intp) status;
	syscallarg(int) options;
};
check_syscall_args(linux32_sys_waitpid)

struct linux32_sys_creat_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) mode;
};
check_syscall_args(linux32_sys_creat)

struct linux32_sys_unlink_args {
	syscallarg(const netbsd32_charp) path;
};
check_syscall_args(linux32_sys_unlink)

struct linux32_sys_time_args {
	syscallarg(linux32_timep_t) t;
};
check_syscall_args(linux32_sys_time)

struct linux32_sys_mknod_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};
check_syscall_args(linux32_sys_mknod)

struct linux32_sys_lchown16_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};
check_syscall_args(linux32_sys_lchown16)

struct linux32_sys_break_args {
	syscallarg(netbsd32_charp) nsize;
};
check_syscall_args(linux32_sys_break)

struct linux32_sys_stime_args {
	syscallarg(linux32_timep_t) t;
};
check_syscall_args(linux32_sys_stime)

struct linux32_sys_alarm_args {
	syscallarg(unsigned int) secs;
};
check_syscall_args(linux32_sys_alarm)

struct linux32_sys_utime_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(linux32_utimbufp_t) times;
};
check_syscall_args(linux32_sys_utime)

struct linux32_sys_nice_args {
	syscallarg(int) incr;
};
check_syscall_args(linux32_sys_nice)

struct linux32_sys_kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};
check_syscall_args(linux32_sys_kill)

struct linux32_sys_rename_args {
	syscallarg(const netbsd32_charp) from;
	syscallarg(const netbsd32_charp) to;
};
check_syscall_args(linux32_sys_rename)

struct linux32_sys_pipe_args {
	syscallarg(netbsd32_intp) fd;
};
check_syscall_args(linux32_sys_pipe)

struct linux32_sys_times_args {
	syscallarg(linux32_tmsp_t) tms;
};
check_syscall_args(linux32_sys_times)

struct linux32_sys_brk_args {
	syscallarg(netbsd32_charp) nsize;
};
check_syscall_args(linux32_sys_brk)

struct linux32_sys_signal_args {
	syscallarg(int) signum;
	syscallarg(linux32_handler_t) handler;
};
check_syscall_args(linux32_sys_signal)

struct linux32_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_u_long) com;
	syscallarg(netbsd32_charp) data;
};
check_syscall_args(linux32_sys_ioctl)

struct linux32_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};
check_syscall_args(linux32_sys_fcntl)

struct linux32_sys_oldolduname_args {
	syscallarg(linux32_oldold_utsnamep_t) up;
};
check_syscall_args(linux32_sys_oldolduname)

struct linux32_sys_setreuid16_args {
	syscallarg(int) ruid;
	syscallarg(int) euid;
};
check_syscall_args(linux32_sys_setreuid16)

struct linux32_sys_setregid16_args {
	syscallarg(int) rgid;
	syscallarg(int) egid;
};
check_syscall_args(linux32_sys_setregid16)

struct linux32_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_setrlimit)

struct linux32_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_getrlimit)

struct linux32_sys_gettimeofday_args {
	syscallarg(netbsd32_timevalp_t) tp;
	syscallarg(netbsd32_timezonep_t) tzp;
};
check_syscall_args(linux32_sys_gettimeofday)

struct linux32_sys_settimeofday_args {
	syscallarg(netbsd32_timevalp_t) tp;
	syscallarg(netbsd32_timezonep_t) tzp;
};
check_syscall_args(linux32_sys_settimeofday)

struct linux32_sys_getgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux32_gidp_t) gidset;
};
check_syscall_args(linux32_sys_getgroups16)

struct linux32_sys_setgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux32_gidp_t) gidset;
};
check_syscall_args(linux32_sys_setgroups16)

struct linux32_sys_oldselect_args {
	syscallarg(linux32_oldselectp_t) lsp;
};
check_syscall_args(linux32_sys_oldselect)

struct linux32_sys_readlink_args {
	syscallarg(const netbsd32_charp) name;
	syscallarg(netbsd32_charp) buf;
	syscallarg(int) count;
};
check_syscall_args(linux32_sys_readlink)

struct linux32_sys_swapon_args {
	syscallarg(netbsd32_charp) name;
};
check_syscall_args(linux32_sys_swapon)

struct linux32_sys_reboot_args {
	syscallarg(int) magic1;
	syscallarg(int) magic2;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};
check_syscall_args(linux32_sys_reboot)

struct linux32_sys_old_mmap_args {
	syscallarg(linux32_oldmmapp) lmp;
};
check_syscall_args(linux32_sys_old_mmap)

struct linux32_sys_truncate_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(netbsd32_long) length;
};
check_syscall_args(linux32_sys_truncate)

struct linux32_sys_fchown16_args {
	syscallarg(int) fd;
	syscallarg(int) uid;
	syscallarg(int) gid;
};
check_syscall_args(linux32_sys_fchown16)

struct linux32_sys_getpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
};
check_syscall_args(linux32_sys_getpriority)

struct linux32_sys_setpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
	syscallarg(int) prio;
};
check_syscall_args(linux32_sys_setpriority)

struct linux32_sys_statfs_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(linux32_statfsp) sp;
};
check_syscall_args(linux32_sys_statfs)

struct linux32_sys_socketcall_args {
	syscallarg(int) what;
	syscallarg(netbsd32_voidp) args;
};
check_syscall_args(linux32_sys_socketcall)

struct linux32_sys_wait4_args {
	syscallarg(int) pid;
	syscallarg(netbsd32_intp) status;
	syscallarg(int) options;
	syscallarg(netbsd32_rusagep_t) rusage;
};
check_syscall_args(linux32_sys_wait4)

struct linux32_sys_swapoff_args {
	syscallarg(const netbsd32_charp) path;
};
check_syscall_args(linux32_sys_swapoff)

struct linux32_sys_sysinfo_args {
	syscallarg(linux32_sysinfop_t) arg;
};
check_syscall_args(linux32_sys_sysinfo)

struct linux32_sys_sigreturn_args {
	syscallarg(linux32_sigcontextp_t) scp;
};
check_syscall_args(linux32_sys_sigreturn)

struct linux32_sys_clone_args {
	syscallarg(int) flags;
	syscallarg(netbsd32_voidp) stack;
};
check_syscall_args(linux32_sys_clone)

struct linux32_sys_uname_args {
	syscallarg(linux32_utsnamep) up;
};
check_syscall_args(linux32_sys_uname)

struct linux32_sys_mprotect_args {
	syscallarg(netbsd32_voidp) addr;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) prot;
};
check_syscall_args(linux32_sys_mprotect)

struct linux32_sys_llseek_args {
	syscallarg(int) fd;
	syscallarg(u_int32_t) ohigh;
	syscallarg(u_int32_t) olow;
	syscallarg(netbsd32_caddr_t) res;
	syscallarg(int) whence;
};
check_syscall_args(linux32_sys_llseek)

struct linux32_sys_getdents_args {
	syscallarg(int) fd;
	syscallarg(linux32_direntp_t) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux32_sys_getdents)

struct linux32_sys_select_args {
	syscallarg(int) nfds;
	syscallarg(netbsd32_fd_setp_t) readfds;
	syscallarg(netbsd32_fd_setp_t) writefds;
	syscallarg(netbsd32_fd_setp_t) exceptfds;
	syscallarg(netbsd32_timevalp_t) timeout;
};
check_syscall_args(linux32_sys_select)

struct linux32_sys_fdatasync_args {
	syscallarg(int) fd;
};
check_syscall_args(linux32_sys_fdatasync)

struct linux32_sys___sysctl_args {
	syscallarg(linux32___sysctlp_t) lsp;
};
check_syscall_args(linux32_sys___sysctl)

struct linux32_sys_sched_getparam_args {
	syscallarg(pid_t) pid;
	syscallarg(linux32_sched_paramp_t) sp;
};
check_syscall_args(linux32_sys_sched_getparam)

struct linux32_sys_sched_setscheduler_args {
	syscallarg(pid_t) pid;
	syscallarg(int) policy;
	syscallarg(const linux32_sched_paramp_t) sp;
};
check_syscall_args(linux32_sys_sched_setscheduler)

struct linux32_sys_sched_getscheduler_args {
	syscallarg(pid_t) pid;
};
check_syscall_args(linux32_sys_sched_getscheduler)

struct linux32_sys_mremap_args {
	syscallarg(netbsd32_voidp) old_address;
	syscallarg(netbsd32_size_t) old_size;
	syscallarg(netbsd32_size_t) new_size;
	syscallarg(netbsd32_u_long) flags;
};
check_syscall_args(linux32_sys_mremap)

struct linux32_sys_setresuid16_args {
	syscallarg(uid_t) ruid;
	syscallarg(uid_t) euid;
	syscallarg(uid_t) suid;
};
check_syscall_args(linux32_sys_setresuid16)

struct linux32_sys_setresgid16_args {
	syscallarg(gid_t) rgid;
	syscallarg(gid_t) egid;
	syscallarg(gid_t) sgid;
};
check_syscall_args(linux32_sys_setresgid16)

struct linux32_sys_rt_sigreturn_args {
	syscallarg(linux32_ucontextp_t) ucp;
};
check_syscall_args(linux32_sys_rt_sigreturn)

struct linux32_sys_rt_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const linux32_sigactionp_t) nsa;
	syscallarg(linux32_sigactionp_t) osa;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigaction)

struct linux32_sys_rt_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const linux32_sigsetp_t) set;
	syscallarg(linux32_sigsetp_t) oset;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigprocmask)

struct linux32_sys_rt_sigsuspend_args {
	syscallarg(linux32_sigsetp_t) unewset;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigsuspend)

struct linux32_sys_chown16_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};
check_syscall_args(linux32_sys_chown16)

struct linux32_sys_ugetrlimit_args {
	syscallarg(int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_ugetrlimit)

struct linux32_sys_mmap2_args {
	syscallarg(netbsd32_u_long) addr;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(linux32_off_t) offset;
};
check_syscall_args(linux32_sys_mmap2)

struct linux32_sys_stat64_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_stat64)

struct linux32_sys_lstat64_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_lstat64)

struct linux32_sys_fstat64_args {
	syscallarg(int) fd;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_fstat64)

struct linux32_sys_setresuid_args {
	syscallarg(uid_t) ruid;
	syscallarg(uid_t) euid;
	syscallarg(uid_t) suid;
};
check_syscall_args(linux32_sys_setresuid)

struct linux32_sys_setresgid_args {
	syscallarg(gid_t) rgid;
	syscallarg(gid_t) egid;
	syscallarg(gid_t) sgid;
};
check_syscall_args(linux32_sys_setresgid)

struct linux32_sys_setfsuid_args {
	syscallarg(uid_t) uid;
};
check_syscall_args(linux32_sys_setfsuid)

struct linux32_sys_getdents64_args {
	syscallarg(int) fd;
	syscallarg(linux32_dirent64p_t) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux32_sys_getdents64)

struct linux32_sys_fcntl64_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};
check_syscall_args(linux32_sys_fcntl64)

struct linux32_sys_exit_group_args {
	syscallarg(int) error_code;
};
check_syscall_args(linux32_sys_exit_group)

/*
 * System call prototypes.
 */

int	linux_sys_nosys(struct lwp *, void *, register_t *);

int	netbsd32_exit(struct lwp *, void *, register_t *);

int	sys_fork(struct lwp *, void *, register_t *);

int	netbsd32_read(struct lwp *, void *, register_t *);

int	netbsd32_write(struct lwp *, void *, register_t *);

int	linux32_sys_open(struct lwp *, void *, register_t *);

int	netbsd32_close(struct lwp *, void *, register_t *);

int	linux32_sys_waitpid(struct lwp *, void *, register_t *);

int	linux32_sys_creat(struct lwp *, void *, register_t *);

int	netbsd32_link(struct lwp *, void *, register_t *);

int	linux32_sys_unlink(struct lwp *, void *, register_t *);

int	netbsd32_execve(struct lwp *, void *, register_t *);

int	netbsd32_chdir(struct lwp *, void *, register_t *);

int	linux32_sys_time(struct lwp *, void *, register_t *);

int	linux32_sys_mknod(struct lwp *, void *, register_t *);

int	netbsd32_chmod(struct lwp *, void *, register_t *);

int	linux32_sys_lchown16(struct lwp *, void *, register_t *);

int	linux32_sys_break(struct lwp *, void *, register_t *);

int	compat_43_netbsd32_olseek(struct lwp *, void *, register_t *);

int	linux_sys_getpid(struct lwp *, void *, register_t *);

int	netbsd32_setuid(struct lwp *, void *, register_t *);

int	sys_getuid(struct lwp *, void *, register_t *);

int	linux32_sys_stime(struct lwp *, void *, register_t *);

int	linux32_sys_alarm(struct lwp *, void *, register_t *);

int	linux_sys_pause(struct lwp *, void *, register_t *);

int	linux32_sys_utime(struct lwp *, void *, register_t *);

int	netbsd32_access(struct lwp *, void *, register_t *);

int	linux32_sys_nice(struct lwp *, void *, register_t *);

int	sys_sync(struct lwp *, void *, register_t *);

int	linux32_sys_kill(struct lwp *, void *, register_t *);

int	linux32_sys_rename(struct lwp *, void *, register_t *);

int	netbsd32_mkdir(struct lwp *, void *, register_t *);

int	netbsd32_rmdir(struct lwp *, void *, register_t *);

int	netbsd32_dup(struct lwp *, void *, register_t *);

int	linux32_sys_pipe(struct lwp *, void *, register_t *);

int	linux32_sys_times(struct lwp *, void *, register_t *);

int	linux32_sys_brk(struct lwp *, void *, register_t *);

int	netbsd32_setgid(struct lwp *, void *, register_t *);

int	sys_getgid(struct lwp *, void *, register_t *);

int	linux32_sys_signal(struct lwp *, void *, register_t *);

int	sys_geteuid(struct lwp *, void *, register_t *);

int	sys_getegid(struct lwp *, void *, register_t *);

int	netbsd32_acct(struct lwp *, void *, register_t *);

int	linux32_sys_ioctl(struct lwp *, void *, register_t *);

int	linux32_sys_fcntl(struct lwp *, void *, register_t *);

int	netbsd32_setpgid(struct lwp *, void *, register_t *);

int	linux32_sys_oldolduname(struct lwp *, void *, register_t *);

int	netbsd32_umask(struct lwp *, void *, register_t *);

int	netbsd32_chroot(struct lwp *, void *, register_t *);

int	netbsd32_dup2(struct lwp *, void *, register_t *);

int	linux_sys_getppid(struct lwp *, void *, register_t *);

int	sys_getpgrp(struct lwp *, void *, register_t *);

int	sys_setsid(struct lwp *, void *, register_t *);

int	linux32_sys_setreuid16(struct lwp *, void *, register_t *);

int	linux32_sys_setregid16(struct lwp *, void *, register_t *);

int	compat_43_netbsd32_osethostname(struct lwp *, void *, register_t *);

int	linux32_sys_setrlimit(struct lwp *, void *, register_t *);

int	linux32_sys_getrlimit(struct lwp *, void *, register_t *);

int	netbsd32_getrusage(struct lwp *, void *, register_t *);

int	linux32_sys_gettimeofday(struct lwp *, void *, register_t *);

int	linux32_sys_settimeofday(struct lwp *, void *, register_t *);

int	linux32_sys_getgroups16(struct lwp *, void *, register_t *);

int	linux32_sys_setgroups16(struct lwp *, void *, register_t *);

int	linux32_sys_oldselect(struct lwp *, void *, register_t *);

int	netbsd32_symlink(struct lwp *, void *, register_t *);

int	compat_43_netbsd32_lstat43(struct lwp *, void *, register_t *);

int	linux32_sys_readlink(struct lwp *, void *, register_t *);

int	linux32_sys_swapon(struct lwp *, void *, register_t *);

int	linux32_sys_reboot(struct lwp *, void *, register_t *);

int	linux32_sys_old_mmap(struct lwp *, void *, register_t *);

int	netbsd32_munmap(struct lwp *, void *, register_t *);

int	linux32_sys_truncate(struct lwp *, void *, register_t *);

int	compat_43_netbsd32_oftruncate(struct lwp *, void *, register_t *);

int	netbsd32_fchmod(struct lwp *, void *, register_t *);

int	linux32_sys_fchown16(struct lwp *, void *, register_t *);

int	linux32_sys_getpriority(struct lwp *, void *, register_t *);

int	linux32_sys_setpriority(struct lwp *, void *, register_t *);

int	linux32_sys_statfs(struct lwp *, void *, register_t *);

int	linux32_sys_socketcall(struct lwp *, void *, register_t *);

int	netbsd32_setitimer(struct lwp *, void *, register_t *);

int	netbsd32_getitimer(struct lwp *, void *, register_t *);

int	linux32_sys_wait4(struct lwp *, void *, register_t *);

int	linux32_sys_swapoff(struct lwp *, void *, register_t *);

int	linux32_sys_sysinfo(struct lwp *, void *, register_t *);

int	netbsd32_fsync(struct lwp *, void *, register_t *);

int	linux32_sys_sigreturn(struct lwp *, void *, register_t *);

int	linux32_sys_clone(struct lwp *, void *, register_t *);

int	linux32_sys_uname(struct lwp *, void *, register_t *);

int	linux32_sys_mprotect(struct lwp *, void *, register_t *);

int	netbsd32_fchdir(struct lwp *, void *, register_t *);

int	linux32_sys_setfsuid(struct lwp *, void *, register_t *);

int	linux_sys_getfsuid(struct lwp *, void *, register_t *);

int	linux32_sys_llseek(struct lwp *, void *, register_t *);

int	linux32_sys_getdents(struct lwp *, void *, register_t *);

int	linux32_sys_select(struct lwp *, void *, register_t *);

int	netbsd32_flock(struct lwp *, void *, register_t *);

int	netbsd32_readv(struct lwp *, void *, register_t *);

int	netbsd32_writev(struct lwp *, void *, register_t *);

int	netbsd32_getsid(struct lwp *, void *, register_t *);

int	linux32_sys_fdatasync(struct lwp *, void *, register_t *);

int	linux32_sys___sysctl(struct lwp *, void *, register_t *);

int	netbsd32_mlockall(struct lwp *, void *, register_t *);

int	sys_munlockall(struct lwp *, void *, register_t *);

int	linux32_sys_sched_getparam(struct lwp *, void *, register_t *);

int	linux32_sys_sched_setscheduler(struct lwp *, void *, register_t *);

int	linux32_sys_sched_getscheduler(struct lwp *, void *, register_t *);

int	linux_sys_sched_yield(struct lwp *, void *, register_t *);

int	netbsd32_nanosleep(struct lwp *, void *, register_t *);

int	linux32_sys_mremap(struct lwp *, void *, register_t *);

int	linux32_sys_setresuid16(struct lwp *, void *, register_t *);

int	netbsd32_poll(struct lwp *, void *, register_t *);

int	linux32_sys_setresgid16(struct lwp *, void *, register_t *);

int	linux32_sys_rt_sigreturn(struct lwp *, void *, register_t *);

int	linux32_sys_rt_sigaction(struct lwp *, void *, register_t *);

int	linux32_sys_rt_sigprocmask(struct lwp *, void *, register_t *);

int	linux32_sys_rt_sigsuspend(struct lwp *, void *, register_t *);

int	linux32_sys_chown16(struct lwp *, void *, register_t *);

int	netbsd32___getcwd(struct lwp *, void *, register_t *);

int	sys___vfork14(struct lwp *, void *, register_t *);

int	linux32_sys_ugetrlimit(struct lwp *, void *, register_t *);

int	linux32_sys_mmap2(struct lwp *, void *, register_t *);

int	linux32_sys_stat64(struct lwp *, void *, register_t *);

int	linux32_sys_lstat64(struct lwp *, void *, register_t *);

int	linux32_sys_fstat64(struct lwp *, void *, register_t *);

int	netbsd32___posix_lchown(struct lwp *, void *, register_t *);

int	netbsd32_setreuid(struct lwp *, void *, register_t *);

int	netbsd32_setregid(struct lwp *, void *, register_t *);

int	netbsd32_getgroups(struct lwp *, void *, register_t *);

int	netbsd32_setgroups(struct lwp *, void *, register_t *);

int	netbsd32___posix_fchown(struct lwp *, void *, register_t *);

int	linux32_sys_setresuid(struct lwp *, void *, register_t *);

int	linux32_sys_setresgid(struct lwp *, void *, register_t *);

int	netbsd32___posix_chown(struct lwp *, void *, register_t *);

int	netbsd32_madvise(struct lwp *, void *, register_t *);

int	linux32_sys_getdents64(struct lwp *, void *, register_t *);

int	linux32_sys_fcntl64(struct lwp *, void *, register_t *);

int	linux_sys_gettid(struct lwp *, void *, register_t *);

int	linux32_sys_exit_group(struct lwp *, void *, register_t *);

#endif /* _LINUX32_SYS_SYSCALLARGS_H_ */
