/*	$NetBSD: tsarmreg.h,v 1.1 2004/12/23 04:29:38 joff Exp $	*/

/*
 * Copyright (c) 2002 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Based on code written by Jason R. Thorpe for Wasabi Systems, Inc.
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

#ifndef _TS7XXXREG_H_
#define	_TS7XXXREG_H_

/*
 * Memory map and register definitions for the TS-7200 single board computer
 */
#define TS7XXX_IO_VBASE		0xf0300000UL
#define TS7XXX_IO8_VBASE	TS7XXX_IO_VBASE
#define TS7XXX_IO8_HWBASE	0x10000000UL
#define TS7XXX_IO8_SIZE		0x04000000UL
#define  TS7XXX_STATUS1		0x00800000UL
#define  TS7XXX_CFREGS1		0x01000001UL
#define  TS7XXX_CFREGS2		0x01040006UL
#define TS7XXX_IO16_VBASE	(TS7XXX_IO8_VBASE + TS7XXX_IO8_SIZE)
#define TS7XXX_IO16_HWBASE	0x20000000UL
#define TS7XXX_IO16_SIZE	0x04000000UL
#define  TS7XXX_ISAMEM		0x01800000UL
#define  TS7XXX_ISAIO		0x01c00000UL
#define  TS7XXX_CFDATA		0x01000000UL
#define  TS7XXX_MODEL		0x02000000UL
#define  TS7XXX_FEATURES	0x02400000UL
#define  TS7XXX_STATUS2		0x02800000UL
#define  TS7XXX_PLDREV		0x03400000UL
#define  TS7XXX_WDOGCTRL	0x03800000UL
#define  TS7XXX_WDOGFEED	0x03c00000UL

#endif /* _TS7XXXREG_H_ */
