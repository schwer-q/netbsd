/*	$KAME: isakmp_inf.c,v 1.63 2000/12/15 13:43:55 sakane Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/param.h>

#include <net/pfkeyv2.h>
#include <netkey/keydb.h>
#include <netkey/key_var.h>
#include <netinet/in.h>
#ifdef IPV6_INRIA_VERSION
#include <sys/queue.h>
#include <netinet/ipsec.h>
#else
#include <netinet6/ipsec.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "libpfkey.h"

#include "var.h"
#include "vmbuf.h"
#include "schedule.h"
#include "str2val.h"
#include "misc.h"
#include "plog.h"
#include "debug.h"

#include "localconf.h"
#include "remoteconf.h"
#include "sockmisc.h"
#include "isakmp_var.h"
#include "isakmp.h"
#include "isakmp_inf.h"
#include "oakley.h"
#include "handler.h"
#include "ipsec_doi.h"
#include "crypto_openssl.h"
#include "pfkey.h"
#include "policy.h"
#include "algorithm.h"
#include "proposal.h"
#include "admin.h"
#include "strnames.h"

/* information exchange */
static int isakmp_info_recv_n __P((struct ph1handle *, vchar_t *));
static int isakmp_info_recv_d __P((struct ph1handle *, vchar_t *));

static void purge_isakmp_spi __P((int, isakmp_index *, size_t));
static void purge_ipsec_spi __P((struct sockaddr *, int, u_int32_t *, size_t));
static void info_recv_initialcontact __P((struct ph1handle *));

/* %%%
 * Information Exchange
 */
/*
 * receive Information
 */
int
isakmp_info_recv(iph1, msg0)
	struct ph1handle *iph1;
	vchar_t *msg0;
{
	vchar_t *msg = NULL;
	int error = -1;
	struct isakmp *isakmp;
	struct isakmp_gen *gen;
	u_int8_t np;
	int encrypted;

	plog(LLV_DEBUG, LOCATION, NULL, "receive Information.\n");

	encrypted = ISSET(((struct isakmp *)msg0->v)->flags, ISAKMP_FLAG_E);

	/* Use new IV to decrypt Informational message. */
	if (encrypted) {

		struct isakmp_ivm *ivm;

		/* compute IV */
		ivm = oakley_newiv2(iph1, ((struct isakmp *)msg0->v)->msgid);
		if (ivm == NULL)
			return -1;

		msg = oakley_do_decrypt(iph1, msg0, ivm->iv, ivm->ive);
		oakley_delivm(ivm);
		if (msg == NULL)
			return -1;

	} else
		msg = vdup(msg0);

	isakmp = (struct isakmp *)msg->v;
	gen = (struct isakmp_gen *)((caddr_t)isakmp + sizeof(struct isakmp));
	if (isakmp->np == ISAKMP_NPTYPE_HASH)
		np = gen->np;
	else
		np = isakmp->np;
		
	/* make sure the packet were encrypted. */
	if (!encrypted) {
		switch (iph1->etype) {
		case ISAKMP_ETYPE_AGG:
		case ISAKMP_ETYPE_BASE:
		case ISAKMP_ETYPE_IDENT:
			if ((iph1->side == INITIATOR && iph1->status < PHASE1ST_MSG3SENT)
			 || (iph1->side == RESPONDER && iph1->status < PHASE1ST_MSG2SENT)) {
				break;
			}
			/*FALLTHRU*/
		default:
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"%s message must be encrypted\n",
				s_isakmp_nptype(np));
			goto end;
		}
	}

	switch (np) {
	case ISAKMP_NPTYPE_N:
		if (isakmp_info_recv_n(iph1, msg) < 0)
			goto end;
		break;
	case ISAKMP_NPTYPE_D:
		if (isakmp_info_recv_d(iph1, msg) < 0)
			goto end;
		break;
	case ISAKMP_NPTYPE_NONCE:
		/* XXX to be 6.4.2 ike-01.txt */
		/* XXX IV is to be synchronized. */
		plog(LLV_ERROR, LOCATION, iph1->remote,
			"ignore Acknowledged Informational\n");
		break;
	default:
		/* don't send information, see isakmp_ident_r1() */
		error = 0;
		plog(LLV_ERROR, LOCATION, iph1->remote,
			"reject the packet, "
			"received unexpecting payload type %d.\n",
			gen->np);
		goto end;
	}

    end:
	if (msg != NULL)
		vfree(msg);

	return 0;
}

/*
 * send Delete payload (for ISAKMP SA) in Informational exchange.
 */
int
isakmp_info_send_d1(iph1)
	struct ph1handle *iph1;
{
	struct isakmp_pl_d *d;
	vchar_t *payload = NULL;
	int tlen;
	int error = 0;

