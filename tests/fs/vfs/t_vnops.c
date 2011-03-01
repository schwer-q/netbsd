/*	$NetBSD: t_vnops.c,v 1.17 2011/03/01 14:27:32 pooka Exp $	*/

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/stat.h>
#include <sys/statvfs.h>

#include <assert.h>
#include <atf-c.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rump/rump_syscalls.h>
#include <rump/rump.h>

#include "../common/h_fsmacros.h"
#include "../../h_macros.h"

#define TESTFILE "afile"

#define USES_DIRS \
    if (FSTYPE_SYSVBFS(tc)) atf_tc_skip("dirs not supported by file system")

#define USES_SYMLINKS					\
    if (FSTYPE_SYSVBFS(tc) || FSTYPE_MSDOS(tc))		\
	atf_tc_skip("symlinks not supported by file system")

static char *
md(char *buf, const char *base, const char *tail)
{

	sprintf(buf, "%s/%s", base, tail);
	return buf;
}

static void
lookup_simple(const atf_tc_t *tc, const char *mountpath)
{
	char pb[MAXPATHLEN], final[MAXPATHLEN];
	struct stat sb1, sb2;

	strcpy(final, mountpath);
	sprintf(pb, "%s/../%s", mountpath, basename(final));
	if (rump_sys_stat(pb, &sb1) == -1)
		atf_tc_fail_errno("stat 1");

	sprintf(pb, "%s/./../%s", mountpath, basename(final));
	if (rump_sys_stat(pb, &sb2) == -1)
		atf_tc_fail_errno("stat 2");

	ATF_REQUIRE(memcmp(&sb1, &sb2, sizeof(sb1)) == 0);
}

static void
lookup_complex(const atf_tc_t *tc, const char *mountpath)
{
	char pb[MAXPATHLEN];
	struct stat sb1, sb2;

	USES_DIRS;

	sprintf(pb, "%s/dir", mountpath);
	if (rump_sys_mkdir(pb, 0777) == -1)
		atf_tc_fail_errno("mkdir");
	if (rump_sys_stat(pb, &sb1) == -1)
		atf_tc_fail_errno("stat 1");

	sprintf(pb, "%s/./dir/../././dir/.", mountpath);
	if (rump_sys_stat(pb, &sb2) == -1)
		atf_tc_fail_errno("stat 2");

	ATF_REQUIRE(memcmp(&sb1, &sb2, sizeof(sb1)) == 0);
}

static void
dir_simple(const atf_tc_t *tc, const char *mountpath)
{
	char pb[MAXPATHLEN];
	struct stat sb;

	USES_DIRS;

	/* check we can create directories */
	sprintf(pb, "%s/dir", mountpath);
	if (rump_sys_mkdir(pb, 0777) == -1)
		atf_tc_fail_errno("mkdir");
	if (rump_sys_stat(pb, &sb) == -1)
		atf_tc_fail_errno("stat new directory");

	/* check we can remove then and that it makes them unreachable */
	if (rump_sys_rmdir(pb) == -1)
		atf_tc_fail_errno("rmdir");
	if (rump_sys_stat(pb, &sb) != -1 || errno != ENOENT)
		atf_tc_fail("ENOENT expected from stat");
}

static void
dir_notempty(const atf_tc_t *tc, const char *mountpath)
{
	char pb[MAXPATHLEN], pb2[MAXPATHLEN];
	int fd, rv;

	USES_DIRS;

	/* check we can create directories */
	sprintf(pb, "%s/dir", mountpath);
	if (rump_sys_mkdir(pb, 0777) == -1)
		atf_tc_fail_errno("mkdir");

	sprintf(pb2, "%s/dir/file", mountpath);
	fd = rump_sys_open(pb2, O_RDWR | O_CREAT, 0777);
	if (fd == -1)
		atf_tc_fail_errno("create file");
	rump_sys_close(fd);

	rv = rump_sys_rmdir(pb);
	if (rv != -1 || errno != ENOTEMPTY)
		atf_tc_fail("non-empty directory removed succesfully");

	if (rump_sys_unlink(pb2) == -1)
		atf_tc_fail_errno("cannot remove dir/file");

	if (rump_sys_rmdir(pb) == -1)
		atf_tc_fail_errno("remove directory");
}

