/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.23 1994/10/20 04:23:12 cgd Exp 
 */

#define	syscallarg(x)	union { x datum; register_t pad; }

struct exit_args {
	syscallarg(int) rval;
};

struct read_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) nbyte;
};

struct write_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) nbyte;
};

struct open_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct close_args {
	syscallarg(int) fd;
};

struct wait4_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(struct rusage *) rusage;
};

struct compat_43_creat_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct link_args {
	syscallarg(char *) path;
	syscallarg(char *) link;
};

struct unlink_args {
	syscallarg(char *) path;
};

struct chdir_args {
	syscallarg(char *) path;
};

struct fchdir_args {
	syscallarg(int) fd;
};

struct mknod_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct chmod_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct chown_args {
	syscallarg(char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct obreak_args {
	syscallarg(char *) nsize;
};

struct getfsstat_args {
	syscallarg(struct statfs *) buf;
	syscallarg(long) bufsize;
	syscallarg(int) flags;
};

struct compat_43_lseek_args {
	syscallarg(int) fd;
	syscallarg(long) offset;
	syscallarg(int) whence;
};

struct mount_args {
	syscallarg(char *) type;
	syscallarg(char *) path;
	syscallarg(int) flags;
	syscallarg(caddr_t) data;
};

struct unmount_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
};

struct setuid_args {
	syscallarg(uid_t) uid;
};

struct ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(caddr_t) addr;
	syscallarg(int) data;
};

struct recvmsg_args {
	syscallarg(int) s;
	syscallarg(struct msghdr *) msg;
	syscallarg(int) flags;
};

struct sendmsg_args {
	syscallarg(int) s;
	syscallarg(caddr_t) msg;
	syscallarg(int) flags;
};

struct recvfrom_args {
	syscallarg(int) s;
	syscallarg(caddr_t) buf;
	syscallarg(size_t) len;
	syscallarg(int) flags;
	syscallarg(caddr_t) from;
	syscallarg(int *) fromlenaddr;
};

struct accept_args {
	syscallarg(int) s;
	syscallarg(caddr_t) name;
	syscallarg(int *) anamelen;
};

struct getpeername_args {
	syscallarg(int) fdes;
	syscallarg(caddr_t) asa;
	syscallarg(int *) alen;
};

struct getsockname_args {
	syscallarg(int) fdes;
	syscallarg(caddr_t) asa;
	syscallarg(int *) alen;
};

struct access_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
};

struct chflags_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
};

struct fchflags_args {
	syscallarg(int) fd;
	syscallarg(int) flags;
};

struct kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};

struct compat_43_stat_args {
	syscallarg(char *) path;
	syscallarg(struct ostat *) ub;
};

struct compat_43_lstat_args {
	syscallarg(char *) path;
	syscallarg(struct ostat *) ub;
};

struct dup_args {
	syscallarg(u_int) fd;
};

struct profil_args {
	syscallarg(caddr_t) samples;
	syscallarg(u_int) size;
	syscallarg(u_int) offset;
	syscallarg(u_int) scale;
};

struct ktrace_args {
	syscallarg(char *) fname;
	syscallarg(int) ops;
	syscallarg(int) facs;
	syscallarg(int) pid;
};

struct sigaction_args {
	syscallarg(int) signum;
	syscallarg(struct sigaction *) nsa;
	syscallarg(struct sigaction *) osa;
};

struct sigprocmask_args {
	syscallarg(int) how;
	syscallarg(sigset_t) mask;
};

struct getlogin_args {
	syscallarg(char *) namebuf;
	syscallarg(u_int) namelen;
};

struct setlogin_args {
	syscallarg(char *) namebuf;
};

struct acct_args {
	syscallarg(char *) path;
};

struct sigaltstack_args {
	syscallarg(struct sigaltstack *) nss;
	syscallarg(struct sigaltstack *) oss;
};

struct ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(caddr_t) data;
};

struct reboot_args {
	syscallarg(int) opt;
};

struct revoke_args {
	syscallarg(char *) path;
};

struct symlink_args {
	syscallarg(char *) path;
	syscallarg(char *) link;
};

struct readlink_args {
	syscallarg(char *) path;
	syscallarg(char *) buf;
	syscallarg(int) count;
};

struct execve_args {
	syscallarg(char *) path;
	syscallarg(char * *) argp;
	syscallarg(char * *) envp;
};