	if (iph1->status != PHASE2ST_ESTABLISHED)
		return 0;

	/* create delete payload */

	/* send SPIs of inbound SAs. */
	/* XXX should send outbound SAs's ? */
	tlen = sizeof(*d) + sizeof(isakmp_index);
	payload = vmalloc(tlen);
	if (payload == NULL) {
		plog(LLV_ERROR, LOCATION, NULL, 
			"failed to get buffer for payload.\n");
		return errno;
	}

	d = (struct isakmp_pl_d *)payload->v;
	d->h.np = ISAKMP_NPTYPE_NONE;
	d->h.len = htons(tlen);
	d->doi = htonl(IPSEC_DOI);
	d->proto_id = IPSECDOI_PROTO_ISAKMP;
	d->spi_size = sizeof(isakmp_index);
	d->num_spi = htons(1);
	memcpy(d + 1, &iph1->index, sizeof(isakmp_index));

	error = isakmp_info_send_common(iph1, payload,
					ISAKMP_NPTYPE_D, 0);
	vfree(payload);

	return error;
}

/*
 * send Delete payload (for IPsec SA) in Informational exchange, based on
 * pfkey msg.  It sends always single SPI.
 */
int
isakmp_info_send_d2(iph2)
	struct ph2handle *iph2;
{
	struct ph1handle *iph1;
	struct saproto *pr;
	struct isakmp_pl_d *d;
	vchar_t *payload = NULL;
	int tlen;
	int error = 0;
	u_int8_t *spi;

	if (iph2->status != PHASE2ST_ESTABLISHED)
		return 0;

	/*
	 * don't send delete information if there is no phase 1 handler.
	 * It's nonsensical to negotiate phase 1 to send the information.
	 */
	iph1 = getph1byaddr(iph2->src, iph2->dst); 
	if (iph1 == NULL)
		return 0;

	/* create delete payload */
	for (pr = iph2->approval->head; pr != NULL; pr = pr->next) {

		/* send SPIs of inbound SAs. */
		/*
		 * XXX should I send outbound SAs's ?
		 * I send inbound SAs's SPI only at the moment because I can't
		 * decode any more if peer send encoded packet without aware of
		 * deletion of SA.  Outbound SAs don't come under the situation.
		 */
		tlen = sizeof(*d) + pr->spisize;
		payload = vmalloc(tlen);
		if (payload == NULL) {
			plog(LLV_ERROR, LOCATION, NULL, 
				"failed to get buffer for payload.\n");
			return errno;
		}

		d = (struct isakmp_pl_d *)payload->v;
		d->h.np = ISAKMP_NPTYPE_NONE;
		d->h.len = htons(tlen);
		d->doi = htonl(IPSEC_DOI);
		d->proto_id = pr->proto_id;
		d->spi_size = pr->spisize;
		d->num_spi = htons(1);
		/*
		 * XXX SPI bits are left-filled, for use with IPComp.
		 * we should be switching to variable-length spi field...
		 */
		spi = (u_int8_t *)&pr->spi;
		spi += sizeof(pr->spi);
		spi -= pr->spisize;
		memcpy(d + 1, spi, pr->spisize);

		error = isakmp_info_send_common(iph1, payload,
						ISAKMP_NPTYPE_D, 0);
		vfree(payload);
	}

	return error;
}

/*
 * send Notification payload (for without ISAKMP SA) in Informational exchange
 */
int
isakmp_info_send_nx(isakmp, remote, local, type, data)
	struct isakmp *isakmp;
	struct sockaddr *remote, *local;
	int type;
	vchar_t *data;
{
	struct ph1handle *iph1 = NULL;
	struct remoteconf *rmconf;
	vchar_t *payload = NULL;
	int tlen;
	int error = -1;
	struct isakmp_pl_n *n;
	int spisiz = 0;		/* see below */

	/* search appropreate configuration */
	rmconf = getrmconf(remote);
	if (rmconf == NULL) {
		plog(LLV_ERROR, LOCATION, remote,
			"no configuration found for peer address.\n");
		goto end;
	}

	/* add new entry to isakmp status table. */
	iph1 = newph1();
	if (iph1 == NULL)
		return -1;

	memcpy(&iph1->index.i_ck, &isakmp->i_ck, sizeof(cookie_t));
	isakmp_newcookie((char *)&iph1->index.r_ck, remote, local);
	iph1->status = PHASE1ST_START;
	iph1->rmconf = rmconf;
	iph1->side = INITIATOR;
	iph1->version = isakmp->v;
	iph1->flags = 0;
	iph1->msgid = 0;	/* XXX */

	/* copy remote address */
	if (copy_ph1addresses(iph1, rmconf, remote, local) < 0)
		return -1;

	tlen = sizeof(*n) + spisiz;
	if (data)
		tlen += data->l;
	payload = vmalloc(tlen);
	if (payload == NULL) { 
		plog(LLV_ERROR, LOCATION, NULL,
			"failed to get buffer to send.\n");
		goto end;
	}

	n = (struct isakmp_pl_n *)payload->v;
	n->h.np = ISAKMP_NPTYPE_NONE;
	n->h.len = htons(tlen);
	n->doi = IPSEC_DOI;
	n->proto_id = IPSECDOI_KEY_IKE;
	n->spi_size = spisiz;
	n->type = htons(type);
	if (spisiz)
		memset(n + 1, 0, spisiz);	/*XXX*/
	if (data)
		memcpy((caddr_t)(n + 1) + spisiz, data->v, data->l);

	error = isakmp_info_send_common(iph1, payload, ISAKMP_NPTYPE_N, 0);
	vfree(payload);

    end:
	if (iph1 != NULL)
		delph1(iph1);

	return error;
}

