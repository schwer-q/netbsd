/*	$NetBSD: pcmciareg.h,v 1.2 1997/10/16 23:27:38 thorpej Exp $	*/

/*
 * Copyright (c) 1997 Marc Horowitz.  All rights reserved.
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
 *	This product includes software developed by Marc Horowitz.
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

/* most of this is from the PCMCIA PC Card Standard, Release 2.1 */

/*
 * CIS Tuples
 */

/* Layer 1 Basic Compatibility Tuples */
#define	PCMCIA_CISTPL_NULL			0x00
#define	PCMCIA_CISTPL_DEVICE			0x01
#define	PCMCIA_DTYPE_MASK			0xF0
#define	PCMCIA_DTYPE_NULL			0x00
#define	PCMCIA_DTYPE_ROM			0x10
#define	PCMCIA_DTYPE_OTPROM			0x20
#define	PCMCIA_DTYPE_EPROM			0x30
#define	PCMCIA_DTYPE_EEPROM			0x40
#define	PCMCIA_DTYPE_FLASH			0x50
#define	PCMCIA_DTYPE_SRAM			0x60
#define	PCMCIA_DTYPE_DRAM			0x70
#define	PCMCIA_DTYPE_FUNCSPEC			0xD0
#define	PCMCIA_DTYPE_EXTEND			0xE0
#define	PCMCIA_DSPEED_MASK			0x07
#define	PCMCIA_DSPEED_NULL			0x00
#define	PCMCIA_DSPEED_250NS			0x01
#define	PCMCIA_DSPEED_200NS			0x02
#define	PCMCIA_DSPEED_150NS			0x03
#define	PCMCIA_DSPEED_100NS			0x04
#define	PCMCIA_DSPEED_EXT			0x05

/*
 * the 2.1 docs have 0x02-0x07 as reserved, but the linux drivers list the
 * follwing tuple code values.  I have at least one card (3com 3c562
 * lan+modem) which has a code 0x06 tuple, so I'm going to assume that these
 * are for real
 */

#define	PCMCIA_CISTPL_LONGLINK_CB		0x02
#define	PCMCIA_CISTPL_CONFIG_CB			0x04
#define	PCMCIA_CISTPL_CFTABLE_ENTRY_CB		0x05
#define	PCMCIA_CISTPL_LONGLINK_MFC		0x06
#define	PCMCIA_MFC_MEM_ATTR			0x00
#define	PCMCIA_MFC_MEM_COMMON			0x01
#define	PCMCIA_CISTPL_BAR			0x07

