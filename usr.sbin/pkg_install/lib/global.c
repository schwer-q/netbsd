/* $NetBSD: global.c,v 1.2 1997/06/05 12:59:49 agc Exp $ */

#ifndef lint
#if 0
static const char *rcsid = "from FreeBSD Id: global.c,v 1.5 1997/02/22 16:09:48 peter Exp";
#else
static const char *rcsid = "$NetBSD: global.c,v 1.2 1997/06/05 12:59:49 agc Exp $";
#endif
#endif

/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
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
 * Jordan K. Hubbard

 * 18 July 1993
 *
 * Semi-convenient place to stick some needed globals.
 *
 */

#include "lib.h"

/* These are global for all utils */
Boolean	Verbose		= FALSE;
Boolean	Fake		= FALSE;
Boolean	Force		= FALSE;
int AutoAnswer		= FALSE;


