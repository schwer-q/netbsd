/*	$NetBSD: stat.c,v 1.1 2002/04/27 16:37:19 atatat Exp $ */

/*
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Brown.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the NetBSD
 *      Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: stat.c,v 1.1 2002/04/27 16:37:19 atatat Exp $");
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

#define DEF_FORMAT \
	"%d %i %Sp %l %Su %Sg %r %z \"%Sa\" \"%Sm\" \"%Sc\" %k %b %N"
#define RAW_FORMAT	"%d %i %#p %l %u %g %r %z %a %m %c %k %b %N"
#define LS_FORMAT	"%Sp %l %Su %Sg %Z %Sm %N%SY"
#define LSF_FORMAT	"%Sp %l %Su %Sg %Z %Sm %N%T%SY"
#define SHELL_FORMAT \
	"st_dev=%d st_ino=%i st_mode=%#p st_nlink=%l " \
	"st_uid=%u st_gid=%g st_rdev=%r st_size=%z " \
	"st_atimespec=%a st_mtimespec=%m st_ctimespec=%c " \
	"st_blksize=%k st_blocks=%b"
#define LINUX_FORMAT \
	"  File: \"%N\"%n" \
	"  Size: %-11z  FileType: %HT%n" \
	"  Mode: (%04OLp/%.10Sp)         Uid: (%5u/%8Su)  Gid: (%5g/%8Sg)%n" \
	"Device: %Hd,%Ld   Inode: %i    Links: %l%n" \
	"Access: %Sa%n" \
	"Modify: %Sm%n" \
	"Change: %Sc"

#define TIME_FORMAT	"%b %e %T %Y"

#define FMT_POUND	0x01
#define FMT_SPACE	0x02
#define FMT_PLUS	0x04
#define FMT_ZERO	0x08
#define FMT_MINUS	0x10

/*
 * These format characters must all be unique.
 */
#define SIMPLE_NEWLINE	'n'
#define SIMPLE_TAB	't'
#define SIMPLE_PERCENT	'%'

#define FMT_DECIMAL 	'D'
#define FMT_OCTAL 	'O'
#define FMT_UNSIGNED 	'U'
#define FMT_HEX 	'X'
#define FMT_FLOAT 	'F'
#define FMT_STRING 	'S'

#define HIGH_PIECE	'H'
#define MIDDLE_PIECE	'M'
#define LOW_PIECE	'L'

#define SHOW_st_dev	'd'
#define SHOW_st_ino	'i'
#define SHOW_st_mode	'p'
#define SHOW_st_nlink	'l'
#define SHOW_st_uid	'u'
#define SHOW_st_gid	'g'
#define SHOW_st_rdev	'r'
#define SHOW_st_atime	'a'
#define SHOW_st_mtime	'm'
#define SHOW_st_ctime	'c'
#define SHOW_st_size	'z'
#define SHOW_st_blocks	'b'
#define SHOW_st_blksize	'k'
#define SHOW_st_flags	'f'
#define SHOW_st_gen	'v'
#define SHOW_symlink	'Y'
#define SHOW_filetype	'T'
#define SHOW_filename	'N'
#define SHOW_sizerdev	'Z'

void	usage(void);
void	output(const struct stat *, const char *,
	    const char *, int);
int	format1(const struct stat *,	/* stat info */
	    const char *,		/* the file name */
	    const char *, int,		/* the format string itself */
	    char *, size_t,		/* a place to put the output */
	    int, int, int, int,		/* the parsed format */
	    int, int);

char *timefmt;

#define addchar(b, n, l, c, nl) \
	do { \
		if ((*(n)) < (l)) { \
			(b)[(*(n))++] = (c); \
			(*nl) = ((c) == '\n'); \
		} \
	} while (0/*CONSTCOND*/)

