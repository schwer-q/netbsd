/*	$NetBSD: soundcard.h,v 1.1 1997/10/16 17:31:04 augustss Exp $	*/

/*
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
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

/*
 * WARNING!  WARNING!
 * This is an OSS (Linux) audio emulator.
 * Use the Native NetBSD API for developing new code, and this
 * only for compiling Linux programs.
 */

#ifndef _soundcard_h_
#define _soundcard_h_

#define SOUND_VERSION	0x030000

#define	SNDCTL_DSP_RESET		_IO  ('P', 0)
#define	SNDCTL_DSP_SYNC			_IO  ('P', 1)
#define	SNDCTL_DSP_SPEED		_IOWR('P', 2, int)
#define	SOUND_PCM_READ_RATE		_IOR ('P', 2, int)
#define	SNDCTL_DSP_STEREO		_IOWR('P', 3, int)
#define	SNDCTL_DSP_GETBLKSIZE		_IOWR('P', 4, int)
#define	SNDCTL_DSP_SETFMT		_IOWR('P', 5, int)
#define	SOUND_PCM_READ_BITS		_IOR ('P', 5, int)
#define	SNDCTL_DSP_CHANNELS		_IOWR('P', 6, int)
#define	SOUND_PCM_READ_CHANNELS		_IOR ('P', 6, int)
#define SOUND_PCM_WRITE_FILTER		_IOWR('P', 7, int)
#define SOUND_PCM_READ_FILTER		_IOR ('P', 7, int)
#define	SNDCTL_DSP_POST			_IO  ('P', 8)
#define SNDCTL_DSP_SUBDIVIDE		_IOWR('P', 9, int)
#define	SNDCTL_DSP_SETFRAGMENT		_IOWR('P', 10, int)
#define	SNDCTL_DSP_GETFMTS		_IOR ('P', 11, int)
#define SNDCTL_DSP_GETOSPACE		_IOR ('P',12, struct audio_buf_info)
#define SNDCTL_DSP_GETISPACE		_IOR ('P',13, struct audio_buf_info)
#define SNDCTL_DSP_NONBLOCK		_IO  ('P',14)
#define SNDCTL_DSP_GETCAPS		_IOR ('P',15, int)
# define DSP_CAP_REVISION		0x000000ff
# define DSP_CAP_DUPLEX			0x00000100
# define DSP_CAP_REALTIME		0x00000200
# define DSP_CAP_BATCH			0x00000400
# define DSP_CAP_COPROC			0x00000800
# define DSP_CAP_TRIGGER		0x00001000
# define DSP_CAP_MMAP			0x00002000
#define SNDCTL_DSP_GETTRIGGER		_IOR ('P', 16, int)
#define SNDCTL_DSP_SETTRIGGER		_IOW ('P', 16, int)
# define PCM_ENABLE_INPUT		0x00000001
# define PCM_ENABLE_OUTPUT		0x00000002
#define SNDCTL_DSP_GETIPTR		_IOR ('P', 17, struct count_info)
#define SNDCTL_DSP_GETOPTR		_IOR ('P', 18, struct count_info)
#define SNDCTL_DSP_MAPINBUF		_IOR ('P', 19, struct buffmem_desc)
#define SNDCTL_DSP_MAPOUTBUF		_IOR ('P', 20, struct buffmem_desc)
#define SNDCTL_DSP_SETSYNCRO		_IO  ('P', 21)
#define SNDCTL_DSP_SETDUPLEX		_IO  ('P', 22)
#define SNDCTL_DSP_PROFILE		_IOW ('P', 23, int)
#define	  APF_NORMAL			0
#define	  APF_NETWORK			1
#define   APF_CPUINTENS			2

#define	AFMT_QUERY			0x00000000
#define	AFMT_MU_LAW			0x00000001
#define	AFMT_A_LAW			0x00000002
#define	AFMT_IMA_ADPCM			0x00000004
#define	AFMT_U8				0x00000008
#define	AFMT_S16_LE			0x00000010
#define	AFMT_S16_BE			0x00000020
#define	AFMT_S8				0x00000040
#define	AFMT_U16_LE			0x00000080
#define	AFMT_U16_BE			0x00000100
#define	AFMT_MPEG			0x00000200

