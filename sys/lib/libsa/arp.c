/*	$NetBSD: arp.c,v 1.7 1995/09/11 21:11:36 thorpej Exp $	*/

/*
 * Copyright (c) 1992 Regents of the University of California.
 * All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
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
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 * @(#) Header: arp.c,v 1.5 93/07/15 05:52:26 leres Exp  (LBL)
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>

#include <netinet/if_ether.h>
#include <netinet/in_systm.h>

#include <string.h>

#include "stand.h"
#include "net.h"

/* Cache stuff */
#define ARP_NUM 8			/* need at most 3 arp entries */

static struct arp_list {
	n_long	addr;
	u_char	ea[6];
} arp_list[ARP_NUM] = {
	{ INADDR_BROADCAST, BA }
};
static	int arp_num = 1;

/* Local forwards */
static	size_t arpsend __P((struct iodesc *, void *, size_t));
static	size_t arprecv __P((struct iodesc *, void *, size_t, time_t));

/* Broadcast an ARP packet, asking who has addr on interface d */
u_char *
arpwhohas(d, addr)
	register struct iodesc *d;
	n_long addr;
{
	register int i;
	register struct ether_arp *ah;
	register struct arp_list *al;
	struct {
		u_char header[ETHER_SIZE];
		struct {
			struct ether_arp arp;
			u_char pad[18]; 	/* 60 - sizeof(arp) */
		} data;
	} wbuf;
	struct {
		u_char header[ETHER_SIZE];
		struct {
			struct ether_arp arp;
			u_char pad[24]; 	/* extra space */
		} data;
	} rbuf;

#ifdef ARP_DEBUG
 	if (debug)
 	    printf("arpwhohas: called for %s\n", intoa(addr));
#endif
	/* Try for cached answer first */
	for (i = 0, al = arp_list; i < arp_num; ++i, ++al)
		if (addr == al->addr)
			return (al->ea);

	/* Don't overflow cache */
	if (arp_num > ARP_NUM - 1)
		panic("arpwhohas: overflowed arp_list!");

#ifdef ARP_DEBUG
 	if (debug)
		printf("arpwhohas: not cached\n");
#endif

	bzero((char*)&wbuf.data, sizeof(wbuf.data));
	ah = &wbuf.data.arp;
	ah->arp_hrd = htons(ARPHRD_ETHER);
	ah->arp_pro = htons(ETHERTYPE_IP);
	ah->arp_hln = sizeof(ah->arp_sha); /* hardware address length */
	ah->arp_pln = sizeof(ah->arp_spa); /* protocol address length */
	ah->arp_op = htons(ARPOP_REQUEST);
	MACPY(d->myea, ah->arp_sha);
	bcopy(&d->myip, ah->arp_spa, sizeof(ah->arp_spa));
	bcopy(&addr, ah->arp_tpa, sizeof(ah->arp_tpa));

	/* Store ip address in cache */
	al->addr = addr;

	(void)sendrecv(d,
	    arpsend, &wbuf.data, sizeof(wbuf.data),
	    arprecv, &rbuf.data, sizeof(rbuf.data));

	/* Store ethernet address in cache */
	MACPY(rbuf.data.arp.arp_sha, al->ea);
	++arp_num;

	return (al->ea);
}

static size_t
arpsend(d, pkt, len)
	register struct iodesc *d;
	register void *pkt;
	register size_t len;
{

#ifdef ARP_DEBUG
 	if (debug)
		printf("arpsend: called\n");
#endif

	return (sendether(d, pkt, len, bcea, ETHERTYPE_ARP));
}