int
main(int argc, char *argv[])
{
	struct stat st;
	int ch, rc, errs;
	int lsF, fmtchar, usestat, nonl;
	char *statfmt;

	lsF = 0;
	fmtchar = '\0';
	usestat = 0;
	nonl = 0;
	statfmt = NULL;
	timefmt = NULL;

	while ((ch = getopt(argc, argv, "f:FlLnrst:x")) != -1)
		switch (ch) {
		case 'F':
			lsF = 1;
			break;
		case 'L':
			usestat = 1;
			break;
		case 'n':
			nonl = 1;
			break;
		case 'f':
			statfmt = optarg;
			/* FALLTHROUGH */
		case 'l':
		case 'r':
		case 's':
		case 'x':
			if (fmtchar != 0)
				errx(1, "can't use format '%c' with '%c'",
				    fmtchar, ch);
			fmtchar = ch;
			break;
		case 't':
			timefmt = optarg;
			break;
		default:
			usage();
		}

	argc -= optind;
	argv += optind;

	if (fmtchar == '\0') {
		if (lsF)
			fmtchar = 'l';
		else {
			fmtchar = 'f';
			statfmt = DEF_FORMAT;
		}
	}

	if (lsF && fmtchar != 'l')
		errx(1, "can't use format '%c' with -F", fmtchar);

	switch (fmtchar) {
	case 'f':
		/* statfmt already set */
		break;
	case 'l':
		statfmt = lsF ? LSF_FORMAT : LS_FORMAT;
		break;
	case 'r':
		statfmt = RAW_FORMAT;
		break;
	case 's':
		statfmt = SHELL_FORMAT;
		break;
	case 'x':
		statfmt = LINUX_FORMAT;
		if (timefmt == NULL)
			timefmt = "%c";
		break;
	default:
		usage();
		/*NOTREACHED*/
	}

	if (timefmt == NULL)
		timefmt = TIME_FORMAT;

	errs = 0;
	do {
		if (argc == 0)
			rc = fstat(STDIN_FILENO, &st);
		else if (usestat)
			rc = stat(argv[0], &st);
		else
			rc = lstat(argv[0], &st);

		if (rc == -1) {
			errs = 1;
			warn("%s: stat", argc == 0 ? "(stdin)" : argv[0]);
		}
		else
			output(&st, argv[0], statfmt, nonl);

		argv++;
		argc--;
	} while (argc > 0);

	return (errs);
}

void
usage(void)
{

	(void)fprintf(stderr,
	    "usage: %s [-FlLnrsx] [-f format] [-t timefmt] [file ...]\n",
	    getprogname());
	exit(1);
}

/* 
 * Parses a format string.
 */