/*
 * send Notification payload (for ISAKMP SA) in Informational exchange
 */
int
isakmp_info_send_n1(iph1, type, data)
	struct ph1handle *iph1;
	int type;
	vchar_t *data;
{
	vchar_t *payload = NULL;
	int tlen;
	int error = 0;
	struct isakmp_pl_n *n;
	int spisiz;

	/*
	 * note on SPI size: which description is correct?  I have chosen
	 * this to be 0.
	 *
	 * RFC2408 3.1, 2nd paragraph says: ISAKMP SA is identified by
	 * Initiator/Responder cookie and SPI has no meaning, SPI size = 0.
	 * RFC2408 3.1, first paragraph on page 40: ISAKMP SA is identified
	 * by cookie and SPI has no meaning, 0 <= SPI size <= 16.
	 * RFC2407 4.6.3.3, INITIAL-CONTACT is required to set to 16.
	 */
	if (type == ISAKMP_NTYPE_INITIAL_CONTACT)
		spisiz = sizeof(isakmp_index);
	else
		spisiz = 0;

	tlen = sizeof(*n) + spisiz;
	if (data)
		tlen += data->l;
	payload = vmalloc(tlen);
	if (payload == NULL) { 
		plog(LLV_ERROR, LOCATION, NULL,
			"failed to get buffer to send.\n");
		return errno;
	}

	n = (struct isakmp_pl_n *)payload->v;
	n->h.np = ISAKMP_NPTYPE_NONE;
	n->h.len = htons(tlen);
	n->doi = htonl(iph1->rmconf->doitype);
	n->proto_id = IPSECDOI_PROTO_ISAKMP; /* XXX to be configurable ? */
	n->spi_size = spisiz;
	n->type = htons(type);
	if (spisiz)
		memcpy(n + 1, &iph1->index, sizeof(isakmp_index));
	if (data)
		memcpy((caddr_t)(n + 1) + spisiz, data->v, data->l);

	error = isakmp_info_send_common(iph1, payload, ISAKMP_NPTYPE_N, iph1->flags);
	vfree(payload);

	return error;
}

/*
 * send Notification payload (for IPsec SA) in Informational exchange
 */
int
isakmp_info_send_n2(iph2, type, data)
	struct ph2handle *iph2;
	int type;
	vchar_t *data;
{
	struct ph1handle *iph1 = iph2->ph1;
	vchar_t *payload = NULL;
	int tlen;
	int error = 0;
	struct isakmp_pl_n *n;
	struct saproto *pr;

	if (!iph2->approval)
		return EINVAL;

	pr = iph2->approval->head;

	/* XXX must be get proper spi */
	tlen = sizeof(*n) + pr->spisize;
	if (data)
		tlen += data->l;
	payload = vmalloc(tlen);
	if (payload == NULL) { 
		plog(LLV_ERROR, LOCATION, NULL,
			"failed to get buffer to send.\n");
		return errno;
	}

	n = (struct isakmp_pl_n *)payload->v;
	n->h.np = ISAKMP_NPTYPE_NONE;
	n->h.len = htons(tlen);
	n->doi = htonl(IPSEC_DOI);		/* IPSEC DOI (1) */
	n->proto_id = pr->proto_id;		/* IPSEC AH/ESP/whatever*/
	n->spi_size = pr->spisize;
	n->type = htons(type);
	*(u_int32_t *)(n + 1) = pr->spi;
	if (data)
		memcpy((caddr_t)(n + 1) + pr->spisize, data->v, data->l);

	iph2->flags |= ISAKMP_FLAG_E;	/* XXX Should we do FLAG_A ? */
	error = isakmp_info_send_common(iph1, payload, ISAKMP_NPTYPE_N, iph2->flags);
	vfree(payload);

	return error;
}

