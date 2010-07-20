/*	$NetBSD: com_mv.c,v 1.2 2010/07/20 11:50:18 kiyohara Exp $	*/
/*
 * Copyright (c) 2007, 2010 KIYOHARA Takashi
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: com_mv.c,v 1.2 2010/07/20 11:50:18 kiyohara Exp $");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>
#include <sys/errno.h>
#include <sys/termios.h>

#include <dev/marvell/gtvar.h>
#include <dev/marvell/marvellreg.h>
#include <dev/marvell/marvellvar.h>

#include <dev/ic/comvar.h>

#include <prop/proplib.h>

#define MVUART_SIZE		0x20


static int mvuart_match(device_t, struct cfdata *, void *);
static void mvuart_attach(device_t, device_t, void *);

CFATTACH_DECL_NEW(mvuart_gt, sizeof(struct com_softc),
    mvuart_match, mvuart_attach, NULL, NULL);
CFATTACH_DECL_NEW(mvuart_mbus, sizeof(struct com_softc),
    mvuart_match, mvuart_attach, NULL, NULL);

#ifdef COM_REGMAP
#define MVUART_INIT_REGS(regs, tag, hdl, addr, size)		\
	do {							\
		int i;						\
								\
		regs.cr_iot = tag;				\
		regs.cr_ioh = hdl;				\
		regs.cr_iobase = addr;				\
		regs.cr_nports = size;				\
		for (i = 0; i < __arraycount(regs.cr_map); i++)	\
			regs.cr_map[i] = com_std_map[i] << 2;	\
	} while (0)
#else
#define MVUART_INIT_REGS(regs, tag, hdl, addr, size) \
	COM_INIT_REGS(regs, tag, hdl, addr)
#endif


/* ARGSUSED */
static int
mvuart_match(device_t parent, struct cfdata *match, void *aux)
{
	struct marvell_attach_args *mva = aux;
	bus_space_handle_t ioh;

	switch (mva->mva_model) {
#if 0
	case MARVELL_DISCOVERY_V:	/* Do we have ?? */
	case MARVELL_DISCOVERY_VI:	/* Do we have ?? */
#endif
	case MARVELL_ORION_1_88F1181:
	case MARVELL_ORION_1_88F5082:
	case MARVELL_ORION_1_88F5180N:
	case MARVELL_ORION_1_88F5181:
	case MARVELL_ORION_1_88F5182:
	case MARVELL_ORION_1_88F6082:
	case MARVELL_ORION_1_88W8660:
	case MARVELL_ORION_2_88F1281:
	case MARVELL_ORION_2_88F5281:
	case MARVELL_KIRKWOOD_88F6180:
	case MARVELL_KIRKWOOD_88F6192:
	case MARVELL_KIRKWOOD_88F6281:
	case MARVELL_MV78XX0_MV78100:
	case MARVELL_MV78XX0_MV78200:
		break;

	default:
		return 0;
	}

	if (com_is_console(mva->mva_iot, mva->mva_addr + mva->mva_offset, NULL))
		return 1;

	if (bus_space_subregion(mva->mva_iot, mva->mva_ioh, mva->mva_offset,
	    mva->mva_size, &ioh))
		return 0;
	if (!comprobe1(mva->mva_iot, ioh))
		return 0;
	mva->mva_size = MVUART_SIZE;
	return 1;
}

/* ARGSUSED */
static void
mvuart_attach(device_t parent, device_t self, void *aux)
{
	struct com_softc *sc = device_private(self);
	struct marvell_attach_args *mva = aux;
	bus_space_tag_t iot;
	bus_space_handle_t ioh;
	prop_dictionary_t dict = device_properties(self);

	sc->sc_dev = self;

	if (!prop_dictionary_get_uint32(dict, "frequency", &sc->sc_frequency)) {
		aprint_error(": no frequency property\n");
		return;
	}

	iot = mva->mva_iot;
	if (!com_is_console(iot, mva->mva_addr + mva->mva_offset, &ioh)) {
		if (bus_space_subregion(iot, mva->mva_ioh, mva->mva_offset,
		    mva->mva_size, &ioh)) {
			aprint_error(": can't map registers\n");
			return;
		}
	}
	MVUART_INIT_REGS(sc->sc_regs,
	    iot, ioh, mva->mva_addr + mva->mva_offset, mva->mva_size);

	com_attach_subr(sc);

	marvell_intr_establish(mva->mva_irq, IPL_SERIAL, comintr, sc);
}

#ifdef COM_REGMAP
int mvuart_cnattach(bus_space_tag_t, bus_addr_t, int, uint32_t, int);

int
mvuart_cnattach(bus_space_tag_t iot, bus_addr_t addr, int baud,
		uint32_t sysfreq, int mode)
{
	struct com_regs regs;

	MVUART_INIT_REGS(regs, iot, 0x0, addr, MVUART_SIZE);

	return comcnattach1(&regs, baud, sysfreq, COM_TYPE_16550_NOERS, mode);
}
#endif /* COM_REGMAP */