void
output(const struct stat *st, const char *file,
    const char *statfmt, int nonl)
{
	int flags, size, prec, ofmt, hilo, what;
	char buf[4096], subbuf[MAXPATHLEN];
	const char *subfmt;
	int nl, t, i;
	size_t len;

	len = 0;
	while (*statfmt != '\0') {

		/*
		 * Non-format characters go straight out.
		 */
		if (*statfmt != '%') {
			addchar(buf, &len, sizeof(buf), *statfmt, &nl);
			statfmt++;
			continue;
		}

		/*
		 * The current format "substring" starts here,
		 * and then we skip the '%'.
		 */
		subfmt = statfmt;
		statfmt++;

		/*
		 * Some simple one-character "formats".
		 */
		switch (*statfmt) {
		case SIMPLE_NEWLINE:
			addchar(buf, &len, sizeof(buf), '\n', &nl);
			statfmt++;
			continue;
		case SIMPLE_TAB:
			addchar(buf, &len, sizeof(buf), '\t', &nl);
			statfmt++;
			continue;
		case SIMPLE_PERCENT:
			addchar(buf, &len, sizeof(buf), '%', &nl);
			statfmt++;
			continue;
		}

		/*
		 * This must be an actual format string.  Format strings are
		 * similar to printf(3) formats up to a point, and are of
		 * the form:
		 *
		 *	%	required start of format
		 *	[-# +0]	opt. format characters
		 *	size	opt. field width
		 *	.	opt. decimal separator, followed by
		 *	prec	opt. precision
		 *	fmt	opt. output specifier (string, numeric, etc.)
		 *	sub	opt. sub field specifier (high, middle, low)
		 *	datum	required field specifier (size, mode, etc)
		 *
		 * Only the % and the datum selector are required.  All data
		 * have reasonable default output forms.  The "sub" specifier
		 * only applies to certain data (mode, dev, rdev, filetype).
		 * The symlink output defaults to STRING, yet will only emit
		 * the leading " -> " if STRING is explicitly specified.  The
		 * sizerdev datum will generate rdev output for character or
		 * block devices, and size output for all others.
		 */
		flags = 0;
		do {
			if      (*statfmt == '#')
				flags |= FMT_POUND;
			else if (*statfmt == ' ')
				flags |= FMT_SPACE;
			else if (*statfmt == '+')
				flags |= FMT_PLUS;
			else if (*statfmt == '0')
				flags |= FMT_ZERO;
			else if (*statfmt == '-')
				flags |= FMT_MINUS;
			else
				break;
			statfmt++;
		} while (1/*CONSTCOND*/);

		size = -1;
		if (isdigit((unsigned)*statfmt)) {
			size = 0;
			while (isdigit((unsigned)*statfmt)) {
				size = (size * 10) + (*statfmt - '0');
				statfmt++;
				if (size < 0)
					goto badfmt;
			}
		}

		prec = -1;
		if (*statfmt == '.') {
			statfmt++;

			prec = 0;
			while (isdigit((unsigned)*statfmt)) {
				prec = (prec * 10) + (*statfmt - '0');
				statfmt++;
				if (prec < 0)
					goto badfmt;
			}
		}

#define fmtcase(x, y) case (y): (x) = (y); statfmt++; break
		switch (*statfmt) {
			fmtcase(ofmt, FMT_DECIMAL);
			fmtcase(ofmt, FMT_OCTAL);
			fmtcase(ofmt, FMT_UNSIGNED);
			fmtcase(ofmt, FMT_HEX);
			fmtcase(ofmt, FMT_FLOAT);
			fmtcase(ofmt, FMT_STRING);
		default:
			ofmt = 0;
			break;
		}

		switch (*statfmt) {
			fmtcase(hilo, HIGH_PIECE);
			fmtcase(hilo, MIDDLE_PIECE);
			fmtcase(hilo, LOW_PIECE);
		default:
			hilo = 0;
			break;
		}

		switch (*statfmt) {
			fmtcase(what, SHOW_st_dev);
			fmtcase(what, SHOW_st_ino);
			fmtcase(what, SHOW_st_mode);
			fmtcase(what, SHOW_st_nlink);
			fmtcase(what, SHOW_st_uid);
			fmtcase(what, SHOW_st_gid);
			fmtcase(what, SHOW_st_rdev);
			fmtcase(what, SHOW_st_atime);
			fmtcase(what, SHOW_st_mtime);
			fmtcase(what, SHOW_st_ctime);
			fmtcase(what, SHOW_st_size);
			fmtcase(what, SHOW_st_blocks);
			fmtcase(what, SHOW_st_blksize);
			fmtcase(what, SHOW_st_flags);
			fmtcase(what, SHOW_st_gen);
			fmtcase(what, SHOW_symlink);
			fmtcase(what, SHOW_filetype);
			fmtcase(what, SHOW_filename);
			fmtcase(what, SHOW_sizerdev);
		default:
			goto badfmt;
		}
#undef fmtcase

		t = format1(st,
		     file,
		     subfmt, statfmt - subfmt,
		     subbuf, sizeof(subbuf),
		     flags, size, prec, ofmt, hilo, what);

		for (i = 0; i < t && i < sizeof(subbuf); i++)
			addchar(buf, &len, sizeof(buf), subbuf[i], &nl);

		continue;

	badfmt:
		errx(1, "%.*s: bad format",
		    (int)(statfmt - subfmt + 1), subfmt);
	}

	(void)write(STDOUT_FILENO, buf, len);
	if (!nl && !nonl)
		(void)write(STDOUT_FILENO, "\n", sizeof("\n"));
}

/*
 * Arranges output according to a single parsed format substring.
 */
