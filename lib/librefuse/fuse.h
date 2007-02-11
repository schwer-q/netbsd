/*
 * Copyright � 2007 Alistair Crooks.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef FUSE_H_
#define FUSE_H_	20070123

#include <sys/types.h>

#include <puffs.h>
#include <utime.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fuse_file_info {
	int32_t		flags;
	uint32_t	fh_old;
	int32_t		writepage;
	uint32_t	direct_io:1;
	uint32_t	keep_cache:1;
	uint32_t	flush:1;
	uint32_t	padding:29;
	uint64_t	fh;
	uint64_t	lock_owner;
};

struct fuse_conn_info {
	uint32_t proto_major;
	uint32_t proto_minor;
	uint32_t async_read;
	uint32_t max_write;
	uint32_t max_readahead;
	uint32_t reserved[27];
};

struct fuse_opt {
	const char	*templ;
	uint32_t	offset;
	int32_t		value;
};

#define FUSE_OPT_KEY(templ, key) { templ, -1U, key }

#define FUSE_OPT_END { .templ = NULL }

/**
 * Argument list
 */
struct fuse_args {
	int	argc;
	char	**argv;
	int	allocated;
};

/**
 * Initializer for 'struct fuse_args'
 */
#define FUSE_ARGS_INIT(argc, argv) { argc, argv, 0 }

enum {
	FUSE_OPT_KEY_OPT = -1,
	FUSE_OPT_KEY_NONOPT = -2,
	FUSE_OPT_KEY_KEEP = -3,
	FUSE_OPT_KEY_DISCARD = -4
};

typedef struct fuse_dirh *fuse_dirh_t;

typedef int (*fuse_fill_dir_t)(void *, const char *, const struct stat *, off_t);
typedef int (*fuse_dirfil_t)(fuse_dirh_t, const char *, int, ino_t);

#define FUSE_VERSION	26

/*
 * These operations shadow those in puffs_usermount, and are used
 * as a table of callbacks to make when file system requests come
 * in.
 */
struct fuse_operations {
	int	(*getattr)(const char *, struct stat *);
	int	(*readlink)(const char *, char *, size_t);
	int	(*mknod)(const char *, mode_t, dev_t);
	int	(*mkdir)(const char *, mode_t);
	int	(*unlink)(const char *);
	int	(*rmdir)(const char *);
	int	(*symlink)(const char *, const char *);
	int	(*rename)(const char *, const char *);
	int	(*link)(const char *, const char *);
	int	(*chmod)(const char *, mode_t);
	int	(*chown)(const char *, uid_t, gid_t);
	int	(*truncate)(const char *, off_t);
	int	(*utime)(const char *, struct utimbuf *);
	int	(*open)(const char *, struct fuse_file_info *);
	int	(*read)(const char *, char *, size_t, off_t, struct fuse_file_info *);
	int	(*write)(const char *, const char *, size_t, off_t, struct fuse_file_info *);
	int	(*statfs)(const char *, struct statvfs *);
	int	(*flush)(const char *, struct fuse_file_info *);
	int	(*release)(const char *, struct fuse_file_info *);
	int	(*fsync)(const char *, int, struct fuse_file_info *);
	int	(*setxattr)(const char *, const char *, const char *, size_t, int);
	int	(*getxattr)(const char *, const char *, char *, size_t);
	int	(*listxattr)(const char *, char *, size_t);
	int	(*removexattr)(const char *, const char *);
	int	(*opendir)(const char *, struct fuse_file_info *);
	int	(*readdir)(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
	int	(*getdir)(const char *, fuse_dirh_t, fuse_dirfil_t);
	int	(*releasedir)(const char *, struct fuse_file_info *);
	int	(*fsyncdir)(const char *, int, struct fuse_file_info *);
	void	*(*init)(struct fuse_conn_info *);
	void	(*destroy)(void *);
	int	(*access)(const char *, int);
	int	(*create)(const char *, mode_t, struct fuse_file_info *);
	int	(*ftruncate)(const char *, off_t, struct fuse_file_info *);
	int	(*fgetattr)(const char *, struct stat *, struct fuse_file_info *);
	int	(*lock)(const char *, struct fuse_file_info *, int, struct flock *);
	int	(*utimens)(const char *, const struct timespec *);
	int	(*bmap)(const char *, size_t , uint64_t *);
	struct puffs_ops	puffs_ops;	/* pointer to puffs operations */
};


typedef int (*fuse_opt_proc_t)(void *, const char *, int, struct fuse_args *);


int fuse_opt_add_arg(struct fuse_args *, const char *);
int fuse_opt_parse(struct fuse_args *, void *, const struct fuse_opt *, fuse_opt_proc_t);
int fuse_main_real(int, char **, const struct fuse_operations *, size_t, void *);


#define fuse_main(argc, argv, op) \
            fuse_main_real(argc, argv, op, sizeof(*(op)), NULL)


#ifdef __cplusplus
}
#endif

#endif
