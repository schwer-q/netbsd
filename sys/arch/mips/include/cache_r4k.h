/*	$NetBSD: cache_r4k.h,v 1.2 2001/11/14 18:26:21 thorpej Exp $	*/

/*
 * Copyright 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Cache definitions/operations for R4000-style caches.
 */

#define	CACHE_R4K_I			0
#define	CACHE_R4K_D			1
#define	CACHE_R4K_SI			2
#define	CACHE_R4K_SD			3

#define	CACHEOP_R4K_INDEX_INV		(0 << 2)	/* I, SI */
#define	CACHEOP_R4K_INDEX_WB_INV	(0 << 2)	/* D, SD */
#define	CACHEOP_R4K_INDEX_LOAD_TAG	(1 << 2)	/* all */
#define	CACHEOP_R4K_INDEX_STORE_TAG	(2 << 2)	/* all */
#define	CACHEOP_R4K_CREATE_DIRTY_EXCL	(3 << 2)	/* D, SD */
#define	CACHEOP_R4K_HIT_INV		(4 << 2)	/* all */
#define	CACHEOP_R4K_HIT_WB_INV		(5 << 2)	/* D, SD */
#define	CACHEOP_R4K_FILL		(5 << 2)	/* I */
#define	CACHEOP_R4K_HIT_WB		(6 << 2)	/* I, D, SD */
#define	CACHEOP_R4K_HIT_SET_VIRTUAL	(7 << 2)	/* SI, SD */

#ifdef _KERNEL

/*
 * cache_r4k_op_line:
 *
 *	Perform the specified cache operation on a single line.
 */
#define	cache_op_r4k_line(va, op)					\
do {									\
	__asm __volatile(						\
		".set noreorder					\n\t"	\
		"cache %1, 0(%0)				\n\t"	\
		".set reorder"						\
	    :								\
	    : "r" (va), "i" (op)					\
	    : "memory");						\
} while (/*CONSTCOND*/0)

/*
 * cache_r4k_op_32lines_16:
 *
 *	Perform the specified cache operation on 32 16-byte
 *	cache lines.
 */
#define	cache_r4k_op_32lines_16(va, op)					\
do {									\
	__asm __volatile(						\
		".set noreorder					\n\t"	\
		"cache %1, 0x000(%0); cache %1, 0x010(%0);	\n\t"	\
		"cache %1, 0x020(%0); cache %1, 0x030(%0);	\n\t"	\
		"cache %1, 0x040(%0); cache %1, 0x050(%0);	\n\t"	\
		"cache %1, 0x060(%0); cache %1, 0x070(%0);	\n\t"	\
		"cache %1, 0x080(%0); cache %1, 0x090(%0);	\n\t"	\
		"cache %1, 0x0a0(%0); cache %1, 0x0b0(%0);	\n\t"	\
		"cache %1, 0x0c0(%0); cache %1, 0x0d0(%0);	\n\t"	\
		"cache %1, 0x0e0(%0); cache %1, 0x0f0(%0);	\n\t"	\
		"cache %1, 0x100(%0); cache %1, 0x110(%0);	\n\t"	\
		"cache %1, 0x120(%0); cache %1, 0x130(%0);	\n\t"	\
		"cache %1, 0x140(%0); cache %1, 0x150(%0);	\n\t"	\
		"cache %1, 0x160(%0); cache %1, 0x170(%0);	\n\t"	\
		"cache %1, 0x180(%0); cache %1, 0x190(%0);	\n\t"	\
		"cache %1, 0x1a0(%0); cache %1, 0x1b0(%0);	\n\t"	\
		"cache %1, 0x1c0(%0); cache %1, 0x1d0(%0);	\n\t"	\
		"cache %1, 0x1e0(%0); cache %1, 0x1f0(%0);	\n\t"	\
		".set reorder"						\
	    :								\
	    : "r" (va), "i" (op)					\
	    : "memory");						\
} while (/*CONSTCOND*/0)

/*
 * cache_r4k_op_32lines_32:
 *
 *	Perform the specified cache operation on 32 32-byte
 *	cache lines.
 */
#define	cache_r4k_op_32lines_32(va, op)					\
do {									\
	__asm __volatile(						\
		".set noreorder					\n\t"	\
		"cache %1, 0x000(%0); cache %1, 0x020(%0);	\n\t"	\
		"cache %1, 0x040(%0); cache %1, 0x060(%0);	\n\t"	\
		"cache %1, 0x080(%0); cache %1, 0x0a0(%0);	\n\t"	\
		"cache %1, 0x0c0(%0); cache %1, 0x0e0(%0);	\n\t"	\
		"cache %1, 0x100(%0); cache %1, 0x120(%0);	\n\t"	\
		"cache %1, 0x140(%0); cache %1, 0x160(%0);	\n\t"	\
		"cache %1, 0x180(%0); cache %1, 0x1a0(%0);	\n\t"	\
		"cache %1, 0x1c0(%0); cache %1, 0x1e0(%0);	\n\t"	\
		"cache %1, 0x200(%0); cache %1, 0x220(%0);	\n\t"	\
		"cache %1, 0x240(%0); cache %1, 0x260(%0);	\n\t"	\
		"cache %1, 0x280(%0); cache %1, 0x2a0(%0);	\n\t"	\
		"cache %1, 0x2c0(%0); cache %1, 0x2e0(%0);	\n\t"	\
		"cache %1, 0x300(%0); cache %1, 0x320(%0);	\n\t"	\
		"cache %1, 0x340(%0); cache %1, 0x360(%0);	\n\t"	\
		"cache %1, 0x380(%0); cache %1, 0x3a0(%0);	\n\t"	\
		"cache %1, 0x3c0(%0); cache %1, 0x3e0(%0);	\n\t"	\
		".set reorder"						\
	    :								\
	    : "r" (va), "i" (op)					\
	    : "memory");						\
} while (/*CONSTCOND*/0)