/*
 * send Information
 * When ph1->skeyid_a == NULL, send message without encoding.
 */
int
isakmp_info_send_common(iph1, payload, np, flags)
	struct ph1handle *iph1;
	vchar_t *payload;
	u_int32_t np;
	int flags;
{
	struct ph2handle *iph2 = NULL;
	vchar_t *hash = NULL;
	struct isakmp *isakmp;
	struct isakmp_gen *gen;
	char *p;
	int tlen;
	int error = -1;

	/* add new entry to isakmp status table */
	iph2 = newph2();
	if (iph2 == NULL)
		goto end;

	iph2->dst = dupsaddr(iph1->remote);
	iph2->src = dupsaddr(iph1->local);
	switch (iph1->remote->sa_family) {
	case AF_INET:
		((struct sockaddr_in *)iph2->dst)->sin_port = 0;
		((struct sockaddr_in *)iph2->src)->sin_port = 0;
		break;
#ifdef INET6
	case AF_INET6:
		((struct sockaddr_in6 *)iph2->dst)->sin6_port = 0;
		((struct sockaddr_in6 *)iph2->src)->sin6_port = 0;
		break;
#endif
	default:
		plog(LLV_ERROR, LOCATION, NULL,
			"invalid family: %d\n", iph1->remote->sa_family);
		delph2(iph2);
		goto end;
	}
	iph2->ph1 = iph1;
	iph2->side = INITIATOR;
	iph2->status = PHASE2ST_START;
	iph2->msgid = isakmp_newmsgid2(iph1);

	/* get IV and HASH(1) if skeyid_a was generated. */
	if (iph1->skeyid_a != NULL) {
		iph2->ivm = oakley_newiv2(iph1, iph2->msgid);
		if (iph2->ivm == NULL) {
			delph2(iph2);
			goto end;
		}

		/* generate HASH(1) */
		hash = oakley_compute_hash1(iph2->ph1, iph2->msgid, payload);
		if (hash == NULL) {
			delph2(iph2);
			goto end;
		}

		/* initialized total buffer length */
		tlen = hash->l;
		tlen += sizeof(*gen);
	} else {
		/* IKE-SA is not established */
		hash = NULL;

		/* initialized total buffer length */
		tlen = 0;
	}
	if ((flags & ISAKMP_FLAG_A) == 0)
		iph2->flags = (hash == NULL ? 0 : ISAKMP_FLAG_E);
	else
		iph2->flags = (hash == NULL ? 0 : ISAKMP_FLAG_A);

	insph2(iph2);
	bindph12(iph1, iph2);

	tlen += sizeof(*isakmp) + payload->l;

	/* create buffer for isakmp payload */
	iph2->sendbuf = vmalloc(tlen);
	if (iph2->sendbuf == NULL) { 
		plog(LLV_ERROR, LOCATION, NULL,
			"failed to get buffer to send.\n");
		goto err;
	}

	/* create isakmp header */
	isakmp = (struct isakmp *)iph2->sendbuf->v;
	memcpy(&isakmp->i_ck, &iph1->index.i_ck, sizeof(cookie_t));
	memcpy(&isakmp->r_ck, &iph1->index.r_ck, sizeof(cookie_t));
	isakmp->np = hash == NULL ? (np & 0xff) : ISAKMP_NPTYPE_HASH;
	isakmp->v = iph1->version;
	isakmp->etype = ISAKMP_ETYPE_INFO;
	isakmp->flags = iph2->flags;
	memcpy(&isakmp->msgid, &iph2->msgid, sizeof(isakmp->msgid));
	isakmp->len   = htonl(tlen);
	p = (char *)(isakmp + 1);

	/* create HASH payload */
	if (hash != NULL) {
		gen = (struct isakmp_gen *)p;
		gen->np = np & 0xff;
		gen->len = htons(sizeof(*gen) + hash->l);
		p += sizeof(*gen);
		memcpy(p, hash->v, hash->l);
		p += hash->l;
	}

	/* add payload */
	memcpy(p, payload->v, payload->l);
	p += payload->l;

#ifdef HAVE_PRINT_ISAKMP_C
	isakmp_printpacket(iph2->sendbuf, iph1->local, iph1->remote, 1);
#endif

	/* encoding */
	if (ISSET(isakmp->flags, ISAKMP_FLAG_E)) {
		vchar_t *tmp;

		tmp = oakley_do_encrypt(iph2->ph1, iph2->sendbuf, iph2->ivm->ive,
				iph2->ivm->iv);
		if (tmp == NULL) {
			vfree(iph2->sendbuf);
			iph2->sendbuf = NULL;
			goto err;
		}
		vfree(iph2->sendbuf);
		iph2->sendbuf = tmp;
	}

	/* HDR*, HASH(1), N */
	if (isakmp_send(iph2->ph1, iph2->sendbuf) < 0) {
		vfree(iph2->sendbuf);
		iph2->sendbuf = NULL;
		goto err;
	}

	plog(LLV_DEBUG, LOCATION, NULL,
		"sendto Information %s.\n", s_isakmp_nptype(np));

	/*
	 * don't resend notify message because peer can use Acknowledged
	 * Informational if peer requires the reply of the notify message.
	 */

	/* XXX If Acknowledged Informational required, don't delete ph2handle */
	error = 0;
	vfree(iph2->sendbuf);
	iph2->sendbuf = NULL;
	goto err;	/* XXX */

end:
	if (hash)
		vfree(hash);
	return error;

err:
	unbindph12(iph2);
	remph2(iph2);
	delph2(iph2);
	goto end;
}