struct umask_args {
	syscallarg(int) newmask;
};

struct chroot_args {
	syscallarg(char *) path;
};

struct compat_43_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct ostat *) sb;
};

struct compat_43_getkerninfo_args {
	syscallarg(int) op;
	syscallarg(char *) where;
	syscallarg(int *) size;
	syscallarg(int) arg;
};

struct msync_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
};

struct sbrk_args {
	syscallarg(int) incr;
};

struct sstk_args {
	syscallarg(int) incr;
};

struct compat_43_mmap_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(long) pos;
};

struct ovadvise_args {
	syscallarg(int) anom;
};

struct munmap_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
};

struct mprotect_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
	syscallarg(int) prot;
};

struct madvise_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
	syscallarg(int) behav;
};

struct mincore_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
	syscallarg(char *) vec;
};

struct getgroups_args {
	syscallarg(u_int) gidsetsize;
	syscallarg(gid_t *) gidset;
};

struct setgroups_args {
	syscallarg(u_int) gidsetsize;
	syscallarg(gid_t *) gidset;
};

struct setpgid_args {
	syscallarg(int) pid;
	syscallarg(int) pgid;
};

struct setitimer_args {
	syscallarg(u_int) which;
	syscallarg(struct itimerval *) itv;
	syscallarg(struct itimerval *) oitv;
};

struct swapon_args {
	syscallarg(char *) name;
};

struct getitimer_args {
	syscallarg(u_int) which;
	syscallarg(struct itimerval *) itv;
};

struct compat_43_gethostname_args {
	syscallarg(char *) hostname;
	syscallarg(u_int) len;
};

struct compat_43_sethostname_args {
	syscallarg(char *) hostname;
	syscallarg(u_int) len;
};

struct dup2_args {
	syscallarg(u_int) from;
	syscallarg(u_int) to;
};

struct fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};

struct select_args {
	syscallarg(u_int) nd;
	syscallarg(fd_set *) in;
	syscallarg(fd_set *) ou;
	syscallarg(fd_set *) ex;
	syscallarg(struct timeval *) tv;
};

struct fsync_args {
	syscallarg(int) fd;
};

struct setpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
	syscallarg(int) prio;
};

struct socket_args {
	syscallarg(int) domain;
	syscallarg(int) type;
	syscallarg(int) protocol;
};

struct connect_args {
	syscallarg(int) s;
	syscallarg(caddr_t) name;
	syscallarg(int) namelen;
};

struct compat_43_accept_args {
	syscallarg(int) s;
	syscallarg(caddr_t) name;
	syscallarg(int *) anamelen;
};

struct getpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
};

struct compat_43_send_args {
	syscallarg(int) s;
	syscallarg(caddr_t) buf;
	syscallarg(int) len;
	syscallarg(int) flags;
};

struct compat_43_recv_args {
	syscallarg(int) s;
	syscallarg(caddr_t) buf;
	syscallarg(int) len;
	syscallarg(int) flags;
};

struct sigreturn_args {
	syscallarg(struct sigcontext *) sigcntxp;
};

struct bind_args {
	syscallarg(int) s;
	syscallarg(caddr_t) name;
	syscallarg(int) namelen;
};

struct setsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(caddr_t) val;
	syscallarg(int) valsize;
};

struct listen_args {
	syscallarg(int) s;
	syscallarg(int) backlog;
};

struct compat_43_sigvec_args {
	syscallarg(int) signum;
	syscallarg(struct sigvec *) nsv;
	syscallarg(struct sigvec *) osv;
};

struct compat_43_sigblock_args {
	syscallarg(int) mask;
};

struct compat_43_sigsetmask_args {
	syscallarg(int) mask;
};

struct sigsuspend_args {
	syscallarg(int) mask;
};

struct compat_43_sigstack_args {
	syscallarg(struct sigstack *) nss;
	syscallarg(struct sigstack *) oss;
};

struct compat_43_recvmsg_args {
	syscallarg(int) s;
	syscallarg(struct omsghdr *) msg;
	syscallarg(int) flags;
};

struct compat_43_sendmsg_args {
	syscallarg(int) s;
	syscallarg(caddr_t) msg;
	syscallarg(int) flags;
};

struct vtrace_args {
	syscallarg(int) request;
	syscallarg(int) value;
};

