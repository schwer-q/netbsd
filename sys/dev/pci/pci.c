/*	$NetBSD: pci.c,v 1.34 1998/04/17 18:40:31 drochner Exp $	*/

/*
 * Copyright (c) 1995, 1996, 1997
 *     Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1994 Charles Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles Hannum.
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

/*
 * PCI bus autoconfiguration.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>

int pcimatch __P((struct device *, struct cfdata *, void *));
void pciattach __P((struct device *, struct device *, void *));

struct pci_softc {
	struct device sc_dev;
	bus_space_tag_t sc_iot, sc_memt;
	bus_dma_tag_t sc_dmat;
	pci_chipset_tag_t sc_pc;
	int sc_bus, sc_maxndevs;
	u_int sc_intrswiz;
	pcitag_t sc_intrtag;
	int sc_flags;
};

struct cfattach pci_ca = {
	sizeof(struct pci_softc), pcimatch, pciattach
};

void	pci_probe_bus __P((struct device *));
int	pciprint __P((void *, const char *));
int	pcisubmatch __P((struct device *, struct cfdata *, void *));

/*
 * Callback so that ISA/EISA bridges can attach their child busses
 * after PCI configuration is done.
 *
 * This works because:
 *	(1) there can be at most one ISA/EISA bridge per PCI bus, and
 *	(2) any ISA/EISA bridges must be attached to primary PCI
 *	    busses (i.e. bus zero).
 *
 * That boils down to: there can only be one of these outstanding
 * at a time, it is cleared when configuring PCI bus 0 before any
 * subdevices have been found, and it is run after all subdevices
 * of PCI bus 0 have been found.
 *
 * This is needed because there are some (legacy) PCI devices which
 * can show up as ISA/EISA devices as well (the prime example of which
 * are VGA controllers).  If you attach ISA from a PCI-ISA/EISA bridge,
 * and the bridge is seen before the video board is, the board can show
 * up as an ISA device, and that can (bogusly) complicate the PCI device's
 * attach code, or make the PCI device not be properly attached at all.
 */
static void	(*pci_isa_bridge_callback) __P((void *));
static void	*pci_isa_bridge_callback_arg;

int
pcimatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct pcibus_attach_args *pba = aux;

	if (strcmp(pba->pba_busname, cf->cf_driver->cd_name))
		return (0);

	/* Check the locators */
	if (cf->pcibuscf_bus != PCIBUS_UNK_BUS &&
	    cf->pcibuscf_bus != pba->pba_bus)
		return (0);

	/* sanity */
	if (pba->pba_bus < 0 || pba->pba_bus > 255)
		return (0);

	/*
	 * XXX check other (hardware?) indicators
	 */

	return 1;
}

void
pci_probe_bus(self)
	struct device *self;
{
	struct pci_softc *sc = (struct pci_softc *)self;
	bus_space_tag_t iot, memt;
	pci_chipset_tag_t pc;
	int bus, device, maxndevs, function, nfunctions;

	iot = sc->sc_iot;
	memt = sc->sc_memt;
	pc = sc->sc_pc;
	bus = sc->sc_bus;
	maxndevs = sc->sc_maxndevs;

	if (bus == 0)
		pci_isa_bridge_callback = NULL;