/*
 * add a notify payload to buffer by reallocating buffer.
 * If buf == NULL, the function only create a notify payload.
 *
 * XXX Which is SPI to be included, inbound or outbound ?
 */
vchar_t *
isakmp_add_pl_n(buf0, np_p, type, pr, data)
	vchar_t *buf0;
	u_int8_t **np_p;
	int type;
	struct saproto *pr;
	vchar_t *data;
{
	vchar_t *buf = NULL;
	struct isakmp_pl_n *n;
	int tlen;
	int oldlen = 0;

	if (*np_p)
		**np_p = ISAKMP_NPTYPE_N;

	tlen = sizeof(*n) + pr->spisize;

	if (data)
		tlen += data->l;
	if (buf0) {
		oldlen = buf0->l;
		buf = vrealloc(buf0, buf0->l + tlen);
	} else
		buf = vmalloc(tlen);
	if (!buf) {
		plog(LLV_ERROR, LOCATION, NULL,
			"failed to get a payload buffer.\n");
		return NULL;
	}

	n = (struct isakmp_pl_n *)(buf->v + oldlen);
	n->h.np = ISAKMP_NPTYPE_NONE;
	n->h.len = htons(tlen);
	n->doi = htonl(IPSEC_DOI);		/* IPSEC DOI (1) */
	n->proto_id = pr->proto_id;		/* IPSEC AH/ESP/whatever*/
	n->spi_size = pr->spisize;
	n->type = htons(type);
	*(u_int32_t *)(n + 1) = pr->spi;	/* XXX */
	if (data)
		memcpy((caddr_t)(n + 1) + pr->spisize, data->v, data->l);

	/* save the pointer of next payload type */
	*np_p = &n->h.np;

	return buf;
}

/*
 * handling to receive Notification payload
 */
static int
isakmp_info_recv_n(iph1, msg)
	struct ph1handle *iph1;
	vchar_t *msg;
{
	struct isakmp_pl_n *n = NULL;
	u_int type;
	vchar_t *pbuf;
	struct isakmp_parse_t *pa, *pap;
	char *spi;

	if (!(pbuf = isakmp_parse(msg)))
		return -1;
	pa = (struct isakmp_parse_t *)pbuf->v;
	for (pap = pa; pap->type; pap++) {
		switch (pap->type) {
		case ISAKMP_NPTYPE_HASH:
			/* do something here */
			break;
		case ISAKMP_NPTYPE_NONCE:
			/* send to ack */
			break;
		case ISAKMP_NPTYPE_N:
			n = (struct isakmp_pl_n *)pap->ptr;
			break;
		default:
			vfree(pbuf);
			return -1;
		}
	}
	vfree(pbuf);
	if (!n)
		return -1;

	type = ntohs(n->type);

	switch (type) {
	case ISAKMP_NTYPE_CONNECTED:
	case ISAKMP_NTYPE_RESPONDER_LIFETIME:
	case ISAKMP_NTYPE_REPLAY_STATUS:
		/* do something */
		break;
	case ISAKMP_NTYPE_INITIAL_CONTACT:
		info_recv_initialcontact(iph1);
		break;
	default:
	    {
		u_int32_t msgid = ((struct isakmp *)msg->v)->msgid;
		struct ph2handle *iph2;

		/* XXX there is a potential of dos attack. */
		if (msgid == 0) {
			/* delete ph1 */
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"delete phase1 handle.\n");
			remph1(iph1);
			delph1(iph1);
		} else {
			iph2 = getph2bymsgid(iph1, msgid);
			if (iph2 == NULL) {
				plog(LLV_ERROR, LOCATION, iph1->remote,
					"unknown notify message, "
					"no phase2 handle found.\n");
			} else {
				/* delete ph2 */
				unbindph12(iph2);
				remph2(iph2);
				delph2(iph2);
			}
		}
	    }
		break;
	}

	/* get spi and allocate */
	if (ntohs(n->h.len) != sizeof(*n) + n->spi_size) {
		plog(LLV_ERROR, LOCATION, iph1->remote,
			"invalid spi_size in notification payload.\n");
	}
	spi = val2str((u_char *)(n + 1), n->spi_size);

	plog(LLV_DEBUG, LOCATION, iph1->remote,
		"notification message %d:%s, "
		"doi=%d proto_id=%d spi=%s(size=%d).\n",
		type, s_isakmp_notify_msg(type),
		ntohl(n->doi), n->proto_id, spi, n->spi_size);

	free(spi);

	return(0);
}