static void
checkfile(const char *path, struct stat *refp)
{
	char buf[MAXPATHLEN];
	struct stat sb;
	static int n = 1;

	md(buf, path, "file");
	if (rump_sys_stat(buf, &sb) == -1)
		atf_tc_fail_errno("cannot stat file %d (%s)", n, buf);
	if (memcmp(&sb, refp, sizeof(sb)) != 0)
		atf_tc_fail("stat mismatch %d", n);
	n++;
}

static void
rename_dir(const atf_tc_t *tc, const char *mp)
{
	char pb1[MAXPATHLEN], pb2[MAXPATHLEN], pb3[MAXPATHLEN];
	struct stat ref, sb;

	//if (FSTYPE_MSDOS(tc))
		//atf_tc_skip("test fails in some setups, reason unknown");

	if (FSTYPE_RUMPFS(tc))
		atf_tc_skip("rename not supported by fs");

	USES_DIRS;

	md(pb1, mp, "dir1");
	if (rump_sys_mkdir(pb1, 0777) == -1)
		atf_tc_fail_errno("mkdir 1");

	md(pb2, mp, "dir2");
	if (rump_sys_mkdir(pb2, 0777) == -1)
		atf_tc_fail_errno("mkdir 2");
	md(pb2, mp, "dir2/subdir");
	if (rump_sys_mkdir(pb2, 0777) == -1)
		atf_tc_fail_errno("mkdir 3");

	md(pb3, mp, "dir1/file");
	if (rump_sys_mknod(pb3, S_IFREG | 0777, -1) == -1)
		atf_tc_fail_errno("create file");
	if (rump_sys_stat(pb3, &ref) == -1)
		atf_tc_fail_errno("stat of file");

	/*
	 * First try ops which should succeed.
	 */

	/* rename within directory */
	md(pb3, mp, "dir3");
	if (rump_sys_rename(pb1, pb3) == -1)
		atf_tc_fail_errno("rename 1");
	checkfile(pb3, &ref);

	/* rename directory onto itself (two ways, should fail) */
	md(pb1, mp, "dir3/.");
	if (rump_sys_rename(pb1, pb3) != -1 || errno != EINVAL)
		atf_tc_fail_errno("rename 2");
	if (rump_sys_rename(pb3, pb1) != -1 || errno != EISDIR)
		atf_tc_fail_errno("rename 3");

	checkfile(pb3, &ref);

	/* rename father of directory into directory */
	md(pb1, mp, "dir2/dir");
	md(pb2, mp, "dir2");
	if (rump_sys_rename(pb2, pb1) != -1 || errno != EINVAL)
		atf_tc_fail_errno("rename 4");

	/* same for grandfather */
	md(pb1, mp, "dir2/subdir/dir2");
	if (rump_sys_rename(pb2, pb1) != -1 || errno != EINVAL)
		atf_tc_fail("rename 5");

	checkfile(pb3, &ref);

	/* rename directory over a non-empty directory */
	if (rump_sys_rename(pb2, pb3) != -1 || errno != ENOTEMPTY)
		atf_tc_fail("rename 6");

	/* cross-directory rename */
	md(pb1, mp, "dir3");
	md(pb2, mp, "dir2/somedir");
	if (rump_sys_rename(pb1, pb2) == -1)
		atf_tc_fail_errno("rename 7");
	checkfile(pb2, &ref);

	/* move to parent directory */
	md(pb1, mp, "dir2/somedir/../../dir3");
	if (rump_sys_rename(pb2, pb1) == -1)
		atf_tc_fail_errno("rename 8");
	md(pb1, mp, "dir2/../dir3");
	checkfile(pb1, &ref);

	/* atomic cross-directory rename */
	md(pb3, mp, "dir2/subdir");
	if (rump_sys_rename(pb1, pb3) == -1)
		atf_tc_fail_errno("rename 9");
	checkfile(pb3, &ref);

	/* rename directory over an empty directory */
	md(pb1, mp, "parent");
	md(pb2, mp, "parent/dir1");
	md(pb3, mp, "parent/dir2");
	RL(rump_sys_mkdir(pb1, 0777));
	RL(rump_sys_mkdir(pb2, 0777));
	RL(rump_sys_mkdir(pb3, 0777));
	RL(rump_sys_rename(pb2, pb3));

	RL(rump_sys_stat(pb1, &sb));
	ATF_CHECK_EQ(sb.st_nlink, 3);
	RL(rump_sys_rmdir(pb3));
	if (FSTYPE_TMPFS(tc))
		atf_tc_expect_signal(-1, "PR kern/44288");
	RL(rump_sys_rmdir(pb1));
}

