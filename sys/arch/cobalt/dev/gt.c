/*	$NetBSD: gt.c,v 1.10 2004/08/28 12:32:48 tsutsui Exp $	*/

/*
 * Copyright (c) 2000 Soren S. Jorvang.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
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
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: gt.c,v 1.10 2004/08/28 12:32:48 tsutsui Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/device.h>

#include <machine/autoconf.h>
#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/pci/pcivar.h>

#include <cobalt/cobalt/clockvar.h>
#include <cobalt/dev/gtreg.h>

#include "pci.h"

struct gt_softc {
	struct device	sc_dev;

	bus_space_tag_t sc_bst;
	bus_space_handle_t sc_bsh;
};

static int	gt_match(struct device *, struct cfdata *, void *);
static void	gt_attach(struct device *, struct device *, void *);
static int	gt_print(void *aux, const char *pnp);

static void	gt_timer_init(struct gt_softc *sc);
static void	gt_timer0_init(void *);
static long	gt_timer0_read(void *);

CFATTACH_DECL(gt, sizeof(struct gt_softc),
    gt_match, gt_attach, NULL, NULL);

static int
gt_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	return 1;
}

#define GT_REG_REGION	0x1000

static void
gt_attach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct mainbus_attach_args *ma = aux;
	struct gt_softc *sc = (void *)self;
#if NPCI > 0
	struct pcibus_attach_args pba;
#endif

	sc->sc_bst = ma->ma_iot;
	if (bus_space_map(sc->sc_bst, ma->ma_addr, GT_REG_REGION,
	    0, &sc->sc_bsh)) {
		printf(": unable to map GT64111 registers\n");
		return;
	}

	printf("\n");

	gt_timer_init(sc);

	bus_space_write_4(sc->sc_bst, sc->sc_bsh, GT_PCI_COMMAND,
	    (bus_space_read_4(sc->sc_bst, sc->sc_bsh, GT_PCI_COMMAND) &
	    ~PCI_SYNCMODE) | PCI_PCLK_HIGH);

#if NPCI > 0
	pba.pba_busname = "pci";
	pba.pba_dmat = &pci_bus_dma_tag;
	pba.pba_dmat64 = NULL;
	pba.pba_flags = PCI_FLAGS_IO_ENABLED | PCI_FLAGS_MEM_ENABLED;
	pba.pba_bus = 0;
	pba.pba_bridgetag = NULL;
	pba.pba_flags = PCI_FLAGS_IO_ENABLED | PCI_FLAGS_MEM_ENABLED |
		PCI_FLAGS_MRL_OKAY | /*PCI_FLAGS_MRM_OKAY|*/ PCI_FLAGS_MWI_OKAY;
	config_found(self, &pba, gt_print);
#endif
}

static int
gt_print(aux, pnp)
	void *aux;
	const char *pnp;
{
	/* XXX */
	return 0;
}

static void
gt_timer_init(struct gt_softc *sc)
{

	/* stop timer0 */
	bus_space_write_4(sc->sc_bst, sc->sc_bsh, GT_TIMER_CTRL,
	    bus_space_read_4(sc->sc_bst, sc->sc_bsh, GT_TIMER_CTRL) & ~ENTC0);

	timer_start = gt_timer0_init;
	timer_read  = gt_timer0_read;
	timer_cookie = sc;
}

#define TIMER0_INIT_VALUE 500000

static void
gt_timer0_init(void *cookie)
{
	struct gt_softc *sc = cookie;

	bus_space_write_4(sc->sc_bst, sc->sc_bsh,
	    GT_TIMER_COUNTER0, TIMER0_INIT_VALUE);
	/* start timer0 */
	bus_space_write_4(sc->sc_bst, sc->sc_bsh, GT_TIMER_CTRL,
	    bus_space_read_4(sc->sc_bst, sc->sc_bsh, GT_TIMER_CTRL) | ENTC0);
}

static long
gt_timer0_read(void *cookie)
{
	struct gt_softc *sc = cookie;
	uint32_t counter0;

	counter0 = bus_space_read_4(sc->sc_bst, sc->sc_bsh, GT_TIMER_COUNTER0);
	counter0 = TIMER0_INIT_VALUE - counter0;
#if 0
	counter /= 50;
#else
	/*
	 * From pmax/pmax/dec_3min.c:
	 * 1/64 + 1/256 + 1/2048 = 41/2048 = 1/49.9512...
	 */
	counter0 = (counter0 >> 6) + (counter0 >> 8) + (counter0 >> 11);
#endif
	return counter0;
}
