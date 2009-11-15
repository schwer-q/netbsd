/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

const char	*version = "version 20070501";

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "awk.h"
#include "awkgram.h"

extern	char	**environ;
extern	int	nfields;

int	dbg	= 0;
char	*cmdname;	/* gets argv[0] for error messages */
extern	FILE	*yyin;	/* lex input file */
char	*lexprog;	/* points to program argument if it exists */
extern	int errorflag;	/* non-zero if any syntax errors; set by yyerror */
int	compile_time = 2;	/* for error printing: */
				/* 2 = cmdline, 1 = compile, 0 = running */

#define	MAX_PFILE	20	/* max number of -f's */

char	*pfile[MAX_PFILE];	/* program filenames from -f's */
int	npfile = 0;	/* number of filenames */
int	curpfile = 0;	/* current filename */

int	safe	= 0;	/* 1 => "safe" mode */

static char *
setfs(char *p)
{
#ifdef notdef
	/* wart: t=>\t */
	if (p[0] == 't' && p[1] == 0)
		return "\t";
	else
#endif
	if (p[0] != 0)
		return p;
	return NULL;
}

static void fpecatch(int n
#ifdef SA_SIGINFO
	, siginfo_t *si, void *uc
#endif
)
{
#ifdef SA_SIGINFO
	static const char *emsg[] = {
	    "Unknown error",
	    "Integer divide by zero",
	    "Integer overflow",
	    "Floating point divide by zero",
	    "Floating point overflow",
	    "Floating point underflow",
	    "Floating point inexact result",
	    "Invalid Floating point operation",
	    "Subscript out of range",
	};
#endif
	FATAL("floating point exception"
#ifdef SA_SIGINFO
	    ": %s\n", emsg[si->si_code >= 1 && si->si_code <= 8 ?
	    si->si_code : 0]
#endif
	    );
}

int main(int argc, char *argv[])
{
	const char *fs = NULL;
	struct lconv *lconv;

	setlocale(LC_CTYPE, "");
	setlocale(LC_NUMERIC, "C"); /* for parsing cmdline & prog */
	cmdname = argv[0];
	if (argc == 1) {
		fprintf(stderr, 
		  "usage: %s [-F fs] [-v var=value] [-f progfile | 'prog'] [file ...]\n", 
		  cmdname);
		exit(1);
	}

	(void) setlocale(LC_ALL, "");
	lconv = localeconv();
	lconv->decimal_point = ".";


#ifdef SA_SIGINFO
	{
		struct sigaction sa;
		sa.sa_sigaction = fpecatch;
		sa.sa_flags = SA_SIGINFO;
		sigemptyset(&sa.sa_mask);
		(void)sigaction(SIGFPE, &sa, NULL);
	}
#else
	(void)signal(SIGFPE, fpecatch);
#endif
	yyin = NULL;
	symtab = makesymtab(NSYMTAB/NSYMTAB);
	while (argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0') {
		if (strcmp(argv[1],"-version") == 0 || strcmp(argv[1],"--version") == 0) {
			printf("awk %s\n", version);
			exit(0);
			break;
		}
		if (strncmp(argv[1], "--", 2) == 0) {	/* explicit end of args */
			argc--;
			argv++;
			break;
		}
		switch (argv[1][1]) {
		case 's':
			if (strcmp(argv[1], "-safe") == 0)
				safe = 1;
			break;
		case 'f':	/* next argument is program filename */
			argc--;
			argv++;
			if (argc <= 1)
				FATAL("no program filename");
			if (npfile >= MAX_PFILE - 1)
				FATAL("too many -f options"); 
			pfile[npfile++] = argv[1];
			break;
		case 'F':	/* set field separator */
			if (argv[1][2] != 0) {	/* arg is -Fsomething */
				fs = setfs(argv[1] + 2);
			} else {		/* arg is -F something */
				argc--; argv++;
				if (argc > 1)
					fs = setfs(argv[1]);
			}
			if (fs == NULL || *fs == '\0')
				WARNING("field separator FS is empty");
			break;
		case 'v':	/* -v a=1 to be done NOW.  one -v for each */
			if (argv[1][2] == '\0' && --argc > 1 && isclvar((++argv)[1]))
				setclvar(argv[1]);
			break;
		case 'd':
			dbg = atoi(&argv[1][2]);
			if (dbg == 0)
				dbg = 1;
			printf("awk %s\n", version);
			break;
		default:
			WARNING("unknown option %s ignored", argv[1]);
			break;
		}
		argc--;
		argv++;
	}
	/* argv[1] is now the first argument */
	if (npfile == 0) {	/* no -f; first argument is program */
		if (argc <= 1) {
			if (dbg)
				exit(0);
			FATAL("no program given");
		}
		   dprintf( ("program = |%s|\n", argv[1]) );
		lexprog = argv[1];
		argc--;
		argv++;
	}
	recinit(recsize);
	syminit();
	compile_time = 1;
	argv[0] = cmdname;	/* put prog name at front of arglist */
	   dprintf( ("argc=%d, argv[0]=%s\n", argc, argv[0]) );
	arginit(argc, argv);
	if (!safe)
		envinit(environ);
	yyparse();
	setlocale(LC_NUMERIC, ""); /* back to whatever it is locally */
	if (fs)
		*FS = qstring(fs, '\0');
	   dprintf( ("errorflag=%d\n", errorflag) );
	if (errorflag == 0) {
		compile_time = 0;
		run(winner);
	} else
		bracecheck();
	return(errorflag);
}

int pgetc(void)		/* get 1 character from awk program */
{
	int c;

	for (;;) {
		if (yyin == NULL) {
			if (curpfile >= npfile)
				return EOF;
			if (strcmp(pfile[curpfile], "-") == 0)
				yyin = stdin;
			else if ((yyin = fopen(pfile[curpfile], "r")) == NULL)
				FATAL("can't open file %s", pfile[curpfile]);
			lineno = 1;
		}
		if ((c = getc(yyin)) != EOF)
			return c;
		if (yyin != stdin)
			fclose(yyin);
		yyin = NULL;
		curpfile++;
	}
}

char *cursource(void)	/* current source file name */
{
	if (npfile > 0)
		return pfile[curpfile];
	else
		return NULL;
}