/* Returns 0 if this is the packet we're waiting for else -1 (and errno == 0) */
static size_t
arprecv(d, pkt, len, tleft)
	register struct iodesc *d;
	register void *pkt;
	register size_t len;
	time_t tleft;
{
	register struct ether_arp *ah;
	u_int16_t etype;	/* host order */

#ifdef ARP_DEBUG
 	if (debug)
		printf("arprecv: ");
#endif

	len = readether(d, pkt, len, tleft, &etype);
	if (len == -1 || len < sizeof(struct ether_arp)) {
#ifdef ARP_DEBUG
		if (debug)
			printf("bad len=%d\n", len);
#endif
		goto bad;
	}

	if (etype != ETHERTYPE_ARP) {
#ifdef ARP_DEBUG
		if (debug)
			printf("not arp type=%d\n", etype);
#endif
		goto bad;
	}

	/* Ethernet address now checked in readether() */

	ah = (struct ether_arp *)pkt;
	if (ah->arp_hrd != htons(ARPHRD_ETHER) ||
	    ah->arp_pro != htons(ETHERTYPE_IP) ||
	    ah->arp_hln != sizeof(ah->arp_sha) ||
	    ah->arp_pln != sizeof(ah->arp_spa) )
	{
#ifdef ARP_DEBUG
		if (debug)
			printf("bad hrd/pro/hln/pln\n")
#endif
		goto bad;
	}

	if (ah->arp_op == htons(ARPOP_REQUEST)) {
		arp_reply(d, ah);
		goto bad;
	}

	if (ah->arp_op != htons(ARPOP_REPLY)) {
#ifdef ARP_DEBUG
		if (debug)
			printf("not ARP reply\n");
#endif
		goto bad;
	}

	if (bcmp(&arp_list[arp_num].addr, ah->arp_spa, sizeof(long)) != 0 ||
	    bcmp(&d->myip, ah->arp_tpa, sizeof(d->myip)) != 0) {
#ifdef ARP_DEBUG
		if (debug)
			printf("already cached??\n");
#endif
		goto bad;
	}

#ifdef ARP_DEBUG
 	if (debug)
		printf("cacheing %s --> %s\n",
			   intoa(ah->arp_spa),
			   ether_sprintf(ah->arp_sha));
#endif
	return (0);

bad:
	errno = 0;
	return (-1);
}

/*
 * Convert an ARP request into a reply and send it.
 * Notes:  Re-uses buffer.  Min send length = 60.
 */
void
arp_reply(d, pkt)
	register struct iodesc *d;
	register void *pkt;		/* the request */
{
	struct ether_arp *arp = pkt;

	if (arp->arp_hrd != htons(ARPHRD_ETHER) ||
	    arp->arp_pro != htons(ETHERTYPE_IP) ||
	    arp->arp_hln != sizeof(arp->arp_sha) ||
	    arp->arp_pln != sizeof(arp->arp_spa) )
	{
#ifdef ARP_DEBUG
		if (debug)
			printf("arp_reply: bad hrd/pro/hln/pln\n")
#endif
		return;
	}

	if (arp->arp_op != htons(ARPOP_REQUEST)) {
#ifdef ARP_DEBUG
		if (debug)
			printf("arp_reply: not request!\n");
#endif
		return;
	}

	/* If we are not the target, ignore the request. */
	if (bcmp(arp->arp_tpa, &d->myip, sizeof(arp->arp_tpa)))
		return;

#ifdef ARP_DEBUG
	if (debug) {
		printf("arp_reply: to %s\n",
		       ether_sprintf(arp->arp_sha));
	}
#endif

	arp->arp_op = htons(ARPOP_REPLY);
	/* source becomes target */
	bcopy(arp->arp_sha, arp->arp_tha, sizeof(arp->arp_tha));
	bcopy(arp->arp_spa, arp->arp_tpa, sizeof(arp->arp_tpa));
	/* here becomes source */
	bcopy(d->myea,  arp->arp_sha, sizeof(arp->arp_sha));
	bcopy(&d->myip, arp->arp_spa, sizeof(arp->arp_spa));

	/*
	 * No need to get fancy here.  If the send fails, the
	 * requestor will just ask again.
	 */
	(void) sendether(d, pkt, 60, arp->arp_tha, ETHERTYPE_ARP);
}
