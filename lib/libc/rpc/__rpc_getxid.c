/*	$NetBSD: __rpc_getxid.c,v 1.1 2003/09/09 03:56:23 itojun Exp $	*/
/*	$OpenBSD: ip_id.c,v 1.6 2002/03/15 18:19:52 millert Exp $	*/

/*
 * Copyright 1998 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Theo de Raadt <deraadt@openbsd.org> came up with the idea of using
 * such a mathematical system to generate more random (yet non-repeating)
 * ids to solve the resolver/named problem.  But Niels designed the
 * actual system based on the constraints.
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
 *      This product includes software developed by Niels Provos.
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
 * seed = random 31bit
 * n = prime, g0 = generator to n,
 * j = random so that gcd(j,n-1) == 1
 * g = g0^j mod n will be a generator again.
 *
 * X[0] = random seed.
 * X[n] = a*X[n-1]+b mod m is a Linear Congruential Generator
 * with a = 7^(even random) mod m,
 *      b = random with gcd(b,m) == 1
 *      m = 1836660096 and a maximal period of m-1.
 *
 * The transaction id is determined by:
 * id[n] = seed xor (g^X[n] mod n)
 *
 * Effectivly the id is restricted to the lower 31 bits, thus
 * yielding two different cycles by toggling the msb on and off.
 * This avoids reuse issues caused by reseeding.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: __rpc_getxid.c,v 1.1 2003/09/09 03:56:23 itojun Exp $");
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include "rpc_internal.h"

#define RU_OUT  180		/* Time after wich will be reseeded */
#define RU_MAX	1000000000	/* Uniq cycle, avoid blackjack prediction */
#define RU_GEN	2		/* Starting generator */
#define RU_N	2147483629	/* RU_N-1 = 2^2*3^2*59652323 */
#define RU_AGEN	7		/* determine ru_a as RU_AGEN^(2*rand) */
#define RU_M	1836660096	/* RU_M = 2^7*3^15 - don't change */

#define PFAC_N 3
const static u_int32_t pfacts[PFAC_N] = {
	2,
	3,
	59652323
};

static u_int32_t ru_x;
static u_int32_t ru_seed, ru_seed2;
static u_int32_t ru_a, ru_b;
static u_int32_t ru_g;
static u_int32_t ru_counter = 0;
static u_int32_t ru_msb = 0;
static long ru_reseed;

static u_int32_t pmod(u_int32_t, u_int32_t, u_int32_t);
static void initid(void);

/*
 * Do a fast modular exponation, returned value will be in the range
 * of 0 - (mod-1)
 */
static u_int32_t
pmod(u_int32_t gen, u_int32_t exp, u_int32_t mod)
{
	u_int64_t s, t, u;

	s = 1;
	t = gen;
	u = exp;

	while (u) {
		if (u & 1)
			s = (s * t) % mod;
		u >>= 1;
		t = (t * t) % mod;
	}
	return ((u_int32_t)s & 0xffffffff);
}

/*
 * Initalizes the seed and chooses a suitable generator. Also toggles
 * the msb flag. The msb flag is used to generate two distinct
 * cycles of random numbers and thus avoiding reuse of ids.
 *
 * This function is called from id_randomid() when needed, an
 * application does not have to worry about it.
 */
static void
initid(void)
{
	u_int32_t j, i;
	int noprime = 1;
	struct timeval tv;

	ru_x = arc4random() % RU_M;

	/* 31 bits of random seed */
	ru_seed = arc4random() & INT32_MAX;
	ru_seed2 = arc4random() & INT32_MAX;

	/* Determine the LCG we use */
	ru_b = arc4random() | 1;
	ru_a = pmod(RU_AGEN, arc4random() & (~1U), RU_M);
	while (ru_b % 3 == 0)
		ru_b += 2;

	j = arc4random() % RU_N;

	/*
	 * Do a fast gcd(j,RU_N-1), so we can find a j with
	 * gcd(j, RU_N-1) == 1, giving a new generator for
	 * RU_GEN^j mod RU_N
	 */
	while (noprime) {
		for (i = 0; i < PFAC_N; i++)
			if (j % pfacts[i] == 0)
				break;

		if (i >= PFAC_N)
			noprime = 0;
		else
			j = (j + 1) % RU_N;
	}

	ru_g = pmod(RU_GEN, j, RU_N);
	ru_counter = 0;

	gettimeofday(&tv, NULL);
	ru_reseed = tv.tv_sec + RU_OUT;
	ru_msb = ru_msb ? 0 : 0x80000000;
}

u_int32_t
__rpc_getxid(void)
{
	int i, n;
	u_int32_t tmp;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	if (ru_counter >= RU_MAX || tv.tv_sec > ru_reseed)
		initid();

	tmp = arc4random();

	/* Skip a random number of ids */
	n = tmp & 0x3; tmp = tmp >> 2;
	if (ru_counter + n >= RU_MAX)
		initid();

	for (i = 0; i <= n; i++) {
		/* Linear Congruential Generator */
		ru_x = (ru_a * ru_x + ru_b) % RU_M;
	}

	ru_counter += i;

	return (ru_seed ^ pmod(ru_g, ru_seed2 ^ ru_x,RU_N)) | ru_msb;
}