struct gettimeofday_args {
	syscallarg(struct timeval *) tp;
	syscallarg(struct timezone *) tzp;
};

struct getrusage_args {
	syscallarg(int) who;
	syscallarg(struct rusage *) rusage;
};

struct getsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(caddr_t) val;
	syscallarg(int *) avalsize;
};

struct resuba_args {
	syscallarg(int) value;
};

struct readv_args {
	syscallarg(int) fd;
	syscallarg(struct iovec *) iovp;
	syscallarg(u_int) iovcnt;
};

struct writev_args {
	syscallarg(int) fd;
	syscallarg(struct iovec *) iovp;
	syscallarg(u_int) iovcnt;
};

struct settimeofday_args {
	syscallarg(struct timeval *) tv;
	syscallarg(struct timezone *) tzp;
};

struct fchown_args {
	syscallarg(int) fd;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct fchmod_args {
	syscallarg(int) fd;
	syscallarg(int) mode;
};

struct compat_43_recvfrom_args {
	syscallarg(int) s;
	syscallarg(caddr_t) buf;
	syscallarg(size_t) len;
	syscallarg(int) flags;
	syscallarg(caddr_t) from;
	syscallarg(int *) fromlenaddr;
};

struct compat_43_setreuid_args {
	syscallarg(int) ruid;
	syscallarg(int) euid;
};

struct compat_43_setregid_args {
	syscallarg(int) rgid;
	syscallarg(int) egid;
};

struct rename_args {
	syscallarg(char *) from;
	syscallarg(char *) to;
};

struct compat_43_truncate_args {
	syscallarg(char *) path;
	syscallarg(long) length;
};

struct compat_43_ftruncate_args {
	syscallarg(int) fd;
	syscallarg(long) length;
};

struct flock_args {
	syscallarg(int) fd;
	syscallarg(int) how;
};

struct mkfifo_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct sendto_args {
	syscallarg(int) s;
	syscallarg(caddr_t) buf;
	syscallarg(size_t) len;
	syscallarg(int) flags;
	syscallarg(caddr_t) to;
	syscallarg(int) tolen;
};

struct shutdown_args {
	syscallarg(int) s;
	syscallarg(int) how;
};

struct socketpair_args {
	syscallarg(int) domain;
	syscallarg(int) type;
	syscallarg(int) protocol;
	syscallarg(int *) rsv;
};

struct mkdir_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct rmdir_args {
	syscallarg(char *) path;
};

struct utimes_args {
	syscallarg(char *) path;
	syscallarg(struct timeval *) tptr;
};

struct adjtime_args {
	syscallarg(struct timeval *) delta;
	syscallarg(struct timeval *) olddelta;
};

struct compat_43_getpeername_args {
	syscallarg(int) fdes;
	syscallarg(caddr_t) asa;
	syscallarg(int *) alen;
};

struct compat_43_sethostid_args {
	syscallarg(int32_t) hostid;
};

struct compat_43_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct ogetrlimit *) rlp;
};

struct compat_43_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct ogetrlimit *) rlp;
};

struct compat_43_killpg_args {
	syscallarg(int) pgid;
	syscallarg(int) signum;
};

struct quotactl_args {
	syscallarg(char *) path;
	syscallarg(int) cmd;
	syscallarg(int) uid;
	syscallarg(caddr_t) arg;
};

struct compat_43_getsockname_args {
	syscallarg(int) fdec;
	syscallarg(caddr_t) asa;
	syscallarg(int *) alen;
};

struct nfssvc_args {
	syscallarg(int) flag;
	syscallarg(caddr_t) argp;
};

struct compat_43_getdirentries_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) count;
	syscallarg(long *) basep;
};

struct statfs_args {
	syscallarg(char *) path;
	syscallarg(struct statfs *) buf;
};

struct fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct statfs *) buf;
};

struct getfh_args {
	syscallarg(char *) fname;
	syscallarg(fhandle_t *) fhp;
};

struct compat_09_getdomainname_args {
	syscallarg(char *) domainname;
	syscallarg(int) len;
};

struct compat_09_setdomainname_args {
	syscallarg(char *) domainname;
	syscallarg(int) len;
};

struct compat_09_uname_args {
	syscallarg(struct outsname *) name;
};

