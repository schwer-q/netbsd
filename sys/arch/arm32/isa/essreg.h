/*	$NetBSD: essreg.h,v 1.2 1998/06/08 20:23:07 tv Exp $	*/

/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

/*
**++
**
**  essreg.h
**
**  FACILITY:
**
**	DIGITAL Network Appliance Reference Design (DNARD)
**
**  MODULE DESCRIPTION:
**
**      This module contains the constant definitions for the device
**      registers on the ESS Technologies 1888/1887/888 sound chip.
**
**  AUTHORS:
**
**	Blair Fidler	Software Engineering Australia
**			Gold Coast, Australia.
**
**  CREATION DATE:  
**
**	March 10, 1997.
**
**  MODIFICATION HISTORY:
**
**--
*/

/*
 * DSP commands.  This unit handles MIDI and audio capabilities.
 * The DSP can be reset, data/commands can be read or written to it,
 * and it can generate interrupts.  Interrupts are generated for MIDI
 * input or DMA completion.  They seem to have neglected the fact 
 * that it would be nice to have a MIDI transmission complete interrupt.
 * Worse, the DMA engine is half-duplex.  This means you need to do
 * (timed) programmed I/O to be able to record and play simulataneously.
 */
#define ESS_ACMD_DAC8WRITE	0x10	/* direct-mode 8-bit DAC write */
#define ESS_ACMD_DAC16WRITE	0x11	/* direct-mode 16-bit DAC write */
#define ESS_ACMD_DMA8OUT	0x14	/* 8-bit linear DMA output */
#define ESS_ACMD_DMA16OUT	0x15	/* 16-bit linear DMA output */
#define ESS_ACMD_AUTODMA8OUT	0x1C	/* auto-init 8-bit linear DMA output */
#define ESS_ACMD_AUTODMA16OUT	0x1D	/* auto-init 16-bit linear DMA output */
#define ESS_ACMD_ADC8READ	0x20	/* direct-mode 8-bit ADC read */
#define ESS_ACMD_ADC16READ	0x21	/* direct-mode 16-bit ADC read */
#define ESS_ACMD_DMA8IN		0x24	/* 8-bit linear DMA input */
#define ESS_ACMD_DMA16IN	0x25	/* 16-bit linear DMA input */
#define ESS_ACMD_AUTODMA8IN	0x2C	/* auto-init 8-bit linear DMA input */
#define ESS_ACMD_AUTODMA16IN	0x2D	/* auto-init 16-bit linear DMA input */
#define ESS_ACMD_SETTIMECONST1	0x40	/* set time constant (1MHz base) */
#define	ESS_ACMD_SETTIMECONST15	0x41	/* set time constant (1.5MHz base) */
#define	ESS_ACMD_SETFILTER	0x42	/* set filter clock independently */
#define ESS_ACMD_BLOCKSIZE	0x48	/* set blk size for high speed xfer */

#define ESS_ACMD_DMA4OUT	0x74	/* 4-bit ADPCM DMA output */
#define ESS_ACMD_DMA4OUTREF	0x75	/* 4-bit ADPCM DMA output with ref */
#define ESS_ACMD_DMA2_6OUT	0x76	/* 2.6-bit ADPCM DMA output */
#define ESS_ACMD_DMA2_6OUTREF	0x77	/* 2.6-bit ADPCM DMA output with ref */
#define ESS_ACMD_DMA2OUT	0x7A	/* 2-bit ADPCM DMA output */
#define ESS_ACMD_DMA2OUTREF	0x7B	/* 2-bit ADPCM DMA output with ref */
#define ESS_ACMD_SILENCEOUT	0x80	/* output a block of silence */
#define ESS_ACMD_START_AUTO_OUT	0x90	/* start auto-init 8-bit DMA output */
#define ESS_ACMD_START_OUT	0x91	/* start 8-bit DMA output */
#define ESS_ACMD_START_AUTO_IN	0x98	/* start auto-init 8-bit DMA input */
#define ESS_ACMD_START_IN	0x99	/* start 8-bit DMA input */

