/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)devname.c	5.14 (Berkeley) 5/6/91";
static char rcsid[] = "$Id: devname.c,v 1.2 1993/03/22 08:04:00 cgd Exp $";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <fcntl.h>
#include <db.h>
#include <stdio.h>
#include <paths.h>

char *
devname(dev, type)
	dev_t dev;
	mode_t type;
{
	struct {
		mode_t type;
		dev_t dev;
	} bkey;
	static DB *db;
	static int failure;
	DBT data, key;

	if (!db && !failure &&
	    !(db = hash_open(_PATH_DEVDB, O_RDONLY, 0, NULL))) {
		(void)fprintf(stderr,
		    "warning: no device database %s\n", _PATH_DEVDB);
		failure = 1;
	}
	if (failure)
		return("??");

	/*
	 * Keys are a mode_t followed by a dev_t.  The former is the type of
	 * the file (mode & S_IFMT), the latter is the st_rdev field.
	 */
	bkey.dev = dev;
	bkey.type = type;
	key.data = &bkey;
	key.size = sizeof(bkey);
	return((db->get)(db, &key, &data, 0L) ? "??" : (char *)data.data);
}
