/*	$NetBSD: mcontext.h,v 1.3 2003/10/13 23:45:14 christos Exp $	*/

#ifndef _SPARC64_MCONTEXT_H_
#define _SPARC64_MCONTEXT_H_

#include <sparc/mcontext.h>

#define _NGREG32	19	/* %psr, pc, npc, %g1-7, %o0-7 */
typedef	int	__greg32_t;
typedef	__greg32_t	__gregset32_t[_NGREG32];

#define	_REG32_PSR	0
#define	_REG32_PC	1
#define	_REG32_nPC	2
#define	_REG32_Y	3
#define	_REG32_G1	4
#define	_REG32_G2	5
#define	_REG32_G3	6
#define	_REG32_G4	7
#define	_REG32_G5	8
#define	_REG32_G6	9
#define	_REG32_G7	10
#define	_REG32_O0	11
#define	_REG32_O1	12
#define	_REG32_O2	13
#define	_REG32_O3	14
#define	_REG32_O4	15
#define	_REG32_O5	16
#define	_REG32_O6	17
#define	_REG32_O7	18

/* Layout of a register window. */
typedef struct {
	__greg32_t	__rw_local[8];	/* %l0-7 */
	__greg32_t	__rw_in[8];	/* %i0-7 */
} __rwindow32_t;

/* Description of available register windows. */
typedef struct {
	int		__wbcnt;
	__greg32_t *	__spbuf[_SPARC_MAXREGWINDOW];
	__rwindow32_t	__wbuf[_SPARC_MAXREGWINDOW];
} __gwindows32_t;

/* FPU state description */
typedef struct {
	union {
		unsigned int	__fpu_regs[32];
		double		__fpu_dregs[16];
	} __fpu_fr;				/* FPR contents */
	struct __fq *	__fpu_q;		/* pointer to FPU insn queue */
	unsigned int	__fpu_fsr;		/* %fsr */
	unsigned char	__fpu_qcnt;		/* # entries in __fpu_q */
	unsigned char	__fpu_q_entrysize; 	/* size of a __fpu_q entry */
	unsigned char	__fpu_en;		/* this context valid? */
} __fpregset32_t;

typedef struct {
	__gregset32_t	__gregs;	/* GPR state */
	__gwindows32_t *__gwins;	/* may point to register windows */
	__fpregset32_t	__fpregs;	/* FPU state, if any */
	__xrs_t		__xrs;		/* may indicate extra reg state */
} mcontext32_t;

#define _UC_MACHINE32_PAD_32		/* Padding appended to ucontext_t */
#define	_UC_MACHINE32_SP(uc)	((uc)->uc_mcontext.__gregs[_REG32_O6])

#endif /* _SPARC64_MCONTEXT_H_ */