static void
rename_dotdot(const atf_tc_t *tc, const char *mp)
{

	if (FSTYPE_RUMPFS(tc))
		atf_tc_skip("rename not supported by fs");

	USES_DIRS;

	if (rump_sys_chdir(mp) == -1)
		atf_tc_fail_errno("chdir mountpoint");

	if (rump_sys_mkdir("dir1", 0777) == -1)
		atf_tc_fail_errno("mkdir 1");
	if (rump_sys_mkdir("dir2", 0777) == -1)
		atf_tc_fail_errno("mkdir 2");

	if (rump_sys_rename("dir1", "dir1/..") != -1 || errno != EINVAL)
		atf_tc_fail_errno("self-dotdot to");

	if (rump_sys_rename("dir1/..", "sometarget") != -1 || errno != EINVAL)
		atf_tc_fail_errno("self-dotdot from");
	atf_tc_expect_pass();

	if (FSTYPE_TMPFS(tc)) {
		atf_tc_expect_fail("PR kern/43617");
	}
	if (rump_sys_rename("dir1", "dir2/..") != -1 || errno != EINVAL)
		atf_tc_fail("other-dotdot");

	rump_sys_chdir("/");
}

static void
rename_reg_nodir(const atf_tc_t *tc, const char *mp)
{
	bool haslinks;
	struct stat sb;
	ino_t f1ino, f2ino;

	if (FSTYPE_RUMPFS(tc))
		atf_tc_skip("rename not supported by fs");

	if (FSTYPE_MSDOS(tc))
		atf_tc_skip("test fails in some setups, reason unknown");

	if (rump_sys_chdir(mp) == -1)
		atf_tc_fail_errno("chdir mountpoint");

	if (FSTYPE_MSDOS(tc) || FSTYPE_SYSVBFS(tc))
		haslinks = false;
	else
		haslinks = true;

	if (rump_sys_mknod("file1", S_IFREG | 0777, -1) == -1)
		atf_tc_fail_errno("create file");
	if (rump_sys_mknod("file2", S_IFREG | 0777, -1) == -1)
		atf_tc_fail_errno("create file");

	if (rump_sys_stat("file1", &sb) == -1)
		atf_tc_fail_errno("stat");
	f1ino = sb.st_ino;

	if (haslinks) {
		if (rump_sys_link("file1", "file_link") == -1)
			atf_tc_fail_errno("link");
		if (rump_sys_stat("file_link", &sb) == -1)
			atf_tc_fail_errno("stat");
		ATF_REQUIRE_EQ(sb.st_ino, f1ino);
		ATF_REQUIRE_EQ(sb.st_nlink, 2);
	}

	if (rump_sys_stat("file2", &sb) == -1)
		atf_tc_fail_errno("stat");
	f2ino = sb.st_ino;

	if (rump_sys_rename("file1", "file3") == -1)
		atf_tc_fail_errno("rename 1");
	if (rump_sys_stat("file3", &sb) == -1)
		atf_tc_fail_errno("stat 1");
	if (haslinks) {
		ATF_REQUIRE_EQ(sb.st_ino, f1ino);
	}
	if (rump_sys_stat("file1", &sb) != -1 || errno != ENOENT)
		atf_tc_fail_errno("source 1");

	if (rump_sys_rename("file3", "file2") == -1)
		atf_tc_fail_errno("rename 2");
	if (rump_sys_stat("file2", &sb) == -1)
		atf_tc_fail_errno("stat 2");
	if (haslinks) {
		ATF_REQUIRE_EQ(sb.st_ino, f1ino);
	}

	if (rump_sys_stat("file3", &sb) != -1 || errno != ENOENT)
		atf_tc_fail_errno("source 2");

	if (haslinks) {
		if (rump_sys_rename("file2", "file_link") == -1)
			atf_tc_fail_errno("rename hardlink");
		if (rump_sys_stat("file2", &sb) != -1 || errno != ENOENT)
			atf_tc_fail_errno("source 3");
		if (rump_sys_stat("file_link", &sb) == -1)
			atf_tc_fail_errno("stat 2");
		ATF_REQUIRE_EQ(sb.st_ino, f1ino);
		ATF_REQUIRE_EQ(sb.st_nlink, 1);
	}

	rump_sys_chdir("/");
}