/* Mixer defines */
#define SOUND_MIXER_FIRST		0
#define SOUND_MIXER_NRDEVICES		17

#define SOUND_MIXER_VOLUME		0
#define SOUND_MIXER_BASS		1
#define SOUND_MIXER_TREBLE		2
#define SOUND_MIXER_SYNTH		3
#define SOUND_MIXER_PCM			4
#define SOUND_MIXER_SPEAKER		5
#define SOUND_MIXER_LINE		6
#define SOUND_MIXER_MIC			7
#define SOUND_MIXER_CD			8
#define SOUND_MIXER_IMIX		9
#define SOUND_MIXER_ALTPCM		10
#define SOUND_MIXER_RECLEV		11
#define SOUND_MIXER_IGAIN		12
#define SOUND_MIXER_OGAIN		13
#define SOUND_MIXER_LINE1		14
#define SOUND_MIXER_LINE2		15
#define SOUND_MIXER_LINE3		16

#define SOUND_MIXER_RECSRC		0xff
#define SOUND_MIXER_DEVMASK		0xfe
#define SOUND_MIXER_RECMASK		0xfd
#define SOUND_MIXER_CAPS		0xfc
#define  SOUND_CAP_EXCL_INPUT		1
#define SOUND_MIXER_STEREODEVS		0xfb

#define MIXER_READ(dev)			_IOR('M', dev, int)

#define SOUND_MIXER_READ_RECSRC		MIXER_READ(SOUND_MIXER_RECSRC)
#define SOUND_MIXER_READ_DEVMASK	MIXER_READ(SOUND_MIXER_DEVMASK)
#define SOUND_MIXER_READ_RECMASK	MIXER_READ(SOUND_MIXER_RECMASK)
#define SOUND_MIXER_READ_STEREODEVS	MIXER_READ(SOUND_MIXER_STEREODEVS)
#define SOUND_MIXER_READ_CAPS		MIXER_READ(SOUND_MIXER_CAPS)

#define SOUND_MIXER_READ_VOLUME		MIXER_READ(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_READ_BASS		MIXER_READ(SOUND_MIXER_BASS)
#define SOUND_MIXER_READ_TREBLE		MIXER_READ(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_READ_SYNTH		MIXER_READ(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_READ_PCM		MIXER_READ(SOUND_MIXER_PCM)
#define SOUND_MIXER_READ_SPEAKER	MIXER_READ(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_READ_LINE		MIXER_READ(SOUND_MIXER_LINE)
#define SOUND_MIXER_READ_MIC		MIXER_READ(SOUND_MIXER_MIC)
#define SOUND_MIXER_READ_CD		MIXER_READ(SOUND_MIXER_CD)
#define SOUND_MIXER_READ_IMIX		MIXER_READ(SOUND_MIXER_IMIX)
#define SOUND_MIXER_READ_ALTPCM		MIXER_READ(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_READ_RECLEV		MIXER_READ(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_READ_IGAIN		MIXER_READ(SOUND_MIXER_IGAIN)
#define SOUND_MIXER_READ_OGAIN		MIXER_READ(SOUND_MIXER_OGAIN)
#define SOUND_MIXER_READ_LINE1		MIXER_READ(SOUND_MIXER_LINE1)
#define SOUND_MIXER_READ_LINE2		MIXER_READ(SOUND_MIXER_LINE2)
#define SOUND_MIXER_READ_LINE3		MIXER_READ(SOUND_MIXER_LINE3)

#define MIXER_WRITE(dev)		_IOW ('M', dev, int)
#define MIXER_WRITE_R(dev)		_IOWR('M', dev, int)

