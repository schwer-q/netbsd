/*	$NetBSD: compat_defs.h,v 1.3 2002/01/29 10:20:31 tv Exp $	*/

#ifndef	__NETBSD_COMPAT_DEFS_H__
#define	__NETBSD_COMPAT_DEFS_H__

/* System headers needed for (re)definitions below. */

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <paths.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#if HAVE_SYS_SYSMACROS_H
/* major(), minor() on SVR4 */
#include <sys/sysmacros.h>
#endif
#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#if HAVE_STDDEF_H
#include <stddef.h>
#endif

/* We don't include <pwd.h> here, so that "compat_pwd.h" works. */
struct passwd;

/* Assume an ANSI compiler for the host. */

#undef __P
#define __P(x) x

#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#endif
#ifndef __END_DECLS
#define __END_DECLS
#endif

/* Some things usually in BSD <sys/param.h>. */

#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#define BIG_ENDIAN 4321
#define LITTLE_ENDIAN 1234

#undef BYTE_ORDER
#if WORDS_BIGENDIAN
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#if !defined(__attribute__) && !defined(__GNUC__)
#define __attribute__(x)
#endif
#ifndef __RENAME
#define __RENAME(x)
#endif
#undef __aconst
#define __aconst
#undef __dead
#define __dead

/* Dirent support. */

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) (strlen((dirent)->d_name))
#else
# define dirent direct
# define NAMLEN(dirent) ((dirent)->d_namlen)
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

/* Some bits pulled from NetBSD libc. */

#if !HAVE_FTS_H
#include "compat_fts.h"
#endif
#if !HAVE_VIS_H
#include "compat_vis.h"
#endif

/* Type substitutes. */

#if !HAVE_ID_T
typedef unsigned long id_t;
#endif

/* Prototypes for replacement functions. */

#if !HAVE_ASPRINTF
int asprintf(char **, const char *, ...);
#endif

#if !HAVE_ASNPRINTF
int asnprintf(char **, size_t, const char *, ...);
#endif

#if !HAVE_BASENAME
char *basename(char *);
#endif

#if !HAVE_DIRNAME
char *dirname(char *);
#endif

#if !HAVE_DIRFD
#if HAVE_DIR_DD_FD
#define dirfd(dirp) ((dirp)->dd_fd)
#else
#error cannot figure out how to turn a DIR * into a fd
#endif
#endif

#if !HAVE_ERR_H
void err(int, const char *, ...);
void errx(int, const char *, ...);
void warn(const char *, ...);
void warnx(const char *, ...);
#endif

#if !HAVE_FGETLN
char *fgetln(FILE *, size_t *);
#endif

#if !HAVE_FLOCK
# define LOCK_SH		0x01
# define LOCK_EX		0x02
# define LOCK_NB		0x04
# define LOCK_UN		0x08
int flock(int, int);
#endif

#if !HAVE_FPARSELN
# define FPARSELN_UNESCESC	0x01
# define FPARSELN_UNESCCONT	0x02
# define FPARSELN_UNESCCOMM	0x04
# define FPARSELN_UNESCREST	0x08
# define FPARSELN_UNESCALL	0x0f
char *fparseln(FILE *, size_t *, size_t *, const char [3], int);
#endif

#if !HAVE_ISBLANK && !defined(isblank)
#define isblank(x) ((x) == ' ' || (x) == '\t')
#endif

#if !HAVE_PREAD
ssize_t pread(int, void *, size_t, off_t);
#endif

#if !HAVE_PWCACHE_USERDB
const char *user_from_uid(uid_t, int);
int uid_from_user(const char *, uid_t *);
int pwcache_userdb(int (*)(int), void (*)(void),
		struct passwd * (*)(const char *), struct passwd * (*)(uid_t));
const char *group_from_gid(gid_t, int);
int gid_from_group(const char *, gid_t *);
int pwcache_groupdb(int (*)(int), void (*)(void),
		struct group * (*)(const char *), struct group * (*)(gid_t));
#endif

#if !HAVE_PWRITE
ssize_t pwrite(int, const void *, size_t, off_t);
#endif

#if !HAVE_SETENV
int setenv(const char *, const char *, int);
#endif

#if !HAVE_SETGROUPENT
int setgroupent(int);
#endif

#if !HAVE_SETPASSENT
int setpassent(int);
#endif

#if !HAVE_SETPROGNAME
const char *getprogname(void);
void setprogname(const char *);
#endif

#if !HAVE_SNPRINTF
int snprintf(char *, size_t, const char *, ...);
#endif

#if !HAVE_STRLCAT
size_t strlcat(char *, const char *, size_t);
#endif

#if !HAVE_STRLCPY
size_t strlcpy(char *, const char *, size_t);
#endif

#if !HAVE_STRSEP
char *strsep(char **, const char *);
#endif

#if !HAVE_VASPRINTF
int vasprintf(char **, const char *, va_list);
#endif

#if !HAVE_VASNPRINTF
int vasnprintf(char **, size_t, const char *, va_list);
#endif

#if !HAVE_VSNPRINTF
int vsnprintf(char *, size_t, const char *, va_list);
#endif

/*
 * getmode() and setmode() are always defined, as these function names
 * exist but with very different meanings on other OS's.  The compat
 * versions here simply accept an octal mode number; the "u+x,g-w" type
 * of syntax is not accepted.
 */

#define getmode __nbcompat_getmode
#define setmode __nbcompat_setmode

mode_t getmode(const void *, mode_t);
void *setmode(const char *);

/* Eliminate assertions embedded in binaries. */

#undef _DIAGASSERT
#define _DIAGASSERT(x)
#undef RCSID
#define RCSID(x)

/* Some definitions not available on all systems. */

#ifndef _BSD_VA_LIST_
#define _BSD_VA_LIST_ va_list
#endif

#ifndef _PATH_BSHELL
#if defined(__sun)
/* Sun's /bin/sh is obnoxiously broken. */
#define _PATH_BSHELL "/usr/xpg4/bin/sh"
#else
#define _PATH_BSHELL "/bin/sh"
#endif
#endif
#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "/usr/bin:/bin:/usr/local/bin"
#endif
#ifndef _PATH_DEV
#define _PATH_DEV "/dev/"
#endif
#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL _PATH_DEV "null"
#endif
#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

#ifndef ALLPERMS
#define ALLPERMS (S_ISUID|S_ISGID|S_ISTXT|S_IRWXU|S_IRWXG|S_IRWXO)
#endif

#ifndef EFTYPE
#define EFTYPE EIO
#endif

#ifndef UID_MAX
#define UID_MAX 32767
#endif
#ifndef GID_MAX
#define GID_MAX UID_MAX
#endif

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#ifndef MAXBSIZE
#define MAXBSIZE (64 * 1024)
#endif

#ifndef O_EXLOCK
#define O_EXLOCK 0
#endif
#ifndef O_SHLOCK
#define O_SHLOCK 0
#endif

#ifndef QUAD_MAX
#define QUAD_MAX ((quad_t)(((u_quad_t)-1) >> 1))
#endif

#ifndef S_ISTXT
#ifdef S_ISVTX
#define S_ISTXT S_ISVTX
#else
#define S_ISTXT 0
#endif
#endif

#endif	/* !__NETBSD_COMPAT_DEFS_H__ */