static void
purge_isakmp_spi(proto, spi, n)
	int proto;
	isakmp_index *spi;	/*network byteorder*/
	size_t n;
{
	struct ph1handle *iph1;
	size_t i;

	for (i = 0; i < n; i++) {
		iph1 = getph1byindex(&spi[i]);
		if (!iph1)
			continue;

		plog(LLV_INFO, LOCATION, NULL,
			"proto_id %s purging spi:%s.\n",
			s_ipsecdoi_proto(proto),
			isakmp_pindex(&spi[i], 0));

		if (iph1->sce)
			SCHED_KILL(iph1->sce);
		iph1->status = PHASE1ST_EXPIRED;
		iph1->sce = sched_new(1, isakmp_ph1delete_stub, iph1);
	}
}

static void
purge_ipsec_spi(dst0, proto, spi, n)
	struct sockaddr *dst0;
	int proto;
	u_int32_t *spi;	/*network byteorder*/
	size_t n;
{
	vchar_t *buf = NULL;
	struct sadb_msg *msg, *next, *end;
	struct sadb_sa *sa;
	struct sockaddr *src, *dst;
	struct ph2handle *iph2;
	size_t i;
	caddr_t mhp[SADB_EXT_MAX + 1];

	buf = pfkey_dump_sadb(ipsecdoi2pfkey_proto(proto));
	if (buf == NULL) {
		plog(LLV_DEBUG, LOCATION, NULL,
			"pfkey_dump_sadb returned nothing.\n");
		return;
	}

	msg = (struct sadb_msg *)buf->v;
	end = (struct sadb_msg *)(buf->v + buf->l);

	while (msg < end) {
		if ((msg->sadb_msg_len << 3) < sizeof(*msg))
			break;
		next = (struct sadb_msg *)((caddr_t)msg + (msg->sadb_msg_len << 3));
		if (msg->sadb_msg_type != SADB_DUMP) {
			msg = next;
			continue;
		}

		if (pfkey_align(msg, mhp) || pfkey_check(mhp)) {
			plog(LLV_ERROR, LOCATION, NULL,
				"pfkey_check (%s)\n", ipsec_strerror());
			msg = next;
			continue;
		}

		sa = (struct sadb_sa *)(mhp[SADB_EXT_SA]);
		if (!sa
		 || !mhp[SADB_EXT_ADDRESS_SRC]
		 || !mhp[SADB_EXT_ADDRESS_DST]) {
			msg = next;
			continue;
		}
		src = PFKEY_ADDR_SADDR(mhp[SADB_EXT_ADDRESS_SRC]);
		dst = PFKEY_ADDR_SADDR(mhp[SADB_EXT_ADDRESS_DST]);

		if (sa->sadb_sa_state != SADB_SASTATE_MATURE
		 && sa->sadb_sa_state != SADB_SASTATE_DYING) {
			msg = next;
			continue;
		}

		/* XXX n^2 algorithm, inefficient */

		/* don't delete inbound SAs at the moment */
		/* XXX should we remove SAs with opposite direction as well? */
		if (cmpsaddrwop(dst0, dst)) {
			msg = next;
			continue;
		}

		for (i = 0; i < n; i++) {
			plog(LLV_DEBUG, LOCATION, NULL,
				"check spi: packet %u against SA %u.\n",
				ntohl(spi[i]), ntohl(sa->sadb_sa_spi));
			if (spi[i] != sa->sadb_sa_spi)
				continue;

			pfkey_send_delete(lcconf->sock_pfkey,
				msg->sadb_msg_satype,
				IPSEC_MODE_ANY,
				src, dst, sa->sadb_sa_spi);

			/*
			 * delete a relative phase 2 handler.
			 * continue to process if no relative phase 2 handler
			 * exists.
			 */
			iph2 = getph2bysaidx(src, dst, proto, spi[i]);
			if (iph2) {
				unbindph12(iph2);
				remph2(iph2);
				delph2(iph2);
			}

			plog(LLV_INFO, LOCATION, NULL,
				"proto_id %s purging spi:%d.\n",
				s_ipsecdoi_proto(proto),
				ntohl(spi[i]));
		}

		msg = next;
	}

	if (buf)
		vfree(buf);
}

