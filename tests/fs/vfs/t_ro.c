/*	$NetBSD: t_ro.c,v 1.2 2010/12/31 18:26:25 pooka Exp $	*/

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

#include <atf-c.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#include <rump/rump_syscalls.h>
#include <rump/rump.h>

#include "../common/h_fsmacros.h"
#include "../../h_macros.h"

#define AFILE "testfile"
#define FUNTEXT "this is some non-humppa text"
#define FUNSIZE (sizeof(FUNTEXT)-1)

static void
nullgen(const atf_tc_t *tc, const char *mp)
{

	return;
}

static void
filegen(const atf_tc_t *tc, const char *mp)
{
	int fd;

	FSTEST_ENTER();
	RL(fd = rump_sys_open(AFILE, O_CREAT | O_RDWR, 0777));
	ATF_REQUIRE_EQ(rump_sys_write(fd, FUNTEXT, FUNSIZE), FUNSIZE);
	RL(rump_sys_close(fd));
	FSTEST_EXIT();
}

/*
 *
 * BEGIN tests
 *
 */

static void
create(const atf_tc_t *tc, const char *mp)
{

	FSTEST_ENTER();
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_open(AFILE, O_CREAT|O_RDONLY) == -1);
	FSTEST_EXIT();
}

static void
rmfile(const atf_tc_t *tc, const char *mp)
{

	FSTEST_ENTER();
	if (FSTYPE_SYSVBFS(tc))
		atf_tc_expect_fail("PR kern/44302");
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_unlink(AFILE) == -1);
	FSTEST_EXIT();
}

static void
fileio(const atf_tc_t *tc, const char *mp)
{
	int fd;
	char buf[FUNSIZE+1];

	FSTEST_ENTER();
	RL(fd = rump_sys_open(AFILE, O_RDONLY));
	ATF_REQUIRE_EQ(rump_sys_read(fd, buf, FUNSIZE), FUNSIZE);
	buf[FUNSIZE] = '\0';
	ATF_REQUIRE_STREQ(buf, FUNTEXT);
	RL(rump_sys_close(fd));

	ATF_REQUIRE_ERRNO(EROFS, rump_sys_open(AFILE, O_WRONLY) == -1);
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_open(AFILE, O_RDWR) == -1);
	FSTEST_EXIT();
}

static void
attrs(const atf_tc_t *tc, const char *mp)
{
	struct timeval sometvs[2];
	struct stat sb;
	int fd;

	FSTEST_ENTER();

	RL(rump_sys_stat(AFILE, &sb));

	ATF_REQUIRE_ERRNO(EROFS, rump_sys_chmod(AFILE, 0775) == -1);
	if (!FSTYPE_MSDOS(tc))
		ATF_REQUIRE_ERRNO(EROFS, rump_sys_chown(AFILE, 1, 1) == -1);
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_utimes(AFILE, sometvs) == -1);

	RL(fd = rump_sys_open(AFILE, O_RDONLY));
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_fchmod(fd, 0775) == -1);
	if (!FSTYPE_MSDOS(tc))
		ATF_REQUIRE_ERRNO(EROFS, rump_sys_fchown(fd, 1, 1) == -1);
	ATF_REQUIRE_ERRNO(EROFS, rump_sys_futimes(fd, sometvs) == -1);
	RL(rump_sys_close(fd));

	FSTEST_EXIT();
}

ATF_TC_FSAPPLY_RO(create, "create file on r/o mount", nullgen);
ATF_TC_FSAPPLY_RO(rmfile, "remove file from r/o mount", filegen);
ATF_TC_FSAPPLY_RO(fileio, "can read a file but not write it", filegen);
ATF_TC_FSAPPLY_RO(attrs, "can query but not change attributes", filegen);

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_FSAPPLY_RO(create);
	ATF_TP_FSAPPLY_RO(rmfile);
	ATF_TP_FSAPPLY_RO(fileio);
	ATF_TP_FSAPPLY_RO(attrs);

	return atf_no_error();
}