#define	PCMCIA_CISTPL_CHECKSUM			0x10
#define	PCMCIA_CISTPL_LONGLINK_A		0x11
#define	PCMCIA_CISTPL_LONGLINK_C		0x12
#define	PCMCIA_CISTPL_LINKTARGET		0x13
#define	PCMCIA_CISTPL_NO_LINK			0x14
#define	PCMCIA_CISTPL_VERS_1			0x15
#define	PCMCIA_CISTPL_ALTSTR			0x16
#define	PCMCIA_CISTPL_DEVICE_A			0x17
#define	PCMCIA_CISTPL_JEDEC_C			0x18
#define	PCMCIA_CISTPL_JEDEC_A			0x19
#define	PCMCIA_CISTPL_CONFIG			0x1A
#define	PCMCIA_TPCC_RASZ_MASK			0x03
#define	PCMCIA_TPCC_RASZ_SHIFT			0
#define	PCMCIA_TPCC_RMSZ_MASK			0x3C
#define	PCMCIA_TPCC_RMSZ_SHIFT			2
#define	PCMCIA_TPCC_RFSZ_MASK			0xC0
#define	PCMCIA_TPCC_RFSZ_SHIFT			6
#define	PCMCIA_CISTPL_CFTABLE_ENTRY		0x1B
#define	PCMCIA_TPCE_INDX_INTFACE		0x80
#define	PCMCIA_TPCE_INDX_DEFAULT		0x40
#define	PCMCIA_TPCE_INDX_NUM_MASK		0x3F
#define	PCMCIA_TPCE_IF_MWAIT			0x80
#define	PCMCIA_TPCE_IF_RDYBSY			0x40
#define	PCMCIA_TPCE_IF_WP			0x20
#define	PCMCIA_TPCE_IF_BVD			0x10
#define	PCMCIA_TPCE_IF_IFTYPE			0x0F
#define	PCMCIA_IFTYPE_MEMORY			0
#define	PCMCIA_IFTYPE_IO			1
#define	PCMCIA_TPCE_FS_MISC			0x80
#define	PCMCIA_TPCE_FS_MEMSPACE_MASK		0x60
#define	PCMCIA_TPCE_FS_MEMSPACE_NONE		0x00
#define	PCMCIA_TPCE_FS_MEMSPACE_LENGTH		0x20
#define	PCMCIA_TPCE_FS_MEMSPACE_LENGTHADDR	0x40
#define	PCMCIA_TPCE_FS_MEMSPACE_TABLE		0x60
#define	PCMCIA_TPCE_FS_IRQ			0x10
#define	PCMCIA_TPCE_FS_IOSPACE			0x08
#define	PCMCIA_TPCE_FS_TIMING			0x04
#define	PCMCIA_TPCE_FS_POWER_MASK		0x03
#define	PCMCIA_TPCE_FS_POWER_NONE		0x00
#define	PCMCIA_TPCE_FS_POWER_VCC		0x01
#define	PCMCIA_TPCE_FS_POWER_VCCVPP1		0x02
#define	PCMCIA_TPCE_FS_POWER_VCCVPP1VPP2	0x03
#define	PCMCIA_TPCE_TD_RESERVED_MASK		0xE0
#define	PCMCIA_TPCE_TD_RDYBSY_MASK		0x1C
#define	PCMCIA_TPCE_TD_WAIT_MASK		0x03
#define	PCMCIA_TPCE_IO_HASRANGE			0x80
#define	PCMCIA_TPCE_IO_BUSWIDTH_16BIT		0x40
#define	PCMCIA_TPCE_IO_BUSWIDTH_8BIT		0x20
#define	PCMCIA_TPCE_IO_IOADDRLINES_MASK		0x1F
#define	PCMCIA_TPCE_IO_RANGE_LENGTHSIZE_MASK	0xC0
#define	PCMCIA_TPCE_IO_RANGE_LENGTHSIZE_NONE	0x00
#define	PCMCIA_TPCE_IO_RANGE_LENGTHSIZE_ONE	0x40
#define	PCMCIA_TPCE_IO_RANGE_LENGTHSIZE_TWO	0x80
#define	PCMCIA_TPCE_IO_RANGE_LENGTHSIZE_FOUR	0xC0
#define	PCMCIA_TPCE_IO_RANGE_ADDRSIZE_MASK	0x30
#define	PCMCIA_TPCE_IO_RANGE_ADDRSIZE_NONE	0x00
#define	PCMCIA_TPCE_IO_RANGE_ADDRSIZE_ONE	0x10
#define	PCMCIA_TPCE_IO_RANGE_ADDRSIZE_TWO	0x20
#define	PCMCIA_TPCE_IO_RANGE_ADDRSIZE_FOUR	0x30
#define	PCMCIA_TPCE_IO_RANGE_COUNT		0x0F
#define	PCMCIA_TPCE_IR_SHARE			0x80
#define	PCMCIA_TPCE_IR_PULSE			0x40
#define	PCMCIA_TPCE_IR_LEVEL			0x20
#define	PCMCIA_TPCE_IR_HASMASK			0x10
#define	PCMCIA_TPCE_IR_IRQ			0x0F
#define	PCMCIA_TPCE_MS_HOSTADDR			0x80
#define	PCMCIA_TPCE_MS_CARDADDR_SIZE_MASK	0x60
#define	PCMCIA_TPCE_MS_CARDADDR_SIZE_SHIFT	5
#define	PCMCIA_TPCE_MS_LENGTH_SIZE_MASK		0x18
#define	PCMCIA_TPCE_MS_LENGTH_SIZE_SHIFT	3
#define	PCMCIA_TPCE_MS_COUNT			0x07
#define	PCMCIA_TPCE_MI_EXT			0x80
#define	PCMCIA_TPCE_MI_RESERVED			0x40
#define	PCMCIA_TPCE_MI_PWRDOWN			0x20
#define	PCMCIA_TPCE_MI_READONLY			0x10
#define	PCMCIA_TPCE_MI_AUDIO			0x08
#define	PCMCIA_TPCE_MI_MAXTWINS			0x07
#define	PCMCIA_CISTPL_DEVICE_OC			0x1C
#define	PCMCIA_CISTPL_DEVICE_OA			0x1D
#define	PCMCIA_CISTPL_DEVICE_GEO		0x1E
#define	PCMCIA_CISTPL_DEVICE_GEO_A		0x1F