static void
create_nametoolong(const atf_tc_t *tc, const char *mp)
{
	char *name;
	int fd;
	long val;
	size_t len;

	if (rump_sys_chdir(mp) == -1)
		atf_tc_fail_errno("chdir mountpoint");

	val = rump_sys_pathconf(".", _PC_NAME_MAX);
	if (val == -1)
		atf_tc_fail_errno("pathconf");

	len = val + 1;
	name = malloc(len+1);
	if (name == NULL)
		atf_tc_fail_errno("malloc");

	memset(name, 'a', len);
	*(name+len) = '\0';

	val = rump_sys_pathconf(".", _PC_NO_TRUNC);
	if (val == -1)
		atf_tc_fail_errno("pathconf");

	if (FSTYPE_MSDOS(tc))
		atf_tc_expect_fail("PR kern/43670");
	fd = rump_sys_open(name, O_RDWR|O_CREAT, 0666);
	if (val != 0 && (fd != -1 || errno != ENAMETOOLONG))
		atf_tc_fail_errno("open");

	if (val == 0 && rump_sys_close(fd) == -1)
		atf_tc_fail_errno("close");
	if (val == 0 && rump_sys_unlink(name) == -1)
		atf_tc_fail_errno("unlink");

	free(name);

	rump_sys_chdir("/");
}

static void
create_exist(const atf_tc_t *tc, const char *mp)
{
	const char *name = "hoge";
	int fd;

	RL(rump_sys_chdir(mp));
	RL(fd = rump_sys_open(name, O_RDWR|O_CREAT|O_EXCL, 0666));
	RL(rump_sys_close(fd));
	RL(rump_sys_unlink(name));
	RL(fd = rump_sys_open(name, O_RDWR|O_CREAT, 0666));
	RL(rump_sys_close(fd));
	RL(fd = rump_sys_open(name, O_RDWR|O_CREAT, 0666));
	RL(rump_sys_close(fd));
	ATF_REQUIRE_ERRNO(EEXIST,
	    (fd = rump_sys_open(name, O_RDWR|O_CREAT|O_EXCL, 0666)));
	RL(rump_sys_unlink(name));
	RL(rump_sys_chdir("/"));
}

static void
rename_nametoolong(const atf_tc_t *tc, const char *mp)
{
	char *name;
	int res, fd;
	long val;
	size_t len;

	if (FSTYPE_RUMPFS(tc))
		atf_tc_skip("rename not supported by fs");

	if (rump_sys_chdir(mp) == -1)
		atf_tc_fail_errno("chdir mountpoint");

	val = rump_sys_pathconf(".", _PC_NAME_MAX);
	if (val == -1)
		atf_tc_fail_errno("pathconf");

	len = val + 1;
	name = malloc(len+1);
	if (name == NULL)
		atf_tc_fail_errno("malloc");

	memset(name, 'a', len);
	*(name+len) = '\0';

	fd = rump_sys_open("dummy", O_RDWR|O_CREAT, 0666);
	if (fd == -1)
		atf_tc_fail_errno("open");
	if (rump_sys_close(fd) == -1)
		atf_tc_fail_errno("close");

	val = rump_sys_pathconf(".", _PC_NO_TRUNC);
	if (val == -1)
		atf_tc_fail_errno("pathconf");

	if (FSTYPE_MSDOS(tc))
		atf_tc_expect_fail("PR kern/43670");
	res = rump_sys_rename("dummy", name);
	if (val != 0 && (res != -1 || errno != ENAMETOOLONG))
		atf_tc_fail_errno("rename");

	if (val == 0 && rump_sys_unlink(name) == -1)
		atf_tc_fail_errno("unlink");

	free(name);

	rump_sys_chdir("/");
}