struct sysarch_args {
	syscallarg(int) op;
	syscallarg(char *) parms;
};

struct compat_10_semsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
};

struct compat_10_msgsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
	syscallarg(int) a6;
};

struct compat_10_shmsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
};

struct setgid_args {
	syscallarg(gid_t) gid;
};

struct setegid_args {
	syscallarg(gid_t) egid;
};

struct seteuid_args {
	syscallarg(uid_t) euid;
};

struct lfs_bmapv_args {
	syscallarg(fsid_t *) fsidp;
	syscallarg(struct block_info *) blkiov;
	syscallarg(int) blkcnt;
};

struct lfs_markv_args {
	syscallarg(fsid_t *) fsidp;
	syscallarg(struct block_info *) blkiov;
	syscallarg(int) blkcnt;
};

struct lfs_segclean_args {
	syscallarg(fsid_t *) fsidp;
	syscallarg(u_long) segment;
};

struct lfs_segwait_args {
	syscallarg(fsid_t *) fsidp;
	syscallarg(struct timeval *) tv;
};

struct stat_args {
	syscallarg(char *) path;
	syscallarg(struct stat *) ub;
};

struct fstat_args {
	syscallarg(int) fd;
	syscallarg(struct stat *) sb;
};

struct lstat_args {
	syscallarg(char *) path;
	syscallarg(struct stat *) ub;
};

struct pathconf_args {
	syscallarg(char *) path;
	syscallarg(int) name;
};

struct fpathconf_args {
	syscallarg(int) fd;
	syscallarg(int) name;
};

struct getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct rlimit *) rlp;
};

struct setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct rlimit *) rlp;
};

struct getdirentries_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(u_int) count;
	syscallarg(long *) basep;
};

struct mmap_args {
	syscallarg(caddr_t) addr;
	syscallarg(size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(long) pad;
	syscallarg(off_t) pos;
};

struct lseek_args {
	syscallarg(int) fd;
	syscallarg(int) pad;
	syscallarg(off_t) offset;
	syscallarg(int) whence;
};

struct truncate_args {
	syscallarg(char *) path;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};

struct ftruncate_args {
	syscallarg(int) fd;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};

struct __sysctl_args {
	syscallarg(int *) name;
	syscallarg(u_int) namelen;
	syscallarg(void *) old;
	syscallarg(size_t *) oldlenp;
	syscallarg(void *) new;
	syscallarg(size_t) newlen;
};

struct mlock_args {
	syscallarg(caddr_t) addr;
	syscallarg(size_t) len;
};

struct munlock_args {
	syscallarg(caddr_t) addr;
	syscallarg(size_t) len;
};

struct __semctl_args {
	syscallarg(int) semid;
	syscallarg(int) semnum;
	syscallarg(int) cmd;
	syscallarg(union semun *) arg;
};

struct semget_args {
	syscallarg(key_t) key;
	syscallarg(int) nsems;
	syscallarg(int) semflg;
};

struct semop_args {
	syscallarg(int) semid;
	syscallarg(struct sembuf *) sops;
	syscallarg(u_int) nsops;
};

struct semconfig_args {
	syscallarg(int) flag;
};

struct msgctl_args {
	syscallarg(int) msqid;
	syscallarg(int) cmd;
	syscallarg(struct msqid_ds *) buf;
};

struct msgget_args {
	syscallarg(key_t) key;
	syscallarg(int) msgflg;
};

struct msgsnd_args {
	syscallarg(int) msqid;
	syscallarg(void *) msgp;
	syscallarg(size_t) msgsz;
	syscallarg(int) msgflg;
};

struct msgrcv_args {
	syscallarg(int) msqid;
	syscallarg(void *) msgp;
	syscallarg(size_t) msgsz;
	syscallarg(long) msgtyp;
	syscallarg(int) msgflg;
};

struct shmat_args {
	syscallarg(int) shmid;
	syscallarg(void *) shmaddr;
	syscallarg(int) shmflg;
};

struct shmctl_args {
	syscallarg(int) shmid;
	syscallarg(int) cmd;
	syscallarg(struct shmid_ds *) buf;
};

struct shmdt_args {
	syscallarg(void *) shmaddr;
};

struct shmget_args {
	syscallarg(key_t) key;
	syscallarg(int) size;
	syscallarg(int) shmflg;
};

#undef	syscallarg