#define SOUND_MIXER_WRITE_RECSRC	MIXER_WRITE(SOUND_MIXER_RECSRC)
#define SOUND_MIXER_WRITE_R_RECSRC	MIXER_WRITE_R(SOUND_MIXER_RECSRC)

#define SOUND_MIXER_WRITE_VOLUME	MIXER_WRITE(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_WRITE_BASS		MIXER_WRITE(SOUND_MIXER_BASS)
#define SOUND_MIXER_WRITE_TREBLE	MIXER_WRITE(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_WRITE_SYNTH		MIXER_WRITE(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_WRITE_PCM		MIXER_WRITE(SOUND_MIXER_PCM)
#define SOUND_MIXER_WRITE_SPEAKER	MIXER_WRITE(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_WRITE_LINE		MIXER_WRITE(SOUND_MIXER_LINE)
#define SOUND_MIXER_WRITE_MIC		MIXER_WRITE(SOUND_MIXER_MIC)
#define SOUND_MIXER_WRITE_CD		MIXER_WRITE(SOUND_MIXER_CD)
#define SOUND_MIXER_WRITE_IMIX		MIXER_WRITE(SOUND_MIXER_IMIX)
#define SOUND_MIXER_WRITE_ALTPCM	MIXER_WRITE(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_WRITE_RECLEV	MIXER_WRITE(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_WRITE_IGAIN		MIXER_WRITE(SOUND_MIXER_IGAIN)
#define SOUND_MIXER_WRITE_OGAIN		MIXER_WRITE(SOUND_MIXER_OGAIN)
#define SOUND_MIXER_WRITE_LINE1		MIXER_WRITE(SOUND_MIXER_LINE1)
#define SOUND_MIXER_WRITE_LINE2		MIXER_WRITE(SOUND_MIXER_LINE2)
#define SOUND_MIXER_WRITE_LINE3		MIXER_WRITE(SOUND_MIXER_LINE3)

#define SOUND_MASK_VOLUME	(1 << SOUND_MIXER_VOLUME)
#define SOUND_MASK_BASS		(1 << SOUND_MIXER_BASS)
#define SOUND_MASK_TREBLE	(1 << SOUND_MIXER_TREBLE)
#define SOUND_MASK_SYNTH	(1 << SOUND_MIXER_SYNTH)
#define SOUND_MASK_PCM		(1 << SOUND_MIXER_PCM)
#define SOUND_MASK_SPEAKER	(1 << SOUND_MIXER_SPEAKER)
#define SOUND_MASK_LINE		(1 << SOUND_MIXER_LINE)
#define SOUND_MASK_MIC		(1 << SOUND_MIXER_MIC)
#define SOUND_MASK_CD		(1 << SOUND_MIXER_CD)
#define SOUND_MASK_IMIX		(1 << SOUND_MIXER_IMIX)
#define SOUND_MASK_ALTPCM	(1 << SOUND_MIXER_ALTPCM)
#define SOUND_MASK_RECLEV	(1 << SOUND_MIXER_RECLEV)
#define SOUND_MASK_IGAIN	(1 << SOUND_MIXER_IGAIN)
#define SOUND_MASK_OGAIN	(1 << SOUND_MIXER_OGAIN)
#define SOUND_MASK_LINE1	(1 << SOUND_MIXER_LINE1)
#define SOUND_MASK_LINE2	(1 << SOUND_MIXER_LINE2)
#define SOUND_MASK_LINE3	(1 << SOUND_MIXER_LINE3)

#define OSS_GETVERSION			_IOR ('M', 118, int)

typedef struct audio_buf_info {
	int fragments;
	int fragstotal;
	int fragsize;
	int bytes;
} audio_buf_info;

typedef struct count_info {
	int bytes;
	int blocks;
	int ptr;
} count_info;

typedef struct buffmem_desc {
	unsigned int *buffer;
	int size;
} buffmem_desc;

#define ioctl(fd, com, argp) _oss_ioctl(fd, com, argp)

int _oss_ioctl(int fd, unsigned long com, void *argp);

#endif