static void
symlink_zerolen(const atf_tc_t *tc, const char *mp)
{

	USES_SYMLINKS;

	RL(rump_sys_chdir(mp));

	if (FSTYPE_TMPFS(tc)) {
		atf_tc_expect_signal(SIGABRT, "PR kern/43843");
	}

	RL(rump_sys_symlink("", "afile"));
	RL(rump_sys_chdir("/"));
}

static void
attrs(const atf_tc_t *tc, const char *mp)
{
	struct stat sb, sb2;
	struct timeval tv[2];
	int fd;

	FSTEST_ENTER();
	RL(fd = rump_sys_open(TESTFILE, O_RDWR | O_CREAT, 0755));
	RL(rump_sys_close(fd));
	RL(rump_sys_stat(TESTFILE, &sb));
	if (!(FSTYPE_MSDOS(tc) || FSTYPE_SYSVBFS(tc))) {
		RL(rump_sys_chown(TESTFILE, 1, 2));
		sb.st_uid = 1;
		sb.st_gid = 2;
		RL(rump_sys_chmod(TESTFILE, 0123));
		sb.st_mode = (sb.st_mode & ~ACCESSPERMS) | 0123;
	}

	tv[0].tv_sec = 1000000000; /* need something >1980 for msdosfs */
	tv[0].tv_usec = 1;
	tv[1].tv_sec = 1000000002; /* need even seconds for msdosfs */
	tv[1].tv_usec = 3;
	RL(rump_sys_utimes(TESTFILE, tv));
	RL(rump_sys_utimes(TESTFILE, tv)); /* XXX: utimes & birthtime */
	sb.st_atimespec.tv_sec = 1000000000;
	sb.st_atimespec.tv_nsec = 1000;
	sb.st_mtimespec.tv_sec = 1000000002;
	sb.st_mtimespec.tv_nsec = 3000;

	RL(rump_sys_stat(TESTFILE, &sb2));
#define CHECK(a) ATF_REQUIRE_EQ(sb.a, sb2.a)
	if (!(FSTYPE_MSDOS(tc) || FSTYPE_SYSVBFS(tc))) {
		CHECK(st_uid);
		CHECK(st_gid);
		CHECK(st_mode);
	}
	if (!FSTYPE_MSDOS(tc)) {
		/* msdosfs has only access date, not time */
		CHECK(st_atimespec.tv_sec);
	}
	CHECK(st_mtimespec.tv_sec);
	if (!(FSTYPE_EXT2FS(tc) || FSTYPE_MSDOS(tc) || FSTYPE_SYSVBFS(tc))) {
		CHECK(st_atimespec.tv_nsec);
		CHECK(st_mtimespec.tv_nsec);
	}
#undef  CHECK

	FSTEST_EXIT();
}