/*
 * delete all IKE/IPSEC-SA relatived to remote address.
 */
static void
info_recv_initialcontact(iph1)
	struct ph1handle *iph1;
{
	vchar_t *buf = NULL;
	struct sadb_msg *msg, *next, *end;
	struct sadb_sa *sa;
	struct sockaddr *src, *dst;
	caddr_t mhp[SADB_EXT_MAX + 1];
	int proto_id;
	struct ph2handle *iph2;

	if (f_local)
		goto step_ph1;

	/*
	 * if the initial contact is acceptable.  when both ends are using
	 * "use" as the policy level, both ends initiate the phase 1 and
	 * send initial-contacts under each phase 1 SA.  In this case, each
	 * phase 1 SA will be banished by the another initial-contact.
	 * Simply calling getcontacted() is the solution to avoid the case.
	 */
	if (getcontacted(iph1->remote))
		return;

	/* purge IPsec-SA(s) */
	buf = pfkey_dump_sadb(SADB_SATYPE_UNSPEC);
	if (buf == NULL) {
		plog(LLV_DEBUG, LOCATION, NULL,
			"pfkey_dump_sadb returned nothing.\n");
		return;
	}

	msg = (struct sadb_msg *)buf->v;
	end = (struct sadb_msg *)(buf->v + buf->l);

	while (msg < end) {
		if ((msg->sadb_msg_len << 3) < sizeof(*msg))
			break;
		next = (struct sadb_msg *)((caddr_t)msg + (msg->sadb_msg_len << 3));
		if (msg->sadb_msg_type != SADB_DUMP) {
			msg = next;
			continue;
		}

		if (pfkey_align(msg, mhp) || pfkey_check(mhp)) {
			plog(LLV_ERROR, LOCATION, NULL,
				"pfkey_check (%s)\n", ipsec_strerror());
			msg = next;
			continue;
		}

		if (mhp[SADB_EXT_SA] == NULL
		 || mhp[SADB_EXT_ADDRESS_SRC] == NULL
		 || mhp[SADB_EXT_ADDRESS_DST] == NULL) {
			msg = next;
			continue;
		}
		sa = (struct sadb_sa *)mhp[SADB_EXT_SA];
		src = PFKEY_ADDR_SADDR(mhp[SADB_EXT_ADDRESS_SRC]);
		dst = PFKEY_ADDR_SADDR(mhp[SADB_EXT_ADDRESS_DST]);

		if (sa->sadb_sa_state != SADB_SASTATE_MATURE
		 && sa->sadb_sa_state != SADB_SASTATE_DYING) {
			msg = next;
			continue;
		}

		/*
		 * If the remote address matchs with src or dst of SA,
		 * then delete the SA.
		 */
		if (cmpsaddrwop(iph1->remote, src)
		 && cmpsaddrwop(iph1->remote, dst)) {
			msg = next;
			continue;
		}

		plog(LLV_INFO, LOCATION, NULL,
			"purging spi=%u.\n", ntohl(sa->sadb_sa_spi));
		pfkey_send_delete(lcconf->sock_pfkey,
			msg->sadb_msg_satype,
			IPSEC_MODE_ANY, src, dst, sa->sadb_sa_spi);

		/*
		 * delete a relative phase 2 handler.
		 * continue to process if no relative phase 2 handler
		 * exists.
		 */
		proto_id = pfkey2ipsecdoi_proto(msg->sadb_msg_satype);
		iph2 = getph2bysaidx(src, dst, proto_id, sa->sadb_sa_spi);
		if (iph2) {
			unbindph12(iph2);
			remph2(iph2);
			delph2(iph2);
		}

		msg = next;
	}

	if (buf)
		vfree(buf);

    step_ph1:
	purgeph1(iph1);
}

/*
 * handling to receive Deletion payload
 */