#define ESS_XCMD_SAMPLE_RATE	0xA1	/* sample rate for Audio1 channel */
#define ESS_XCMD_FILTER_CLOCK	0xA2	/* filter clock for Audio1 channel*/
#define ESS_XCMD_XFER_COUNTLO	0xA4	/* */
#define ESS_XCMD_XFER_COUNTHI	0xA5	/* */
#define ESS_XCMD_AUDIO_CTRL	0xA8	/* */
#define					ESS_AUDIO_CTRL_MONITOR	0x08
					/* 0=disable/1=enable */
#define					ESS_AUDIO_CTRL_MONO	0x02
					/* 0=disable/1=enable */
#define					ESS_AUDIO_CTRL_STEREO	0x01
					/* 0=disable/1=enable */
#define ESS_XCMD_PREAMP_CTRL	0xA9 	/* */
#define					ESS_PREAMP_CTRL_ENABLE	0x04

#define ESS_XCMD_IRQ_CTRL	0xB1	/* legacy audio interrupt control */
#define ESS_XCMD_DRQ_CTRL	0xB2	/* audio DRQ control */
#define ESS_XCMD_VOLIN_CTRL	0xB4	/* stereo input volume control */
#define ESS_XCMD_AUDIO1_CTRL1	0xB7	/* */
#define					ESS_AUDIO1_CTRL1_FIFO_SIGNED	0x20
					/* 0=unsigned/1=signed */
#define					ESS_AUDIO1_CTRL1_FIFO_SIZE	0x04
					/* 0=8-bit/1=16-bit */
#define ESS_XCMD_AUDIO1_CTRL2	0xB8	/* */
#define					ESS_AUDIO1_CTRL2_FIFO_ENABLE	0x01
					/* 0=disable/1=enable */
#define					ESS_AUDIO1_CTRL2_DMA_READ	0x02
					/* 0=DMA write/1=DMA read */
#define					ESS_AUDIO1_CTRL2_ADC_ENABLE	0x08
					/* 0=DAC mode/1=ADC mode */
#define	ESS_XCMD_DEMAND_CTRL	0xB9	/* */

#define ESS_ACMD_ENABLE_EXT	0xC6	/* enable ESS extension commands */
#define ESS_ACMD_DISABLE_EXT	0xC7	/* enable ESS extension commands */

#define ESS_ACMD_PAUSE_DMA	0xD0	/* pause DMA */
#define ESS_ACMD_ENABLE_SPKR	0xD1	/* enable Audio1 DAC input to mixer */
#define ESS_ACMD_DISABLE_SPKR	0xD3	/* disable Audio1 DAC input to mixer */
#define ESS_ACMD_CONT_DMA	0xD4	/* continue paused DMA */
#define ESS_ACMD_SPKR_STATUS	0xD8	/* return Audio1 DAC status: */
#define 				ESS_SPKR_OFF	0x00
#define 				ESS_SPKR_ON	0xFF
#define ESS_ACMD_VERSION	0xE1	/* get version number */
#define ESS_ACMD_LEGACY_ID	0xE7	/* get legacy ES688/ES1688 ID bytes */
/*
 * Macros to detect valid hardware configuration data.
 */
#define ESS_IRQ1_VALID(irq, model)  ((irq) == 5 || (irq) == 7 || (irq) == 9 || (irq) == 10 || ((model) == ESS_1887 && (irq) == 15))

#define ESS_IRQ2_VALID(irq, model)  (((model) != ESS_1887) ? ((irq) == 15) : ((irq) == 5 || (irq) == 7 || (irq) == 9 || (irq) == 10) || (irq) == 15)

#define ESS_DRQ1_VALID(chan, model) ((chan) == 0 || (chan) == 1 || (chan) == 3 || ((model) == ESS_1887 && (chan) == 5))

#define ESS_DRQ2_VALID(chan, model) (((model) != ESS_1887) ? ((chan) == 5) : ((chan) == 0 || (chan) == 1 || (chan) == 3 || (chan) == 5))

#define ESS_BASE_VALID(base) ((base) == 0x220 || (base) == 0x230 || (base) == 0x240 || (base) == 0x250)

#define ESS_DMA_SIZE(chan) ((chan & 4) ? ESS_MODE_16BIT: ESS_MODE_8BIT)

/*
 * Macros to manipulate gain values
 */
