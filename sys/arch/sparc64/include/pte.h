/*	$NetBSD: pte.h,v 1.21 2009/12/05 22:24:11 mrg Exp $ */

/*
 * Copyright (c) 1996-1999 Eduardo Horvath
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR  ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR  BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _MACHINE_PTE_H_
#define _MACHINE_PTE_H_

#if defined(_KERNEL_OPT)
#include "opt_sparc_arch.h"
#endif

/*
 * Address translation works as follows:
 *
 **
 * For sun4u:
 *	
 *	Take your pick; it's all S/W anyway.  We'll start by emulating a sun4.
 *	Oh, here's the sun4u TTE for reference:
 *
 *	struct sun4u_tte {
 *		uint64	tag_g:1,	(global flag)
 *			tag_reserved:2,	(reserved for future use)
 *			tag_ctxt:13,	(context for mapping)
 *			tag_unassigned:6,
 *			tag_va:42;	(virtual address bits<64:22>)
 *		uint64	data_v:1,	(valid bit)
 *			data_size:2,	(page size [8K*8**<SIZE>])
 *			data_nfo:1,	(no-fault only)
 *			data_ie:1,	(invert endianness [inefficient])
 *			data_soft9:9,	(reserved for S/W)
 *			data_reserved:7,(reserved for future use)
 *			data_pa:30,	(physical address)
 *			data_soft:6,	(reserved for S/W)
 *			data_lock:1,	(lock into TLB)
 *			data_cacheable:2,	(cacheability control)
 *			data_e:1,	(explicit accesses only)
 *			data_priv:1,	(privileged page)
 *			data_w:1,	(writable)
 *			data_g:1;	(same as tag_g)
 *	};	
 */

/* virtual address to virtual page number */
#define	VA_SUN4_VPG(va)		(((int)(va) >> 13) & 31)
#define	VA_SUN4C_VPG(va)	(((int)(va) >> 12) & 63)
#define	VA_SUN4U_VPG(va)	(((int)(va) >> 13) & 31)

/* virtual address to offset within page */
#define VA_SUN4_OFF(va)       	(((int)(va)) & 0x1FFF)
#define VA_SUN4C_OFF(va)     	(((int)(va)) & 0xFFF)
#define VA_SUN4U_OFF(va)       	(((int)(va)) & 0x1FFF)

/* When we go to 64-bit VAs we need to handle the hole */
#define VA_VPG(va)	VA_SUN4U_VPG(va)
#define VA_OFF(va)	VA_SUN4U_OFF(va)

#define PG_SHIFT4U	13
#define MMU_PAGE_ALIGN	8192

/* If you know where a tte is in the tsb, how do you find its va? */	
#define TSBVA(i)	((tsb[(i)].tag.f.tag_va<<22)|(((i)<<13)&0x3ff000))

#ifndef _LOCORE
/* 
 *  This is the spitfire TTE.
 *
 *  We could use bitmasks and shifts to construct this if
 *  we had a 64-bit compiler w/64-bit longs.  Otherwise it's
 *  a real pain to do this in C.
 */
#if 0
/* We don't use bitfeilds anyway. */
struct sun4u_tag_fields {
	uint64_t tag_g:1,	/* global flag */
		tag_reserved:2,	/* reserved for future use */
		tag_ctxt:13,	/* context for mapping */
		tag_unassigned:6,
		tag_va:42;	/* virtual address bits<64:22> */
};
union sun4u_tag { struct sun4u_tag_fields f; int64_t tag; };
struct sun4u_data_fields {
	uint64_t data_v:1,	/* valid bit */
		data_size:2,	/* page size [8K*8**<SIZE>] */
		data_nfo:1,	/* no-fault only */
		data_ie:1,	/* invert endianness [inefficient] */
		data_soft2:9,	/* reserved for S/W */
		data_reserved:7,/* reserved for future use */
		data_pa:30,	/* physical address */
		data_tsblock:1,	/* S/W TSB locked entry */
		data_modified:1,/* S/W modified bit */
		data_realw:1,	/* S/W real writable bit (to manage modified) */
		data_accessed:1,/* S/W accessed bit */
		data_exec:1,	/* S/W Executable */
		data_onlyexec:1,/* S/W Executable only */
		data_lock:1,	/* lock into TLB */
		data_cacheable:2,	/* cacheability control */
		data_e:1,	/* explicit accesses only */
		data_priv:1,	/* privileged page */
		data_w:1,	/* writable */
		data_g:1;	/* same as tag_g */
};
union sun4u_data { struct sun4u_data_fields f; int64_t data; };
struct sun4u_tte {
	union sun4u_tag tag;
	union sun4u_data data;
};
#else
struct sun4u_tte {
	int64_t tag;
	int64_t data;
};
#endif
typedef struct sun4u_tte pte_t;