static void
fcntl_lock(const atf_tc_t *tc, const char *mp)
{
	int fd, fd2;
	struct flock l;
	struct lwp *lwp1, *lwp2;

	FSTEST_ENTER();
	l.l_pid = 0;
	l.l_start = l.l_len = 1024;
	l.l_type = F_RDLCK | F_WRLCK;
	l.l_whence = SEEK_END;

	lwp1 = rump_pub_lwproc_curlwp();
	RL(fd = rump_sys_open(TESTFILE, O_RDWR | O_CREAT, 0755));
	RL(rump_sys_ftruncate(fd, 8192));

	/* PR kern/43321 */
	RL(rump_sys_fcntl(fd, F_SETLK, &l));

	/* Next, we fork and try to lock the same area */
	RZ(rump_pub_lwproc_rfork(RUMP_RFCFDG));
	lwp2 = rump_pub_lwproc_curlwp();
	RL(fd2 = rump_sys_open(TESTFILE, O_RDWR, 0));
	ATF_REQUIRE_ERRNO(EAGAIN, rump_sys_fcntl(fd2, F_SETLK, &l));

	/* Switch back and unlock... */
	rump_pub_lwproc_switch(lwp1);
	l.l_type = F_UNLCK;
	RL(rump_sys_fcntl(fd, F_SETLK, &l));

	/* ... and try to lock again */
	rump_pub_lwproc_switch(lwp2);
	l.l_type = F_RDLCK | F_WRLCK;
	RL(rump_sys_fcntl(fd2, F_SETLK, &l));

	RL(rump_sys_close(fd2));
	rump_pub_lwproc_releaselwp();

	RL(rump_sys_close(fd));

	FSTEST_EXIT();
}

static int
flock_compare(const void *p, const void *q)
{
	int a = ((const struct flock *)p)->l_start;
	int b = ((const struct flock *)q)->l_start;
	return a < b ? -1 : (a > b ? 1 : 0);
}

static void
fcntl_getlock_pids(const atf_tc_t *tc, const char *mp)
{
	/* test non-overlaping ranges */
	struct flock expect[4];
	const struct flock lock[4] = {
		{ 0, 2, 0, F_WRLCK, SEEK_SET },
		{ 2, 1, 0, F_WRLCK, SEEK_SET },
		{ 7, 5, 0, F_WRLCK, SEEK_SET },
		{ 4, 3, 0, F_WRLCK, SEEK_SET },
	};

	int fd[4];
	struct lwp *lwp[4];
	pid_t prevpid = 0;

	unsigned int i, j;
	const off_t sz = 8192;
	int omode  = 0755;
	int oflags = O_RDWR | O_CREAT;

	memcpy(expect, lock, sizeof(lock));
	qsort(expect, __arraycount(expect), sizeof(expect[0]), &flock_compare);

	FSTEST_ENTER();

	/*
	 * First, we create 4 processes and let each lock a range of the
	 * file.  Note that the third and fourth processes lock in
	 * "reverse" order, i.e. the greater pid locks a range before
	 * the lesser pid.
	 */
	for(i = 0; i < __arraycount(lwp); i++) {
		RZ(rump_pub_lwproc_rfork(RUMP_RFCFDG));

		lwp[i] = rump_pub_lwproc_curlwp();
		assert(rump_sys_getpid() > prevpid);
		prevpid = rump_sys_getpid();

		RL(fd[i] = rump_sys_open(TESTFILE, oflags, omode));
		oflags = O_RDWR;
		omode  = 0;

		RL(rump_sys_ftruncate(fd[i], sz));
		RL(rump_sys_fcntl(fd[i], F_SETLK, &lock[i]));
	}

	atf_tc_expect_fail("PR kern/44494");
	/*
	 * In the context of each pid , do GETLK for a readlock from
	 * i = [0,__arraycount(locks)].  If we try to lock from the same
	 * start offset as the lock our current process holds, check
	 * that we fail on the offset of the next lock ("else if" branch).
	 * Otherwise, expect to get a lock for the current offset
	 * ("if" branch).  The "else" branch is purely for the last
	 * process where we expect no blocking locks.
	 */
	for(i = 0; i < __arraycount(lwp); i++) {
		rump_pub_lwproc_switch(lwp[i]);

		for(j = 0; j < __arraycount(lwp); j++) {
			struct flock l;
			l = expect[j];
			l.l_len = sz;
			l.l_type = F_RDLCK;

			RL(rump_sys_fcntl(fd[i], F_GETLK, &l));

			if(expect[j].l_start != lock[i].l_start) {
				/*
				 * lock set by another process
				 */
				ATF_CHECK(l.l_type != F_UNLCK);
				ATF_CHECK_EQ(l.l_start, expect[j].l_start);
				ATF_CHECK_EQ(l.l_len,   expect[j].l_len);
			} else if (j != __arraycount(lwp) - 1) {
				/*
				 * lock set by the current process
				 */
				ATF_CHECK(l.l_type != F_UNLCK);
				ATF_CHECK_EQ(l.l_start, expect[j+1].l_start);
				ATF_CHECK_EQ(l.l_len,   expect[j+1].l_len);
			} else {
				/*
				 * there are no other locks after the
				 * current process lock
				 */
				ATF_CHECK_EQ(l.l_type,   F_UNLCK);
				ATF_CHECK_EQ(l.l_start,  expect[j].l_start);
				ATF_CHECK_EQ(l.l_len,    sz);
				ATF_CHECK_EQ(l.l_pid,    expect[j].l_pid);
				ATF_CHECK_EQ(l.l_whence, expect[j].l_whence);
			}
		}
	}

	/*
	 * Release processes.  This also releases the fds and locks
	 * making fs unmount possible
	 */
	for(i = 0; i < __arraycount(lwp); i++) {
		rump_pub_lwproc_switch(lwp[i]);
		rump_pub_lwproc_releaselwp();
	}

	FSTEST_EXIT();
}

