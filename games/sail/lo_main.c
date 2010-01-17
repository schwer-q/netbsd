/*	$NetBSD: lo_main.c,v 1.18 2010/01/17 22:56:32 wiz Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)lo_main.c	8.2 (Berkeley) 4/28/95";
#else
__RCSID("$NetBSD: lo_main.c,v 1.18 2010/01/17 22:56:32 wiz Exp $");
#endif
#endif /* not lint */

/*
 * Print out the top ten SAILors
 *
 * -l force a long listing (print out real usernames)
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include "extern.h"
#include "pathnames.h"

static const char *const title[] = {
	"Admiral", "Commodore", "Captain", "Captain",
	"Captain", "Captain", "Captain", "Commander",
	"Commander", "Lieutenant"
};

int
lo_main(void)
{
	FILE *fp;
	char sbuf[32];
	int n = 0, npeople;
	struct passwd *pass;
	struct logs log;
	struct ship *ship;

	if ((fp = fopen(_PATH_LOGFILE, "r")) == 0) {
		err(1, "%s", _PATH_LOGFILE);
	}
	switch (fread(&npeople, sizeof npeople, 1, fp)) {
	case 0:
		printf("Nobody has sailed yet.\n");
		exit(0);
	case 1:
		break;
	default:
		err(1, "%s", _PATH_LOGFILE);
	}
	while (fread(&log, sizeof log, 1, fp) == 1 &&
	       log.l_name[0] != '\0') {
		if (longfmt && (pass = getpwuid(log.l_uid)) != NULL)
			snprintf(sbuf, sizeof(sbuf),
				"%10.10s (%s)", log.l_name, pass->pw_name);
		else
			snprintf(sbuf, sizeof(sbuf),
				"%20.20s", log.l_name);
		ship = &scene[log.l_gamenum].ship[log.l_shipnum];
		printf("%-10s %21s of the %15s %3d points, %5.2f equiv\n",
			title[n++], sbuf, ship->shipname, log.l_netpoints,
			(float) log.l_netpoints / ship->specs->pts);
	}
	fclose(fp);
	printf("\n%d people have played.\n", npeople);
	return 0;
}
