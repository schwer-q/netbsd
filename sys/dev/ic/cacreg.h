/*	$NetBSD: cacreg.h,v 1.3 2000/06/13 13:36:47 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
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

/*-
 * Copyright (c) 1999 Jonathan Lemon
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * FreeBSD: src/sys/dev/ida/idareg.h,v 1.2 1999/08/28 00:41:55 peter Exp
 */

#ifndef _IC_CACREG_H_
#define _IC_CACREG_H_

#define cac_inl(sc, port) \
	bus_space_read_4((sc)->sc_iot, (sc)->sc_ioh, port)
#define cac_outl(sc, port, val) \
	bus_space_write_4((sc)->sc_iot, (sc)->sc_ioh, port, val)

/* Board register offsets */
#define CAC_REG_CMD_FIFO	0x04
#define CAC_REG_DONE_FIFO	0x08
#define CAC_REG_INT_MASK	0x0C
#define CAC_REG_STATUS		0x10
#define CAC_REG_INT_PENDING	0x14

#define CAC_42REG_CMD_FIFO	0x40
#define CAC_42REG_DONE_FIFO	0x44
#define CAC_42REG_INT_MASK	0x34
#define CAC_42REG_STATUS	0x30
#define CAC_42REG_INT_PENDING	0x08

/* Interrupt mask values */
#define CAC_INT_DISABLE		0x00
#define CAC_INT_ENABLE		0x01

/* Command types */
#define	CAC_CMD_GET_LOG_DRV_INFO	0x10
#define	CAC_CMD_GET_CTRL_INFO		0x11
#define	CAC_CMD_SENSE_DRV_STATUS	0x12
#define	CAC_CMD_START_RECOVERY		0x13
#define	CAC_CMD_GET_PHYS_DRV_INFO	0x15
#define	CAC_CMD_BLINK_DRV_LEDS		0x16
#define	CAC_CMD_SENSE_DRV_LEDS		0x17
#define	CAC_CMD_GET_LOG_DRV_EXT		0x18
#define	CAC_CMD_GET_CTRL_INFO		0x11
#define CAC_CMD_READ			0x20
#define CAC_CMD_WRITE			0x30
#define CAC_CMD_WRITE_MEDIA		0x31
#define CAC_CMD_GET_CONFIG		0x50
#define CAC_CMD_SET_CONFIG		0x51
#define CAC_CMD_FLUSH_CACHE		0xc2

/* Return status codes */
#define CAC_RET_SOFT_ERROR		0x02
#define CAC_RET_HARD_ERROR		0x04
#define CAC_RET_CMD_REJECTED		0x14

struct cac_drive_info {
	u_int16_t	secsize;
	u_int32_t	secperunit;
	u_int16_t	ncylinders;
	u_int8_t	nheads;
	u_int8_t	signature;
	u_int8_t	psectors;
	u_int16_t	wprecomp;
	u_int8_t	max_acc;
	u_int8_t	control;
	u_int16_t	pcylinders;
	u_int8_t	ptracks;
	u_int16_t	landing_zone;
	u_int8_t	nsectors;
	u_int8_t	checksum;
	u_int8_t	mirror;
} __attribute__((__packed__));

struct cac_controller_info {
	u_int8_t	num_drvs;
	u_int32_t	signature;
	u_int8_t	firm_rev[4];
} __attribute__((__packed__));

struct cac_hdr {
	u_int8_t	drive;		/* logical drive */
	u_int8_t	priority;	/* block priority */
	u_int16_t	size;		/* size of request, in words */
} __attribute__((__packed__));

struct cac_req {
	u_int16_t	next;		/* offset of next request */
	u_int8_t	command;	/* command */
	u_int8_t	error;		/* return error code */
	u_int32_t	blkno;		/* block number */
	u_int16_t	bcount;		/* block count */
	u_int8_t	sgcount;	/* number of scatter/gather entries */
	u_int8_t	reserved;	/* reserved */
} __attribute__((__packed__));

struct cac_sgb {
	u_int32_t	length;		/* length of S/G segment */
	u_int32_t	addr;		/* physical address of block */
} __attribute__((__packed__));

/*
 * Stupid macros to deal with alignment/endianness issues.
 */

#define CAC_GET1(x)							\
	(((u_char *)&(x))[0])
#define CAC_GET2(x)							\
	(((u_char *)&(x))[0] | (((u_char *)&(x))[1] << 8))
#define CAC_GET4(x)							\
	((((u_char *)&(x))[0] | (((u_char *)&(x))[1] << 8)) |		\
	(((u_char *)&(x))[0] << 16 | (((u_char *)&(x))[1] << 24)))
	
#endif	/* !_IC_CACREG_H_ */
