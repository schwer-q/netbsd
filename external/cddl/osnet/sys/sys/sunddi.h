/*	$NetBSD: sunddi.h,v 1.4 2010/05/19 18:01:26 haad Exp $	*/

/*-
 * Copyright (c) 2007 Pawel Jakub Dawidek <pjd@FreeBSD.org>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/compat/opensolaris/sys/sunddi.h,v 1.1 2007/04/23 00:52:06 pjd Exp $
 */

#ifndef _OPENSOLARIS_SYS_SUNDDI_H_
#define	_OPENSOLARIS_SYS_SUNDDI_H_

#ifndef _KERNEL
#define	ddi_copyin(from, to, size, flag)	(bcopy((from), (to), (size)), 0)
#define	ddi_copyout(from, to, size, flag)	(bcopy((from), (to), (size)), 0)
#else
#define	ddi_copyin(from, to, size, flag)	(ioctl_copyin((flag), (from), (to), (size)))
#define	ddi_copyout(from, to, size, flag)	(ioctl_copyout((flag), (from), (to), (size)))
#endif
int ddi_strtoul(const char *str, char **nptr, int base, unsigned long *result);

#endif	/* _OPENSOLARIS_SYS_SUNDDI_H_ */