static int
isakmp_info_recv_d(iph1, msg)
	struct ph1handle *iph1;
	vchar_t *msg;
{
	struct isakmp_pl_d *d;
	int tlen, num_spi;
	vchar_t *pbuf;
	struct isakmp_parse_t *pa, *pap;
	int protected = 0;
	union {
		u_int32_t spi32;
		u_int16_t spi16[2];
	} spi;

	/* validate the type of next payload */
	if (!(pbuf = isakmp_parse(msg)))
		return -1;
	pa = (struct isakmp_parse_t *)pbuf->v;
	for (pap = pa; pap->type; pap++) {
		switch (pap->type) {
		case ISAKMP_NPTYPE_D:
			break;
		case ISAKMP_NPTYPE_HASH:
			if (pap == pa) {
				protected++;
				break;
			}
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"received next payload type %d "
				"in wrong place (must be the first payload).\n",
				pap->type);
			vfree(pbuf);
			return -1;
		default:
			/* don't send information, see isakmp_ident_r1() */
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"reject the packet, "
				"received unexpecting payload type %d.\n",
				pap->type);
			vfree(pbuf);
			return 0;
		}
	}

	if (!protected) {
		plog(LLV_ERROR, LOCATION, NULL,
			"delete payload is not proteted, "
			"ignored.\n");
		vfree(pbuf);
		return -1;
	}

	/* process a delete payload */
	for (pap = pa; pap->type; pap++) {
		if (pap->type != ISAKMP_NPTYPE_D)
			continue;

		d = (struct isakmp_pl_d *)pap->ptr;

		if (ntohl(d->doi) != IPSEC_DOI) {
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"delete payload with invalid doi:%d.\n",
				ntohl(d->doi));
			continue;
		}

		num_spi = ntohs(d->num_spi);
		tlen = ntohs(d->h.len) - sizeof(struct isakmp_pl_d);

		if (tlen != num_spi * d->spi_size) {
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"deletion payload with invalid length.\n");
			vfree(pbuf);
			return -1;
		}

		switch (d->proto_id) {
		case IPSECDOI_PROTO_ISAKMP:
			if (d->spi_size != sizeof(isakmp_index)) {
				plog(LLV_ERROR, LOCATION, iph1->remote,
					"delete payload with strange spi "
					"size %d(proto_id:%d)\n",
					d->spi_size, d->proto_id);
				continue;
			}
			purge_isakmp_spi(d->proto_id,
					(isakmp_index *)(d + 1), num_spi);
			break;

		case IPSECDOI_PROTO_IPSEC_AH:
		case IPSECDOI_PROTO_IPSEC_ESP:
			if (d->spi_size != sizeof(u_int32_t)) {
				plog(LLV_ERROR, LOCATION, iph1->remote,
					"delete payload with strange spi "
					"size %d(proto_id:%d)\n",
					d->spi_size, d->proto_id);
				continue;
			}
			purge_ipsec_spi(iph1->remote, d->proto_id,
			    (u_int32_t *)(d + 1), num_spi);
			break;

		case IPSECDOI_PROTO_IPCOMP:
			/* need to handle both 16bit/32bit SPI */
			memset(&spi, 0, sizeof(spi));
			if (d->spi_size == sizeof(spi.spi16[1])) {
				memcpy(&spi.spi16[1], d + 1,
				    sizeof(spi.spi16[1]));
			} else if (d->spi_size == sizeof(spi.spi32))
				memcpy(&spi.spi32, d + 1, sizeof(spi.spi32));
			else {
				plog(LLV_ERROR, LOCATION, iph1->remote,
					"delete payload with strange spi "
					"size %d(proto_id:%d)\n",
					d->spi_size, d->proto_id);
				continue;
			}
			purge_ipsec_spi(iph1->remote, d->proto_id,
			    &spi.spi32, num_spi);
			break;

		default:
			plog(LLV_ERROR, LOCATION, iph1->remote,
				"deletion message received, "
				"invalid proto_id: %d\n",
				d->proto_id);
			continue;
		}

		plog(LLV_INFO, LOCATION, NULL,
			"packet properly proteted, purge SPIs.\n");
	}

	vfree(pbuf);

	return 0;
}

void
isakmp_check_notify(gen, iph1)
	struct isakmp_gen *gen;		/* points to Notify payload */
	struct ph1handle *iph1;
{
	struct isakmp_pl_n *notify = (struct isakmp_pl_n *)gen;

	plog(LLV_DEBUG, LOCATION, iph1->remote,
		"Notify Message received\n");

	switch (ntohs(notify->type)) {
	case ISAKMP_NTYPE_CONNECTED:
		plog(LLV_WARNING, LOCATION, iph1->remote,
			"ignore CONNECTED notification.\n");
		break;
	case ISAKMP_NTYPE_RESPONDER_LIFETIME:
		plog(LLV_WARNING, LOCATION, iph1->remote,
			"ignore RESPONDER-LIFETIME notification.\n");
		break;
	case ISAKMP_NTYPE_REPLAY_STATUS:
		plog(LLV_WARNING, LOCATION, iph1->remote,
			"ignore REPLAY-STATUS notification.\n");
		break;
	case ISAKMP_NTYPE_INITIAL_CONTACT:
		plog(LLV_WARNING, LOCATION, iph1->remote,
			"ignore INITIAL-CONTACT notification.\n");
		break;
	default:
		isakmp_info_send_n1(iph1, ISAKMP_NTYPE_INVALID_PAYLOAD_TYPE, NULL);
		plog(LLV_ERROR, LOCATION, iph1->remote,
			"received unknown notification type %u.\n",
		    ntohs(notify->type));
	}

	return;
}