#define ESS_4BIT_GAIN(x)	((x) & 0xf0)
#define ESS_3BIT_GAIN(x)	(((x) & 0xe0) >> 1)
#define ESS_STEREO_GAIN(l, r)	((l) | ((r) >> 4))
#define ESS_MONO_GAIN(x)	((x) >> 4)

#ifdef ESS_AMODE_LOW
/*
 * Registers used to configure ESS chip via Read Key Sequence
 */
#define ESS_CONFIG_KEY_BASE	0x229
#define ESS_CONFIG_KEY_PORTS	3
#else
/*
 * Registers used to configure ESS chip via System Control Register (SCR)
 */
#define ESS_SCR_ACCESS_BASE	0xF9
#define ESS_SCR_ACCESS_PORTS	3
#define ESS_SCR_LOCK		0
#define ESS_SCR_UNLOCK		2

#define ESS_SCR_BASE		0xE0
#define ESS_SCR_PORTS		2
#define ESS_SCR_INDEX		0
#define ESS_SCR_DATA		1

/*
 * Bit definitions for SCR
 */
#define ESS_SCR_AUDIO_ENABLE	0x04
#define ESS_SCR_AUDIO_220	0x00
#define ESS_SCR_AUDIO_230	0x01
#define ESS_SCR_AUDIO_240	0x02
#define ESS_SCR_AUDIO_250	0x03
#endif

/*****************************************************************************/
/*  DSP Timeout Definitions                                                  */
/*****************************************************************************/
#define	ESS_RESET_TIMEOUT	5000	/* ??? */
#define	ESS_READ_TIMEOUT  	5000	/* number of times to try a read */
#define	ESS_WRITE_TIMEOUT  	5000	/* number of times to try a write */


#define ESS_NPORT		16
#define ESS_DSP_RESET		0x06
#define		ESS_MAGIC	0xAA 	/* response to successful reset */

#define ESS_DSP_READ		0x0A
#define ESS_DSP_WRITE		0x0C

#define	ESS_DSP_RW_STATUS   	0x0C
#define ESS_CLEAR_INTR		0x0E

#define	ESS_DSP_READ_STATUS   	0x0C
#define	ESS_DSP_READ_MASK      	0x40
#define	ESS_DSP_READ_READY     	0x40

#define	ESS_DSP_WRITE_STATUS	0x0C
#define	ESS_DSP_WRITE_MASK	0x80
#define ESS_DSP_WRITE_READY     0x00


#define	ESS_MIX_REG_SELECT	0x04
#define	ESS_MIX_REG_DATA	0x05
#define ESS_MIX_RESET		0x00 	/* mixer reset port and value */


/*
 * ESS Mixer registers
 */
#define ESS_MREG_SAMPLE_RATE	0x70	/* sample rate for Audio2 channel */
#define ESS_MREG_FILTER_CLOCK	0x72	/* filter clock for Audio2 channel */
#define ESS_MREG_XFER_COUNTLO	0x74	/* low-byte of DMA transfer size */
#define ESS_MREG_XFER_COUNTHI	0x76	/* high-byte of DMA transfer size */
#define ESS_MREG_AUDIO2_CTRL1	0x78	/* control register 1 for Audio2: */
#define					ESS_AUDIO2_CTRL1_XFER_SIZE	0x20
					/* 0=8-bit/1=16-bit */
#define					ESS_AUDIO2_CTRL1_FIFO_ENABLE	0x02
					/* 0=disable/1=enable */
#define					ESS_AUDIO2_CTRL1_DAC_ENABLE	0x01
					/* 0=disable/1=enable */
#define ESS_MREG_AUDIO2_CTRL2	0x7A	/* control register 2 for Audio2: */
#define					ESS_AUDIO2_CTRL2_FIFO_SIZE	0x01
					/* 0=8-bit/1=16-bit */
#define					ESS_AUDIO2_CTRL2_CHANNELS	0x02
					/* 0=mono/1=stereo */
#define					ESS_AUDIO2_CTRL2_FIFO_SIGNED	0x04
					/* 0=unsigned/1=signed */
#define					ESS_AUDIO2_CTRL2_DMA_ENABLE	0x20
					/* 0=disable/1=enable */