static void
access_simple(const atf_tc_t *tc, const char *mp)
{
	int fd;
	int tmode;

	FSTEST_ENTER();
	RL(fd = rump_sys_open("tfile", O_CREAT | O_RDWR, 0777));
	RL(rump_sys_close(fd));

#define ALLACC (F_OK | X_OK | W_OK | R_OK)
	if (FSTYPE_SYSVBFS(tc) || FSTYPE_MSDOS(tc))
		tmode = F_OK;
	else
		tmode = ALLACC;

	RL(rump_sys_access("tfile", tmode));

	/* PR kern/44648 */
	ATF_REQUIRE_ERRNO(EINVAL, rump_sys_access("tfile", ALLACC+1) == -1);
#undef ALLACC
	FSTEST_EXIT();
}

ATF_TC_FSAPPLY(lookup_simple, "simple lookup (./.. on root)");
ATF_TC_FSAPPLY(lookup_complex, "lookup of non-dot entries");
ATF_TC_FSAPPLY(dir_simple, "mkdir/rmdir");
ATF_TC_FSAPPLY(dir_notempty, "non-empty directories cannot be removed");
ATF_TC_FSAPPLY(rename_dir, "exercise various directory renaming ops");
ATF_TC_FSAPPLY(rename_dotdot, "rename dir ..");
ATF_TC_FSAPPLY(rename_reg_nodir, "rename regular files, no subdirectories");
ATF_TC_FSAPPLY(create_nametoolong, "create file with name too long");
ATF_TC_FSAPPLY(create_exist, "create with O_EXCL");
ATF_TC_FSAPPLY(rename_nametoolong, "rename to file with name too long");
ATF_TC_FSAPPLY(symlink_zerolen, "symlink with 0-len target");
ATF_TC_FSAPPLY(attrs, "check setting attributes works");
ATF_TC_FSAPPLY(fcntl_lock, "check fcntl F_SETLK");
ATF_TC_FSAPPLY(fcntl_getlock_pids,"fcntl F_GETLK w/ many procs, PR kern/44494");
ATF_TC_FSAPPLY(access_simple, "access(2)");

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_FSAPPLY(lookup_simple);
	ATF_TP_FSAPPLY(lookup_complex);
	ATF_TP_FSAPPLY(dir_simple);
	ATF_TP_FSAPPLY(dir_notempty);
	ATF_TP_FSAPPLY(rename_dir);
	ATF_TP_FSAPPLY(rename_dotdot);
	ATF_TP_FSAPPLY(rename_reg_nodir);
	ATF_TP_FSAPPLY(create_nametoolong);
	ATF_TP_FSAPPLY(create_exist);
	ATF_TP_FSAPPLY(rename_nametoolong);
	ATF_TP_FSAPPLY(symlink_zerolen);
	ATF_TP_FSAPPLY(attrs);
	ATF_TP_FSAPPLY(fcntl_lock);
	ATF_TP_FSAPPLY(fcntl_getlock_pids);
	ATF_TP_FSAPPLY(access_simple);

	return atf_no_error();
}
