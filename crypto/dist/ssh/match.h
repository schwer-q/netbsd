/*	$NetBSD: match.h,v 1.1.1.6 2001/09/27 02:00:44 itojun Exp $	*/
/*	$OpenBSD: match.h,v 1.9 2001/06/26 06:32:56 itojun Exp $	*/

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
#ifndef MATCH_H
#define MATCH_H

int      match_pattern(const char *, const char *);
int      match_hostname(const char *, const char *, u_int);
int	 match_host_and_ip(const char *, const char *, const char *);
int	 match_user(const char *, const char *, const char *, const char *);
char	*match_list(const char *, const char *, u_int *);

#endif