#endif /* _LOCORE */

/* TSB tag masks */
#define CTX_MASK		((1<<13)-1)
#define TSB_TAG_CTX_SHIFT	48
#define TSB_TAG_VA_SHIFT	22
#define TSB_TAG_G		0x8000000000000000LL

#define TSB_TAG_CTX(t)		((((int64_t)(t))>>TSB_TAG_CTX_SHIFT)&CTX_MASK)
#define TSB_TAG_VA(t)		((((int64_t)(t))<<TSB_TAG_VA_SHIFT))
#define TSB_TAG(g,ctx,va)	((((uint64_t)((g)!=0))<<63)|(((uint64_t)(ctx)&CTX_MASK)<<TSB_TAG_CTX_SHIFT)|(((uint64_t)va)>>TSB_TAG_VA_SHIFT))

/* Page sizes */
#define	PGSZ_8K			0
#define	PGSZ_64K		1
#define	PGSZ_512K		2
#define	PGSZ_4M			3

#define	PGSZ_SHIFT		61
#define	TLB_SZ(s)		(((uint64_t)(s))<<PGSZ_SHIFT)

/* TLB data masks */
#define TLB_V			0x8000000000000000LL
#define TLB_8K			TLB_SZ(PGSZ_8K)
#define TLB_64K			TLB_SZ(PGSZ_64K)
#define TLB_512K		TLB_SZ(PGSZ_512K)
#define TLB_4M			TLB_SZ(PGSZ_4M)
#define TLB_SZ_MASK		0x6000000000000000LL
#define TLB_NFO			0x1000000000000000LL
#define TLB_IE			0x0800000000000000LL
#define TLB_SOFT2_MASK		0x07fc000000000000LL
#define TLB_RESERVED_MASK	0x0003f80000000000LL
#define TLB_PA_MASK		0x000007ffffffe000LL
#define TLB_SOFT_MASK		0x0000000000001f80LL
/* S/W bits */
/* Access & TSB locked bits are swapped so I can set access w/one insn */
/* #define TLB_ACCESS		0x0000000000001000LL */
#define TLB_ACCESS		0x0000000000000200LL
#define TLB_MODIFY		0x0000000000000800LL
#define TLB_REAL_W		0x0000000000000400LL
/* #define TLB_TSB_LOCK		0x0000000000000200LL */
#define TLB_TSB_LOCK		0x0000000000001000LL
#define TLB_EXEC		0x0000000000000100LL
#define TLB_EXEC_ONLY		0x0000000000000080LL
/* H/W bits */
#define TLB_L			0x0000000000000040LL
#define TLB_CACHE_MASK		0x0000000000000030LL
#define TLB_CP			0x0000000000000020LL
#define TLB_CV			0x0000000000000010LL
#define TLB_E			0x0000000000000008LL
#define TLB_P			0x0000000000000004LL
#define TLB_W			0x0000000000000002LL
#define TLB_G			0x0000000000000001LL

/* Use a bit in the SOFT2 area to indicate a locked mapping. */
#define	TLB_WIRED		0x0010000000000000LL

/* 
 * The following bits are used by locore so they should
 * be duplicates of the above w/o the "long long"
 */
