/*	$NetBSD: param.h,v 1.14 2001/08/17 05:49:43 eeh Exp $	*/

/*
 * Copyright (c) 1994, 1995 Jochen Pohl
 * All Rights Reserved.
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
 *      This product includes software developed by Jochen Pohl for
 *	The NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */

/*
 * Minimun size of string buffer. If this is not enough, the buffer
 * is enlarged in steps of STRBLEN bytes.
 */
#define	STRBLEN		256

/*
 * This defines the size of memory blocks which are used to allocate
 * memory in larger chunks.
 */
#define	MBLKSIZ		((size_t)0x4000)

/*
 * Sizes of hash tables
 * Should be a prime. Possible primes are
 * 307, 401, 503, 601, 701, 809, 907, 1009, 1103, 1201, 1301, 1409, 1511.
 *
 * HSHSIZ1	symbol table 1st pass
 * HSHSIZ2	symbol table 2nd pass
 * THSHSIZ2	type table 2nd pass
 */
#define	HSHSIZ1		503
#define HSHSIZ2		1009
#define	THSHSIZ2	1009

/*
 * Should be set to 1 if the difference of two pointers is of type long
 * or the value of sizeof is of type unsigned long.
 */
#if defined(__alpha__)
#define PTRDIFF_IS_LONG		1
#define SIZEOF_IS_ULONG		1
#elif defined(__i386__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__m68k__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__mips__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__ns32k__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__sh3__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__sparc__)
#define PTRDIFF_IS_LONG		1
#define SIZEOF_IS_ULONG		1
#elif defined(__sparc64__)
#define PTRDIFF_IS_LONG		1
#define SIZEOF_IS_ULONG		1
#elif defined(__vax__)
#define PTRDIFF_IS_LONG         0
#define SIZEOF_IS_ULONG         0
#elif defined(__arm__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#elif defined(__powerpc__) || defined(__ppc__)
#define PTRDIFF_IS_LONG		0
#define SIZEOF_IS_ULONG		0
#else
#error unknown machine type
#endif

/*
 * Make sure this matches wchar_t.
 */
#define WCHAR	SHORT

#ifndef __GNUC__
#ifndef lint
#ifndef QUAD_MAX	/* necessary for mkdep */
#define QUAD_MAX	LONG_MAX
#define QUAD_MIN	LONG_MIN
#define UQUAD_MAX	ULONG_MAX
#endif
typedef	long	quad_t;
typedef	u_long	u_quad_t;
#endif
#endif


/*
 * long double only in ANSI C.
 *
 * And the sparc64 long double code generation is broken.
 */
#if !defined(__sparc64__) && defined(__STDC__)
typedef	long double ldbl_t;
#else
typedef	double	ldbl_t;
#endif

/*
 * Some traditional compilers are not able to assign structures.
 */
#ifdef __STDC__
#define STRUCT_ASSIGN(dest, src)	(dest) = (src)
#else
#define STRUCT_ASSIGN(dest, src)	(void)memcpy(&(dest), &(src), \
						     sizeof (dest));
#endif
