/*	$NetBSD: msm6258.c,v 1.7 2002/04/02 15:19:00 isaki Exp $	*/

/*
 * Copyright (c) 2001 Tetsuya Isaki. All rights reserved.
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
 *      This product includes software developed by Tetsuya Isaki.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * OKI MSM6258 ADPCM voice synthesizer codec.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: msm6258.c,v 1.7 2002/04/02 15:19:00 isaki Exp $");

#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/select.h>
#include <sys/audioio.h>

#include <dev/audio_if.h>
#include <dev/auconv.h>
#include <dev/audiovar.h>
#include <dev/mulaw.h>
#include <dev/ic/msm6258var.h>


static inline u_char pcm2adpcm_step(short, short *, char *);
static inline void adpcm2pcm_step(u_char, short *, char *);


static int adpcm_estimindex[16] = {
	 1,  3,  5,  7,  9,  11,  13,  15,
	-1, -3, -5, -7, -9, -11, -13, -15
};

static int adpcm_estim[49] = {
	 16,  17,  19,  21,  23,  25,  28,  31,  34,  37,
	 41,  45,  50,  55,  60,  66,  73,  80,  88,  97,
	107, 118, 130, 143, 157, 173, 190, 209, 230, 253,
	279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 875, 963, 1060, 1166, 1282, 1411, 1552
};

static u_char adpcm_estimindex_correct[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

struct msm6258_codecvar {
	short	mc_amp;
	char	mc_estim;
};

struct msm6258_softc {
	struct device sc_dev;
	struct msm6258_codecvar *sc_mc;
	/* MD vars follow */
};

void *
msm6258_codec_init (void)
{
	struct msm6258_codecvar *r;

	r = malloc (sizeof(*r), M_DEVBUF, M_NOWAIT);
	if (r == 0)
		return 0;
	r->mc_amp = r->mc_estim = 0;

	return r;
}

static inline u_char
pcm2adpcm_step(short a, short *y, char *x)
{
	int c, d;
	register unsigned char b;

	a -= *y;
	d = adpcm_estim[(int) *x];
	c = a * 4  / d;

	if (c < 0) {
		b = (unsigned char)-c;
		if (b >= 8)
			b = 7;
		b |= 0x08;
	} else {
		b = (unsigned char)c;
		if (b >= 8)
			b = 7;
	}

	*y += (short)(adpcm_estimindex[b] * d / 8);
	*x += adpcm_estimindex_correct[b];
	if (*x < 0)
		*x = 0;
	else if (*x > 48)
		*x = 48;
	return b;
}

void
msm6258_ulinear8_to_adpcm(void *hdl, u_char *p, int cc)
{
	struct msm6258_softc *sc = hdl;
	struct msm6258_codecvar *mc = sc->sc_mc;
	char *x = &(mc->mc_estim);
	short *y = &(mc->mc_amp);
	register int i;
	u_char *d = p;
	u_char f;

	for (i = 0; i < cc; ) {
		f = pcm2adpcm_step(p[i++], y, x);
		*d++ = f + (pcm2adpcm_step(p[i++], y, x) << 4);
	}
}

void
msm6258_mulaw_to_adpcm(void *hdl, u_char *p, int cc)
{
	mulaw_to_ulinear8(hdl, p, cc);
	msm6258_ulinear8_to_adpcm(hdl, p, cc);
}

static inline void
adpcm2pcm_step(u_char b, short *y, char *x)
{
	short dl;
	short pcm = *y;
	int estim = *x;

	dl = adpcm_estim[estim] * adpcm_estimindex[b] / 8;
	pcm += dl;
	*y = pcm / 256;

	estim += adpcm_estimindex_correct[b];
	if (estim < 0)
		estim = 0;
	if (estim > 48)
		estim = 48;
	*x = estim;
}

/* ADPCM stream must be converted in order. */
u_char tmpbuf[AU_RING_SIZE]; /* XXX */

void
msm6258_adpcm_to_ulinear8(void *hdl, u_char *p, int cc)
{
	struct msm6258_softc *sc = hdl;
	struct msm6258_codecvar *mc = sc->sc_mc;
	char *x = &(mc->mc_estim);
	short *y = &(mc->mc_amp);
	u_char a, b;
	int i;

	/* cc may be even. XXX alignment? */
	memcpy(tmpbuf, p, cc/2);
	for (i = 0; i < cc/2;) {
		a = tmpbuf[i++];
		b = a & 0x0f;
		adpcm2pcm_step(b, y, x);
		*p++ = *y;
		b = a >> 4;
		adpcm2pcm_step(b, y, x);
		*p++ = *y;
	}
}

void
msm6258_adpcm_to_mulaw(void *hdl, u_char *p, int cc)
{
	msm6258_adpcm_to_ulinear8(hdl, p, cc);
	ulinear8_to_mulaw(hdl, p, cc*2);
}