/* Layer 2 Data Recording Format Tuples */

#define	PCMCIA_CISTPL_MANFID			0x20
#define	PCMCIA_CISTPL_FUNCID			0x21
#define	PCMCIA_CISTPL_FUNCE			0x22
#define	PCMCIA_CISTPL_SWIL			0x23
/* #define	PCMCIA_CISTPL_RESERVED		0x24-0x3F */
#define	PCMCIA_CISTPL_VERS_2			0x40
#define	PCMCIA_CISTPL_FORMAT			0x41
#define	PCMCIA_CISTPL_GEOMETRY			0x42
#define	PCMCIA_CISTPL_BYTEORDER			0x43
#define	PCMCIA_CISTPL_DATE			0x44
#define	PCMCIA_CISTPL_BATTERY			0x45

/* Layer 3 Data Organization Tuples */

#define	PCMCIA_CISTPL_ORG			0x46
/* #define	PCMCIA_CISTPL_RESERVED		0x47-0x7F */

/* Layer 4 System-Specific Standard Tuples */

/* #define	PCMCIA_CISTPL_RESERVED		0x80-0xFE */
#define	PCMCIA_CISTPL_END			0xFF

/*
 * Card Configuration Registers
 */

#define	PCMCIA_CCR_OPTION			0x00
#define	PCMCIA_CCR_OPTION_SRESET		0x80
#define	PCMCIA_CCR_OPTION_LEVIREQ		0x40
#define	PCMCIA_CCR_OPTION_CFINDEX		0x3F
#define	PCMCIA_CCR_OPTION_IREQ_ENABLE		0x04
#define	PCMCIA_CCR_OPTION_ADDR_DECODE		0x02
#define	PCMCIA_CCR_OPTION_FUNC_ENABLE		0x01
#define	PCMCIA_CCR_STATUS			0x02
#define	PCMCIA_CCR_STATUS_PINCHANGED		0x80
#define	PCMCIA_CCR_STATUS_SIGCHG		0x40
#define	PCMCIA_CCR_STATUS_IOIS8			0x20
#define	PCMCIA_CCR_STATUS_RESERVED1		0x10
#define	PCMCIA_CCR_STATUS_AUDIO			0x08
#define	PCMCIA_CCR_STATUS_PWRDWN		0x04
#define	PCMCIA_CCR_STATUS_INTR			0x02
#define	PCMCIA_CCR_STATUS_INTRACK		0x01
#define	PCMCIA_CCR_PIN				0x04
#define	PCMCIA_CCR_PIN_CBVD1			0x80
#define	PCMCIA_CCR_PIN_CBVD2			0x40
#define	PCMCIA_CCR_PIN_CRDYBSY			0x20
#define	PCMCIA_CCR_PIN_CWPROT			0x10
#define	PCMCIA_CCR_PIN_RBVD1			0x08
#define	PCMCIA_CCR_PIN_RBVD2			0x04
#define	PCMCIA_CCR_PIN_RRDYBSY			0x02
#define	PCMCIA_CCR_PIN_RWPROT			0x01
#define	PCMCIA_CCR_SOCKETCOPY			0x06
#define	PCMCIA_CCR_SOCKETCOPY_RESERVED		0x80
#define	PCMCIA_CCR_SOCKETCOPY_COPY_MASK		0x70
#define	PCMCIA_CCR_SOCKETCOPY_COPY_SHIFT	4
#define	PCMCIA_CCR_SOCKETCOPY_SOCKET_MASK	0x0F
#define	PCMCIA_CCR_IOBASE0			0x0A
#define	PCMCIA_CCR_IOBASE1			0x0C
#define	PCMCIA_CCR_IOBASE2			0x0E
#define	PCMCIA_CCR_IOBASE3			0x10
#define	PCMCIA_CCR_IOSIZE			0x12

#define	PCMCIA_CCR_SIZE				0x14
