/*	$NetBSD: cdefs.h,v 1.1 1995/01/19 01:42:35 jtc Exp $	*/

/*
 * Written by J.T. Conklin <jtc@wimsey.com> 01/17/95.
 * Public domain.
 */

#ifndef	_MACHINE_CDEFS_H_
#define	_MACHINE_CDEFS_H_

#ifdef __STDC__
#define _C_LABEL(x)	_STRING(_ ## x)
#else
#define _C_LABEL(x)	_STRING(_/**/x)
#endif

#ifdef __GNUC__
#ifdef __STDC__
#define __weak_reference(sym,alias)	\
	__asm__(".stabs \"_" #alias "\",11,0,0,0");	\
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#define __warn_references(sym,msg)	\
	__asm__(".stabs \"" msg "\",30,0,0,0");		\
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#else
#define __weak_reference(sym,alias)	\
	__asm__(".stabs \"_/**/alias\",11,0,0,0");	\
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#define __warn_references(sym,msg)	\
	__asm__(".stabs msg,30,0,0,0");			\
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#endif
#endif

#endif /* !_MACHINE_CDEFS_H_ */
