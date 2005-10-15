/*	$NetBSD: subr_bufq.c,v 1.1 2005/10/15 17:29:26 yamt Exp $	*/
/*	$NetBSD: subr_bufq.c,v 1.1 2005/10/15 17:29:26 yamt Exp $	*/

/*-
 * Copyright (c) 1996, 1997, 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ufs_disksubr.c	8.5 (Berkeley) 1/21/94
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: subr_bufq.c,v 1.1 2005/10/15 17:29:26 yamt Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/bufq.h>
#include <sys/bufq_impl.h>
#include <sys/malloc.h>

BUFQ_DEFINE(dummy, 0, NULL); /* so that bufq_strats won't be empty */

#define	STRAT_MATCH(id, bs)	\
	((id) == BUFQ_STRAT_ANY || strcmp((id), (bs)->bs_name) == 0)

/*
 * Create a device buffer queue.
 */
int
bufq_alloc(struct bufq_state **bufqp, const char *strategy, int flags)
{
	__link_set_decl(bufq_strats, const struct bufq_strat);
	const struct bufq_strat *bsp;
	const struct bufq_strat * const *it;
	struct bufq_state *bufq;
	int error = 0;

	KASSERT((flags & BUFQ_EXACT) == 0 || strategy != BUFQ_STRAT_ANY);

	switch (flags & BUFQ_SORT_MASK) {
	case BUFQ_SORT_RAWBLOCK:
	case BUFQ_SORT_CYLINDER:
		break;
	case 0:
		/*
		 * for strategies which don't care about block numbers.
		 * eg. fcfs
		 */
		flags |= BUFQ_SORT_RAWBLOCK;
		break;
	default:
		panic("bufq_alloc: sort out of range");
	}

	/*
	 * select strategy.
	 * if a strategy specified by flags is found, use it.
	 * otherwise, select one with the largest bs_prio.
	 */
	bsp = NULL;
	__link_set_foreach(it, bufq_strats) {
		if ((*it) == &bufq_strat_dummy)
			continue;
		if (STRAT_MATCH(strategy, (*it))) {
			bsp = *it;
			break;
		}
		if (bsp == NULL || (*it)->bs_prio > bsp->bs_prio)
			bsp = *it;
	}

	if (bsp == NULL) {
		panic("bufq_alloc: no strategy");
	}
	if (!STRAT_MATCH(strategy, bsp)) {
		if ((flags & BUFQ_EXACT)) {
			error = ENOENT;
			goto out;
		}
#if defined(DEBUG)
		printf("bufq_alloc: '%s' is not available. using '%s'.\n",
		    strategy, bsp->bs_name);
#endif
	}
#if defined(BUFQ_DEBUG)
	/* XXX aprint? */
	printf("bufq_alloc: using '%s'\n", bsp->bs_name);
#endif

	*bufqp = bufq = malloc(sizeof(*bufq), M_DEVBUF, M_WAITOK | M_ZERO);
	bufq->bq_flags = flags;
	(*bsp->bs_initfn)(bufq);

out:
	return 0;
}

void
bufq_put(struct bufq_state *bufq, struct buf *bp)
{

	(*bufq->bq_put)(bufq, bp);
}

struct buf *
bufq_get(struct bufq_state *bufq)
{

	return (*bufq->bq_get)(bufq, 1);
}

struct buf *
bufq_peek(struct bufq_state *bufq)
{

	return (*bufq->bq_get)(bufq, 0);
}

/*
 * Drain a device buffer queue.
 */
void
bufq_drain(struct bufq_state *bufq)
{
	struct buf *bp;

	while ((bp = BUFQ_GET(bufq)) != NULL) {
		bp->b_error = EIO;
		bp->b_flags |= B_ERROR;
		bp->b_resid = bp->b_bcount;
		biodone(bp);
	}
}

/*
 * Destroy a device buffer queue.
 */
void
bufq_free(struct bufq_state *bufq)
{

	KASSERT(bufq->bq_private != NULL);
	KASSERT(BUFQ_PEEK(bufq) == NULL);

	free(bufq->bq_private, M_DEVBUF);
	free(bufq, M_DEVBUF);
}
