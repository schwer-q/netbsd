/*
 * $NetBSD: main.c,v 1.2 2002/01/26 13:21:10 aymeric Exp $
 *
 *
 * Copyright (c) 1996,1999 Ignatios Souvatzis
 * Copyright (c) 1994 Michael L. Hitch
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
 *      This product includes software developed by Michael L. Hitch.
 * 4. The name of the authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <sys/cdefs.h>
#include <sys/reboot.h>
#include <sys/types.h>

#include <sys/exec_aout.h>

#include <amiga/cfdev.h>
#include <amiga/memlist.h>
#include <include/cpu.h>

#include <saerrno.h>
#include <stand.h>

#include "libstubs.h"
#include "samachdep.h"

#undef __LDPGSZ
#define __LDPGSZ 8192
#define __PGSZ 8192

#define DRACOREVISION	(*(u_int8_t *)0x02000009)
#define DRACOMMUMARGIN	0x200000
#define DRACOZ2OFFSET	0x3000000
#define DRACOZ2MAX	0x1000000

#define EXECMIN 36

/*
 * vers.c (generated by newvers.sh)
 */
extern const char bootprog_rev[];

void startit(void *, void *, void *);

int consclose(void);

extern void *ConsoleBase;

int
pain(void *aio)
{
	long int io = 0;
	caddr_t kp;
	int ksize;
	struct stat sb;

	extern u_int16_t timelimit;

	xdinit(aio);

	if (consinit(NULL))		/* Initialize fresh console */
		return(1);

#ifdef PPCBOOTER
	printf("NetBSD/AmigaPPC " NETBSD_VERS " Primary Bootstrap %s\n", bootprog_rev);
#else
	printf("NetBSD/Amiga " NETBSD_VERS " Primary Bootstrap %s\n", bootprog_rev);
#endif
	io = open("/boot.amiga", 0);	/* Try /boot.amiga first */
	if (io < 0) {
		io = open("/boot", 0);	/* Fallback to /boot */
		if (io < 0) {
			io = open("/boot.ami", 0);	/* 8.3 name? */
			if (io < 0) {
				goto err;
			}
		}
	}

	/* get size of file? */
	if (fstat(io, &sb))
		goto err;
	/* allocate memory for file */
	ksize = sb.st_size;
	if (ksize == 0) {
		printf("Bad size, using 32K\n");	/* XXX debug? */
		ksize = 32 * 1024;
	}
	kp = alloc(ksize);
	if (kp == 0) {
		errno = ENOMEM;
		goto err;
	}
	/* read file into memory */
	if (read(io, kp, ksize) != ksize) {
		errno = ENOEXEC;
		goto freeall;
	}
	/* validate boot: DOS\0 and checksum? */
	if (strcmp(kp, "DOS") != 0 &&
	    (*(u_int32_t *)kp) != 0x424f4f54) {
		errno = ENOEXEC;
		goto freeall;
	}
	/* call boot+12(aio, sysbase); */
	close(io);
	startit(kp, aio, ConsoleBase);
	errno = -1;

freeall:
	free(kp, ksize);
err:
	printf("Error %ld\n", (long)errno);
	close(io);

	timelimit = 10;
	(void)getchar();
	consclose();
	return 1;
}