int
format1(const struct stat *st,
    const char *file,
    const char *fmt, int flen,
    char *buf, size_t blen,
    int flags, int size, int prec, int ofmt,
    int hilo, int what)
{
	u_int64_t data;
	char *sdata, lfmt[24], tmp[20];
	char smode[12], sid[12], path[MAXPATHLEN + 4];
	struct passwd *pw;
	struct group *gr;
	const struct timespec *tsp;
	struct timespec ts;
	struct tm *tm;
	int l, small, formats;

	tsp = NULL;
	formats = 0;
	small = 0;

	/*
	 * First, pick out the data and tweak it based on hilo or
	 * specified output format (symlink output only).
	 */
	switch (what) {
	case SHOW_st_dev:
	case SHOW_st_rdev:
		small = (sizeof(st->st_dev) == 4);
		data = (what == SHOW_st_dev) ? st->st_dev : st->st_rdev;
		sdata = (what == SHOW_st_dev) ?
		    devname(st->st_dev, S_IFBLK) :
		    devname(st->st_rdev, 
		    S_ISCHR(st->st_mode) ? S_IFCHR :
		    S_ISBLK(st->st_mode) ? S_IFBLK :
		    0U);
		if (hilo == HIGH_PIECE) {
			data = major(data);
			hilo = 0;
		}
		else if (hilo == LOW_PIECE) {
			data = minor((unsigned)data);
			hilo = 0;
		}
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX |
		    FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_ino:
		small = (sizeof(st->st_ino) == 4);
		data = st->st_ino;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_mode:
		small = (sizeof(st->st_mode) == 4);
		data = st->st_mode;
		strmode(st->st_mode, smode);
		sdata = smode;
		l = strlen(sdata);
		if (sdata[l - 1] == ' ')
			sdata[--l] = '\0';
		if (hilo == HIGH_PIECE) {
			data >>= 12;
			sdata += 1;
			sdata[3] = '\0';
			hilo = 0;
		}
		else if (hilo == MIDDLE_PIECE) {
			data &= 07777;
			sdata += 4;
			sdata[3] = '\0';
			hilo = 0;
		}
		else if (hilo == LOW_PIECE) {
			data &= 07777;
			sdata += 7;
			sdata[3] = '\0';
			hilo = 0;
		}
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX |
		    FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_OCTAL;
		break;
	case SHOW_st_nlink:
		small = (sizeof(st->st_dev) == 4);
		data = st->st_nlink;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_uid:
		small = (sizeof(st->st_uid) == 4);
		data = st->st_uid;
		if ((pw = getpwuid(st->st_uid)) != NULL)
			sdata = pw->pw_name;
		else {
			snprintf(sid, sizeof(sid), "(%ld)", (long)st->st_uid);
			sdata = sid;
		}
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX |
		    FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_gid:
		small = (sizeof(st->st_gid) == 4);
		data = st->st_gid;
		if ((gr = getgrgid(st->st_gid)) != NULL)
			sdata = gr->gr_name;
		else {
			snprintf(sid, sizeof(sid), "(%ld)", (long)st->st_gid);
			sdata = sid;
		}
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX |
		    FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_atime:
		tsp = &st->st_atimespec;
		/* FALLTHROUGH */
	case SHOW_st_mtime:
		if (tsp == NULL)
			tsp = &st->st_mtimespec;
		/* FALLTHROUGH */
	case SHOW_st_ctime:
		if (tsp == NULL)
			tsp = &st->st_ctimespec;
		ts = *tsp;		/* copy so we can muck with it */
		small = (sizeof(ts.tv_sec) == 4);
		data = ts.tv_sec;
		small = 1;
		tm = localtime(&ts.tv_sec);
		(void)strftime(path, sizeof(path), timefmt, tm);
		sdata = path;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX |
		    FMT_FLOAT | FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_DECIMAL;
		break;
	case SHOW_st_size:
		small = (sizeof(st->st_size) == 4);
		data = st->st_size;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_blocks:
		small = (sizeof(st->st_blocks) == 4);
		data = st->st_blocks;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_blksize:
		small = (sizeof(st->st_blksize) == 4);
		data = st->st_blksize;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_flags:
		small = (sizeof(st->st_flags) == 4);
		data = st->st_flags;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_st_gen:
		small = (sizeof(st->st_gen) == 4);
		data = st->st_gen;
		sdata = NULL;
		formats = FMT_DECIMAL | FMT_OCTAL | FMT_UNSIGNED | FMT_HEX;
		if (ofmt == 0)
			ofmt = FMT_UNSIGNED;
		break;
	case SHOW_symlink:
		small = 0;
		data = 0;
		if (S_ISLNK(st->st_mode)) {
			snprintf(path, sizeof(path), " -> ");
			l = readlink(file, path + 4, sizeof(path) - 4);
			if (l == -1) {
				l = 0;
				path[0] = '\0';
			}
			path[l + 4] = '\0';
			sdata = path + (ofmt == FMT_STRING ? 0 : 4);
		}
		else
			sdata = "";
		formats = FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_STRING;
		break;
	case SHOW_filetype:
		small = 0;
		data = 0;
		sdata = smode;
		sdata[0] = '\0';
		if (hilo == 0 || hilo == LOW_PIECE) {
			switch (st->st_mode & S_IFMT) {
			case S_IFIFO:	(void)strcat(sdata, "|");	break;
			case S_IFDIR:	(void)strcat(sdata, "/");	break;
			case S_IFREG:
				if (st->st_mode &
				    (S_IXUSR | S_IXGRP | S_IXOTH))
					(void)strcat(sdata, "*");
				break;
			case S_IFLNK:	(void)strcat(sdata, "@");	break;
			case S_IFSOCK:	(void)strcat(sdata, "=");	break;
			case S_IFWHT:	(void)strcat(sdata, "%");	break;
			}
			hilo = 0;
		}
		else if (hilo == HIGH_PIECE) {
			switch (st->st_mode & S_IFMT) {
			case S_IFIFO:	sdata = "Fifo File";		break;
			case S_IFCHR:	sdata = "Character Device";	break;
			case S_IFDIR:	sdata = "Directory";		break;
			case S_IFBLK:	sdata = "Block Device";		break;
			case S_IFREG:	sdata = "Regular File";		break;
			case S_IFLNK:	sdata = "Symbolic Link";	break;
			case S_IFSOCK:	sdata = "Socket";		break;
			case S_IFWHT:	sdata = "Whiteout File";	break;
			default:	sdata = "???";			break;
			}
			hilo = 0;
		}
		formats = FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_STRING;
		break;
	case SHOW_filename:
		small = 0;
		data = 0;
		if (file == NULL)
			(void)strncpy(path, "(stdin)", sizeof(path));
		else
			(void)strncpy(path, file, sizeof(path));
		sdata = path;
		formats = FMT_STRING;
		if (ofmt == 0)
			ofmt = FMT_STRING;
		break;
	case SHOW_sizerdev:
		if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
			char majdev[20], mindev[20];
			int l1, l2;

			l1 = format1(st,
			    file,
			    fmt, flen,
			    majdev, sizeof(majdev),
			    flags, size, prec,
			    ofmt, HIGH_PIECE, SHOW_st_rdev);
			l2 = format1(st,
			    file,
			    fmt, flen,
			    mindev, sizeof(mindev),
			    flags, size, prec,
			    ofmt, LOW_PIECE, SHOW_st_rdev);
			return (snprintf(buf, blen, "%.*s,%.*s",
			    l1, majdev, l2, mindev));
		}
		else {
			return (format1(st,
			    file,
			    fmt, flen,
			    buf, blen,
			    flags, size, prec,
			    ofmt, 0, SHOW_st_size));
		}
		/*NOTREACHED*/
	default:
		errx(1, "%.*s: bad format", (int)flen, fmt);
	}

	/*
	 * If a subdatum was specified but not supported, or an output
	 * format was selected that is not supported, that's an error.
	 */
	if (hilo != 0 || (ofmt & formats) == 0)
		errx(1, "%.*s: bad format", (int)flen, fmt);

	/*
	 * Assemble the format string for passing to printf(3).
	 */
	lfmt[0] = '\0';
	(void)strcat(lfmt, "%");
	if (flags & FMT_POUND)
		(void)strcat(lfmt, "#");
	if (flags & FMT_SPACE)
		(void)strcat(lfmt, " ");
	if (flags & FMT_PLUS)
		(void)strcat(lfmt, "+");
	if (flags & FMT_MINUS)
		(void)strcat(lfmt, "-");
	if (flags & FMT_ZERO)
		(void)strcat(lfmt, "0");

	/*
	 * Only the timespecs support the FLOAT output format, and that
	 * requires work that differs from the other formats.
	 */ 
	if (ofmt == FMT_FLOAT) {
		/*
		 * Nothing after the decimal point, so just print seconds.
		 */
		if (prec == 0) {
			if (size != -1) {
				(void)snprintf(tmp, sizeof(tmp), "%d", size);
				(void)strcat(lfmt, tmp);
			}
			(void)strcat(lfmt, "d");
			return (snprintf(buf, blen, lfmt, ts.tv_sec));
		}

		/*
		 * Unspecified precision gets all the precision we have:
		 * 9 digits.
		 */
		if (prec == -1)
			prec = 9;

		/*
		 * Adjust the size for the decimal point and the digits
		 * that will follow.
		 */
		size -= prec + 1;

		/*
		 * Any leftover size that's legitimate will be used.
		 */
		if (size > 0) {
			(void)snprintf(tmp, sizeof(tmp), "%d", size);
			(void)strcat(lfmt, tmp);
		}
		(void)strcat(lfmt, "d");

		/*
		 * The stuff after the decimal point always needs zero
		 * filling.
		 */
		(void)strcat(lfmt, ".%0");

		/*
		 * We can "print" at most nine digits of precision.  The
		 * rest we will pad on at the end.
		 */
		(void)snprintf(tmp, sizeof(tmp), "%dd", prec > 9 ? 9 : prec);
		(void)strcat(lfmt, tmp);

		/*
		 * For precision of less that nine digits, trim off the
		 * less significant figures.
		 */
		for (; prec < 9; prec++)
			ts.tv_nsec /= 10;

		/*
		 * Use the format, and then tack on any zeroes that
		 * might be required to make up the requested precision.
		 */
		l = snprintf(buf, blen, lfmt, ts.tv_sec, ts.tv_nsec);
		for (; prec > 9 && l < blen; prec--, l++)
			(void)strcat(buf, "0");
		return (l);
	}

	/*
	 * Add on size and precision, if specified, to the format.
	 */
	if (size != -1) {
		(void)snprintf(tmp, sizeof(tmp), "%d", size);
		(void)strcat(lfmt, tmp);
	}
	if (prec != -1) {
		(void)snprintf(tmp, sizeof(tmp), ".%d", prec);
		(void)strcat(lfmt, tmp);
	}

	/*
	 * String output uses the temporary sdata.
	 */
	if (ofmt == FMT_STRING) {
		if (sdata == NULL)
			errx(1, "%.*s: bad format", (int)flen, fmt);
		(void)strcat(lfmt, "s");
		return (snprintf(buf, blen, lfmt, sdata));
	}

	/*
	 * Ensure that sign extension does not cause bad looking output
	 * for some forms.
	 */
	if (small && ofmt != FMT_DECIMAL)
		data = (u_int32_t)data;

	/*
	 * The four "numeric" output forms.
	 */
	(void)strcat(lfmt, "ll");
	switch (ofmt) {
	case FMT_DECIMAL:	(void)strcat(lfmt, "d");	break;
	case FMT_OCTAL:		(void)strcat(lfmt, "o");	break;
	case FMT_UNSIGNED:	(void)strcat(lfmt, "u");	break;
	case FMT_HEX:		(void)strcat(lfmt, "x");	break;
	}

	return (snprintf(buf, blen, lfmt, data));
}