	for (device = 0; device < maxndevs; device++) {
		pcitag_t tag;
		pcireg_t id, class, intr, bhlcr, csr;
		struct pci_attach_args pa;
		int pin;

		tag = pci_make_tag(pc, bus, device, 0);
		id = pci_conf_read(pc, tag, PCI_ID_REG);

		/* Invalid vendor ID value? */
		if (PCI_VENDOR(id) == PCI_VENDOR_INVALID)
			continue;
		/* XXX Not invalid, but we've done this ~forever. */
		if (PCI_VENDOR(id) == 0)
			continue;

		bhlcr = pci_conf_read(pc, tag, PCI_BHLC_REG);
		nfunctions = PCI_HDRTYPE_MULTIFN(bhlcr) ? 8 : 1;

		for (function = 0; function < nfunctions; function++) {
			tag = pci_make_tag(pc, bus, device, function);
			id = pci_conf_read(pc, tag, PCI_ID_REG);
			csr = pci_conf_read(pc, tag, PCI_COMMAND_STATUS_REG);
			class = pci_conf_read(pc, tag, PCI_CLASS_REG);
			intr = pci_conf_read(pc, tag, PCI_INTERRUPT_REG);

			/* Invalid vendor ID value? */
			if (PCI_VENDOR(id) == PCI_VENDOR_INVALID)
				continue;
			/* XXX Not invalid, but we've done this ~forever. */
			if (PCI_VENDOR(id) == 0)
				continue;

			pa.pa_iot = iot;
			pa.pa_memt = memt;
			pa.pa_dmat = sc->sc_dmat;
			pa.pa_pc = pc;
			pa.pa_device = device;
			pa.pa_function = function;
			pa.pa_tag = tag;
			pa.pa_id = id;
			pa.pa_class = class;

			/* set up memory and I/O enable flags as appropriate */
			pa.pa_flags = 0;
			if ((sc->sc_flags & PCI_FLAGS_IO_ENABLED) &&
			    (csr & PCI_COMMAND_IO_ENABLE))
				pa.pa_flags |= PCI_FLAGS_IO_ENABLED;
			if ((sc->sc_flags & PCI_FLAGS_MEM_ENABLED) &&
			    (csr & PCI_COMMAND_MEM_ENABLE))
				pa.pa_flags |= PCI_FLAGS_MEM_ENABLED;

			if (bus == 0) {
				pa.pa_intrswiz = 0;
				pa.pa_intrtag = tag;
			} else {
				pa.pa_intrswiz = sc->sc_intrswiz + device;
				pa.pa_intrtag = sc->sc_intrtag;
			}
			pin = PCI_INTERRUPT_PIN(intr);
			if (pin == PCI_INTERRUPT_PIN_NONE) {
				/* no interrupt */
				pa.pa_intrpin = 0;
			} else {
				/*
				 * swizzle it based on the number of
				 * busses we're behind and our device
				 * number.
				 */
				pa.pa_intrpin =			/* XXX */
				    ((pin + pa.pa_intrswiz - 1) % 4) + 1;
			}
			pa.pa_intrline = PCI_INTERRUPT_LINE(intr);

			config_found_sm(self, &pa, pciprint, pcisubmatch);
		}
	}

	if (bus == 0 && pci_isa_bridge_callback != NULL)
		(*pci_isa_bridge_callback)(pci_isa_bridge_callback_arg);
}

void
pciattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct pcibus_attach_args *pba = aux;
	struct pci_softc *sc = (struct pci_softc *)self;
	int io_enabled, mem_enabled;

	pci_attach_hook(parent, self, pba);
	printf("\n");

	io_enabled = (pba->pba_flags & PCI_FLAGS_IO_ENABLED);
	mem_enabled = (pba->pba_flags & PCI_FLAGS_MEM_ENABLED);

	if (io_enabled == 0 && mem_enabled == 0) {
		printf("%s: no spaces enabled!\n", self->dv_xname);
		return;
	}

	printf("%s: ", self->dv_xname);
	if (io_enabled)
		printf("i/o enabled");
	if (mem_enabled) {
		if (io_enabled)
			printf(", ");
		printf("memory enabled");
	}
	printf("\n");

	sc->sc_iot = pba->pba_iot;
	sc->sc_memt = pba->pba_memt;
	sc->sc_dmat = pba->pba_dmat;
	sc->sc_pc = pba->pba_pc;
	sc->sc_bus = pba->pba_bus;
	sc->sc_maxndevs = pci_bus_maxdevs(pba->pba_pc, pba->pba_bus);
	sc->sc_intrswiz = pba->pba_intrswiz;
	sc->sc_intrtag = pba->pba_intrtag;
	sc->sc_flags = pba->pba_flags;

	pci_probe_bus(self);
}

int
pciprint(aux, pnp)
	void *aux;
	const char *pnp;
{
	register struct pci_attach_args *pa = aux;
	char devinfo[256];

	if (pnp) {
		pci_devinfo(pa->pa_id, pa->pa_class, 1, devinfo);
		printf("%s at %s", devinfo, pnp);
	}
	printf(" dev %d function %d", pa->pa_device, pa->pa_function);
#if 0
	printf(" (%si/o, %smem)",
	    pa->pa_flags & PCI_FLAGS_IO_ENABLED ? "" : "no ",
	    pa->pa_flags & PCI_FLAGS_MEM_ENABLED ? "" : "no ");
#endif
	return (UNCONF);
}

int
pcisubmatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct pci_attach_args *pa = aux;

	if (cf->pcicf_dev != PCI_UNK_DEV &&
	    cf->pcicf_dev != pa->pa_device)
		return 0;
	if (cf->pcicf_function != PCI_UNK_FUNCTION &&
	    cf->pcicf_function != pa->pa_function)
		return 0;
	return ((*cf->cf_attach->ca_match)(parent, cf, aux));
}

void
set_pci_isa_bridge_callback(fn, arg)
	void (*fn) __P((void *));
	void *arg;
{

	if (pci_isa_bridge_callback != NULL)
		panic("set_pci_isa_bridge_callback");
	pci_isa_bridge_callback = fn;
	pci_isa_bridge_callback_arg = arg;
}