/*
 * cache_r4k_op_16lines_32_2way:
 *
 *	Perform the specified cache operation on 16 32-byte
 * 	cache lines, 2-ways.
 */
#define	cache_r4k_op_16lines_32_2way(va1, va2, op)			\
do {									\
	__asm __volatile(						\
		".set noreorder					\n\t"	\
		"cache %2, 0x000(%0); cache %2, 0x000(%1);	\n\t"	\
		"cache %2, 0x020(%0); cache %2, 0x020(%1);	\n\t"	\
		"cache %2, 0x040(%0); cache %2, 0x040(%1);	\n\t"	\
		"cache %2, 0x060(%0); cache %2, 0x060(%1);	\n\t"	\
		"cache %2, 0x080(%0); cache %2, 0x080(%1);	\n\t"	\
		"cache %2, 0x0a0(%0); cache %2, 0x0a0(%1);	\n\t"	\
		"cache %2, 0x0c0(%0); cache %2, 0x0c0(%1);	\n\t"	\
		"cache %2, 0x0e0(%0); cache %2, 0x0e0(%1);	\n\t"	\
		"cache %2, 0x100(%0); cache %2, 0x100(%1);	\n\t"	\
		"cache %2, 0x120(%0); cache %2, 0x120(%1);	\n\t"	\
		"cache %2, 0x140(%0); cache %2, 0x140(%1);	\n\t"	\
		"cache %2, 0x160(%0); cache %2, 0x160(%1);	\n\t"	\
		"cache %2, 0x180(%0); cache %2, 0x180(%1);	\n\t"	\
		"cache %2, 0x1a0(%0); cache %2, 0x1a0(%1);	\n\t"	\
		"cache %2, 0x1c0(%0); cache %2, 0x1c0(%1);	\n\t"	\
		"cache %2, 0x1e0(%0); cache %2, 0x1e0(%1);	\n\t"	\
		".set reorder"						\
	    :								\
	    : "r" (va1), "r" (va2), "i" (op)				\
	    : "memory");						\
} while (/*CONSTCOND*/0)

void	r4k_icache_sync_all_16(void);
void	r4k_icache_sync_range_16(vaddr_t, vsize_t);
void	r4k_icache_sync_range_index_16(vaddr_t, vsize_t);

void	r4k_pdcache_wbinv_all_16(void);
void	r4k_pdcache_wbinv_range_16(vaddr_t, vsize_t);
void	r4k_pdcache_wbinv_range_index_16(vaddr_t, vsize_t);

void	r4k_pdcache_inv_range_16(vaddr_t, vsize_t);
void	r4k_pdcache_wb_range_16(vaddr_t, vsize_t);

void	r5k_icache_sync_all_32(void);
void	r5k_icache_sync_range_32(vaddr_t, vsize_t);
void	r5k_icache_sync_range_index_32(vaddr_t, vsize_t);

void	r5k_pdcache_wbinv_all_32(void);
void	r4600v1_pdcache_wbinv_range_32(vaddr_t, vsize_t);
void	r4600v2_pdcache_wbinv_range_32(vaddr_t, vsize_t);
void	r5k_pdcache_wbinv_range_32(vaddr_t, vsize_t);
void	r5k_pdcache_wbinv_range_index_32(vaddr_t, vsize_t);

void	r4600v1_pdcache_inv_range_32(vaddr_t, vsize_t);
void	r4600v2_pdcache_inv_range_32(vaddr_t, vsize_t);
void	r5k_pdcache_inv_range_32(vaddr_t, vsize_t);
void	r4600v1_pdcache_wb_range_32(vaddr_t, vsize_t);
void	r4600v2_pdcache_wb_range_32(vaddr_t, vsize_t);
void	r5k_pdcache_wb_range_32(vaddr_t, vsize_t);

void	r4k_sdcache_wbinv_all_32(void);
void	r4k_sdcache_wbinv_range_32(vaddr_t, vsize_t);
void	r4k_sdcache_wbinv_range_index_32(vaddr_t, vsize_t);

void	r4k_sdcache_inv_range_32(vaddr_t, vsize_t);
void	r4k_sdcache_wb_range_32(vaddr_t, vsize_t);

void	r4k_sdcache_wbinv_all_generic(void);
void	r4k_sdcache_wbinv_range_generic(vaddr_t, vsize_t);
void	r4k_sdcache_wbinv_range_index_generic(vaddr_t, vsize_t);

void	r4k_sdcache_inv_range_generic(vaddr_t, vsize_t);
void	r4k_sdcache_wb_range_generic(vaddr_t, vsize_t);

#endif /* _KERNEL */