/* S/W bits */
/* #define TTE_ACCESS		0x0000000000001000 */
#define TTE_ACCESS		0x0000000000000200
#define TTE_MODIFY		0x0000000000000800
#define TTE_REAL_W		0x0000000000000400
/* #define TTE_TSB_LOCK		0x0000000000000200 */
#define TTE_TSB_LOCK		0x0000000000001000
#define TTE_EXEC		0x0000000000000100
#define TTE_EXEC_ONLY		0x0000000000000080
/* H/W bits */
#define TTE_L			0x0000000000000040
#define TTE_CACHE_MASK		0x0000000000000030
#define TTE_CP			0x0000000000000020
#define TTE_CV			0x0000000000000010
#define TTE_E			0x0000000000000008
#define TTE_P			0x0000000000000004
#define TTE_W			0x0000000000000002
#define TTE_G			0x0000000000000001

#define TTE_DATA_BITS	"\177\20" \
        "b\77V\0" "f\75\2SIZE\0" "b\77V\0" "f\75\2SIZE\0" \
        "=\0008K\0" "=\00164K\0" "=\002512K\0" "=\0034M\0" \
        "b\74NFO\0"     "b\73IE\0"      "f\62\10SOFT2\0" \
        "f\51\10DIAG\0" "f\15\33PA<40:13>\0" "f\7\5SOFT\0" \
        "b\6L\0"        "b\5CP\0"       "b\4CV\0" \
        "b\3E\0"        "b\2P\0"        "b\1W\0"        "b\0G\0"

#define TSB_DATA(g,sz,pa,priv,write,cache,aliased,valid,ie) \
(((valid)?TLB_V:0LL)|TLB_SZ(sz)|(((uint64_t)(pa))&TLB_PA_MASK)|\
((cache)?((aliased)?TLB_CP:TLB_CACHE_MASK):TLB_E)|\
((priv)?TLB_P:0LL)|((write)?TLB_W:0LL)|((g)?TLB_G:0LL)|((ie)?TLB_IE:0LL))

#define MMU_CACHE_VIRT	0x3
#define MMU_CACHE_PHYS	0x2
#define MMU_CACHE_NONE	0x0

/* This needs to be updated for sun4u IOMMUs */
/*
 * IOMMU PTE bits.
 */
#define IOPTE_PPN_MASK  0x07ffff00
#define IOPTE_PPN_SHIFT 8
#define IOPTE_RSVD      0x000000f1
#define IOPTE_WRITE     0x00000004
#define IOPTE_VALID     0x00000002

/*
 * This is purely for compatibility with the old SPARC machines.
 */
#define	NBPRG	(1 << 24)	/* bytes per region */
#define	RGSHIFT	24		/* log2(NBPRG) */
#define NSEGRG	(NBPRG / NBPSG)	/* segments per region */

#define	NBPSG	(1 << 18)	/* bytes per segment */
#define	SGSHIFT	18		/* log2(NBPSG) */

/* there is no `struct pte'; we just use `int'; this is for non-4M only */
#define	PG_V		0x80000000
#define	PG_PFNUM	0x0007ffff	/* n.b.: only 16 bits on sun4c */

/* virtual address to virtual region number */
#define	VA_VREG(va)	(((unsigned int)(va) >> RGSHIFT) & 255)

/* virtual address to virtual segment number */
#define	VA_VSEG(va)	(((unsigned int)(va) >> SGSHIFT) & 63)

#ifndef _LOCORE
typedef u_short pmeg_t;		/* 10 bits needed per Sun-4 segmap entry */
#endif

/*
 * Here are the bit definitions for 4M/SRMMU pte's
 */
		/* MMU TABLE ENTRIES */
#define SRMMU_TETYPE	0x3		/* mask for table entry type */
#define SRMMU_TEPTE	0x2		/* Page Table Entry */
		/* PTE FIELDS */
#define SRMMU_PPNMASK	0xFFFFFF00
#define SRMMU_PPNPASHIFT 0x4 		/* shift to put ppn into PAddr */

#endif /* _MACHINE_PTE_H_ */
