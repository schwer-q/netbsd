/* $NetBSD: if_mec.c,v 1.30 2008/08/14 03:48:43 tsutsui Exp $ */

/*-
 * Copyright (c) 2004 Izumi Tsutsui.  All rights reserved.
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
 * Copyright (c) 2003 Christopher SEKIYA
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
 *          This product includes software developed for the
 *          NetBSD Project.  See http://www.NetBSD.org/ for
 *          information about NetBSD.
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
 * MACE MAC-110 Ethernet driver
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: if_mec.c,v 1.30 2008/08/14 03:48:43 tsutsui Exp $");

#include "opt_ddb.h"
#include "bpfilter.h"
#include "rnd.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/callout.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#if NRND > 0
#include <sys/rnd.h>
#endif

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_ether.h>

#if NBPFILTER > 0
#include <net/bpf.h>
#endif

#include <machine/bus.h>
#include <machine/intr.h>
#include <machine/machtype.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

#include <sgimips/mace/macevar.h>
#include <sgimips/mace/if_mecreg.h>

#include <dev/arcbios/arcbios.h>
#include <dev/arcbios/arcbiosvar.h>

/* #define MEC_DEBUG */

#ifdef MEC_DEBUG
#define MEC_DEBUG_RESET		0x01
#define MEC_DEBUG_START		0x02
#define MEC_DEBUG_STOP		0x04
#define MEC_DEBUG_INTR		0x08
#define MEC_DEBUG_RXINTR	0x10
#define MEC_DEBUG_TXINTR	0x20
uint32_t mec_debug = 0;
#define DPRINTF(x, y)	if (mec_debug & (x)) printf y
#else
#define DPRINTF(x, y)	/* nothing */
#endif

/*
 * Transmit descriptor list size
 */
#define MEC_NTXDESC		64
#define MEC_NTXDESC_MASK	(MEC_NTXDESC - 1)
#define MEC_NEXTTX(x)		(((x) + 1) & MEC_NTXDESC_MASK)
#define MEC_NTXDESC_RSVD	4
#define MEC_NTXDESC_INTR	8

/*
 * software state for TX
 */
struct mec_txsoft {
	struct mbuf *txs_mbuf;		/* head of our mbuf chain */
	bus_dmamap_t txs_dmamap;	/* our DMA map */
	uint32_t txs_flags;
#define MEC_TXS_BUFLEN_MASK	0x0000007f	/* data len in txd_buf */
#define MEC_TXS_TXDBUF		0x00000080	/* txd_buf is used */
#define MEC_TXS_TXDPTR1		0x00000100	/* txd_ptr[0] is used */
};

/*
 * Transmit buffer descriptor
 */
#define MEC_TXDESCSIZE		128
#define MEC_NTXPTR		3
#define MEC_TXD_BUFOFFSET	\
	(sizeof(uint64_t) + MEC_NTXPTR * sizeof(uint64_t))
#define MEC_TXD_BUFSIZE		(MEC_TXDESCSIZE - MEC_TXD_BUFOFFSET)
#define MEC_TXD_BUFSTART(len)	(MEC_TXD_BUFSIZE - (len))
#define MEC_TXD_ALIGN		8
#define MEC_TXD_ROUNDUP(addr)	\
	(((addr) + (MEC_TXD_ALIGN - 1)) & ~((uint64_t)MEC_TXD_ALIGN - 1))

struct mec_txdesc {
	volatile uint64_t txd_cmd;
#define MEC_TXCMD_DATALEN	0x000000000000ffff	/* data length */
#define MEC_TXCMD_BUFSTART	0x00000000007f0000	/* start byte offset */
#define  TXCMD_BUFSTART(x)	((x) << 16)
#define MEC_TXCMD_TERMDMA	0x0000000000800000	/* stop DMA on abort */
#define MEC_TXCMD_TXINT		0x0000000001000000	/* INT after TX done */
#define MEC_TXCMD_PTR1		0x0000000002000000	/* valid 1st txd_ptr */
#define MEC_TXCMD_PTR2		0x0000000004000000	/* valid 2nd txd_ptr */
#define MEC_TXCMD_PTR3		0x0000000008000000	/* valid 3rd txd_ptr */
#define MEC_TXCMD_UNUSED	0xfffffffff0000000ULL	/* should be zero */

#define txd_stat	txd_cmd
#define MEC_TXSTAT_LEN		0x000000000000ffff	/* TX length */
#define MEC_TXSTAT_COLCNT	0x00000000000f0000	/* collision count */
#define MEC_TXSTAT_COLCNT_SHIFT	16
#define MEC_TXSTAT_LATE_COL	0x0000000000100000	/* late collision */
#define MEC_TXSTAT_CRCERROR	0x0000000000200000	/* */
#define MEC_TXSTAT_DEFERRED	0x0000000000400000	/* */
#define MEC_TXSTAT_SUCCESS	0x0000000000800000	/* TX complete */
#define MEC_TXSTAT_TOOBIG	0x0000000001000000	/* */
#define MEC_TXSTAT_UNDERRUN	0x0000000002000000	/* */
#define MEC_TXSTAT_COLLISIONS	0x0000000004000000	/* */
#define MEC_TXSTAT_EXDEFERRAL	0x0000000008000000	/* */
#define MEC_TXSTAT_COLLIDED	0x0000000010000000	/* */
#define MEC_TXSTAT_UNUSED	0x7fffffffe0000000ULL	/* should be zero */
#define MEC_TXSTAT_SENT		0x8000000000000000ULL	/* packet sent */

	uint64_t txd_ptr[MEC_NTXPTR];
#define MEC_TXPTR_UNUSED2	0x0000000000000007	/* should be zero */
#define MEC_TXPTR_DMAADDR	0x00000000fffffff8	/* TX DMA address */
#define MEC_TXPTR_LEN		0x0000ffff00000000ULL	/* buffer length */
#define  TXPTR_LEN(x)		((uint64_t)(x) << 32)
#define MEC_TXPTR_UNUSED1	0xffff000000000000ULL	/* should be zero */

	uint8_t txd_buf[MEC_TXD_BUFSIZE];
};

/*
 * Receive buffer size
 */
#define MEC_NRXDESC		16
#define MEC_NRXDESC_MASK	(MEC_NRXDESC - 1)
#define MEC_NEXTRX(x)		(((x) + 1) & MEC_NRXDESC_MASK)

/*
 * Receive buffer description
 */
#define MEC_RXDESCSIZE		4096	/* umm, should be 4kbyte aligned */
#define MEC_RXD_NRXPAD		3
#define MEC_RXD_DMAOFFSET	(1 + MEC_RXD_NRXPAD)
#define MEC_RXD_BUFOFFSET	(MEC_RXD_DMAOFFSET * sizeof(uint64_t))
#define MEC_RXD_BUFSIZE		(MEC_RXDESCSIZE - MEC_RXD_BUFOFFSET)

struct mec_rxdesc {
	volatile uint64_t rxd_stat;
#define MEC_RXSTAT_LEN		0x000000000000ffff	/* data length */
#define MEC_RXSTAT_VIOLATION	0x0000000000010000	/* code violation (?) */
#define MEC_RXSTAT_UNUSED2	0x0000000000020000	/* unknown (?) */
#define MEC_RXSTAT_CRCERROR	0x0000000000040000	/* CRC error */
#define MEC_RXSTAT_MULTICAST	0x0000000000080000	/* multicast packet */
#define MEC_RXSTAT_BROADCAST	0x0000000000100000	/* broadcast packet */
#define MEC_RXSTAT_INVALID	0x0000000000200000	/* invalid preamble */
#define MEC_RXSTAT_LONGEVENT	0x0000000000400000	/* long packet */
#define MEC_RXSTAT_BADPACKET	0x0000000000800000	/* bad packet */
#define MEC_RXSTAT_CAREVENT	0x0000000001000000	/* carrier event */
#define MEC_RXSTAT_MATCHMCAST	0x0000000002000000	/* match multicast */
#define MEC_RXSTAT_MATCHMAC	0x0000000004000000	/* match MAC */
#define MEC_RXSTAT_SEQNUM	0x00000000f8000000	/* sequence number */
#define MEC_RXSTAT_CKSUM	0x0000ffff00000000ULL	/* IP checksum */
#define MEC_RXSTAT_UNUSED1	0x7fff000000000000ULL	/* should be zero */
#define MEC_RXSTAT_RECEIVED	0x8000000000000000ULL	/* set to 1 on RX */
	uint64_t rxd_pad1[MEC_RXD_NRXPAD];
	uint8_t  rxd_buf[MEC_RXD_BUFSIZE];
};

/*
 * control structures for DMA ops
 */
struct mec_control_data {
	/*
	 * TX descriptors and buffers
	 */
	struct mec_txdesc mcd_txdesc[MEC_NTXDESC];

	/*
	 * RX descriptors and buffers
	 */
	struct mec_rxdesc mcd_rxdesc[MEC_NRXDESC];
};

/*
 * It _seems_ there are some restrictions on descriptor address:
 *
 * - Base address of txdescs should be 8kbyte aligned
 * - Each txdesc should be 128byte aligned
 * - Each rxdesc should be 4kbyte aligned
 *
 * So we should specify 8k align to allocalte txdescs.
 * In this case, sizeof(struct mec_txdesc) * MEC_NTXDESC is 8192
 * so rxdescs are also allocated at 4kbyte aligned.
 */
#define MEC_CONTROL_DATA_ALIGN	(8 * 1024)

#define MEC_CDOFF(x)	offsetof(struct mec_control_data, x)
#define MEC_CDTXOFF(x)	MEC_CDOFF(mcd_txdesc[(x)])
#define MEC_CDRXOFF(x)	MEC_CDOFF(mcd_rxdesc[(x)])

/*
 * software state per device
 */
struct mec_softc {
	device_t sc_dev;		/* generic device structures */

	bus_space_tag_t sc_st;		/* bus_space tag */
	bus_space_handle_t sc_sh;	/* bus_space handle */
	bus_dma_tag_t sc_dmat;		/* bus_dma tag */
	void *sc_sdhook;		/* shutdown hook */

	struct ethercom sc_ethercom;	/* Ethernet common part */

	struct mii_data sc_mii;		/* MII/media information */
	int sc_phyaddr;			/* MII address */
	struct callout sc_tick_ch;	/* tick callout */

	uint8_t sc_enaddr[ETHER_ADDR_LEN]; /* MAC address */

	bus_dmamap_t sc_cddmamap;	/* bus_dma map for control data */
#define sc_cddma	sc_cddmamap->dm_segs[0].ds_addr

	/* pointer to allocated control data */
	struct mec_control_data *sc_control_data;
#define sc_txdesc	sc_control_data->mcd_txdesc
#define sc_rxdesc	sc_control_data->mcd_rxdesc

	/* software state for TX descs */
	struct mec_txsoft sc_txsoft[MEC_NTXDESC];

	int sc_txpending;		/* number of TX requests pending */
	int sc_txdirty;			/* first dirty TX descriptor */
	int sc_txlast;			/* last used TX descriptor */

	int sc_rxptr;			/* next ready RX buffer */

#if NRND > 0
	rndsource_element_t sc_rnd_source; /* random source */
#endif
};

#define MEC_CDTXADDR(sc, x)	((sc)->sc_cddma + MEC_CDTXOFF(x))
#define MEC_CDRXADDR(sc, x)	((sc)->sc_cddma + MEC_CDRXOFF(x))

#define MEC_TXDESCSYNC(sc, x, ops)					\
	bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_cddmamap,		\
	    MEC_CDTXOFF(x), MEC_TXDESCSIZE, (ops))
#define MEC_TXCMDSYNC(sc, x, ops)					\
	bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_cddmamap,		\
	    MEC_CDTXOFF(x), sizeof(uint64_t), (ops))

#define MEC_RXSTATSYNC(sc, x, ops)					\
	bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_cddmamap,		\
	    MEC_CDRXOFF(x), sizeof(uint64_t), (ops))
#define MEC_RXBUFSYNC(sc, x, len, ops)					\
	bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_cddmamap,		\
	    MEC_CDRXOFF(x) + MEC_RXD_BUFOFFSET,				\
	    MEC_ETHER_ALIGN + (len), (ops))

/* XXX these values should be moved to <net/if_ether.h> ? */
#define ETHER_PAD_LEN	(ETHER_MIN_LEN - ETHER_CRC_LEN)
#define MEC_ETHER_ALIGN	2

static int	mec_match(device_t, cfdata_t, void *);
static void	mec_attach(device_t, device_t, void *);

static int	mec_mii_readreg(device_t, int, int);
static void	mec_mii_writereg(device_t, int, int, int);
static int	mec_mii_wait(struct mec_softc *);
static void	mec_statchg(device_t);

static void	enaddr_aton(const char *, uint8_t *);

static int	mec_init(struct ifnet * ifp);
static void	mec_start(struct ifnet *);
static void	mec_watchdog(struct ifnet *);
static void	mec_tick(void *);
static int	mec_ioctl(struct ifnet *, u_long, void *);
static void	mec_reset(struct mec_softc *);
static void	mec_setfilter(struct mec_softc *);
static int	mec_intr(void *arg);
static void	mec_stop(struct ifnet *, int);
static void	mec_rxintr(struct mec_softc *);
static void	mec_txintr(struct mec_softc *, uint32_t);
static void	mec_shutdown(void *);

CFATTACH_DECL_NEW(mec, sizeof(struct mec_softc),
    mec_match, mec_attach, NULL, NULL);

static int mec_matched = 0;

static int
mec_match(device_t parent, cfdata_t cf, void *aux)
{

	/* allow only one device */
	if (mec_matched)
		return 0;

	mec_matched = 1;
	return 1;
}

static void
mec_attach(device_t parent, device_t self, void *aux)
{
	struct mec_softc *sc = device_private(self);
	struct mace_attach_args *maa = aux;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	uint64_t address, command;
	const char *macaddr;
	struct mii_softc *child;
	bus_dma_segment_t seg;
	int i, err, rseg;
	bool mac_is_fake;

	sc->sc_dev = self;
	sc->sc_st = maa->maa_st;
	if (bus_space_subregion(sc->sc_st, maa->maa_sh,
	    maa->maa_offset, 0,	&sc->sc_sh) != 0) {
		aprint_error(": can't map i/o space\n");
		return;
	}

	/* set up DMA structures */
	sc->sc_dmat = maa->maa_dmat;

	/*
	 * Allocate the control data structures, and create and load the
	 * DMA map for it.
	 */
	if ((err = bus_dmamem_alloc(sc->sc_dmat,
	    sizeof(struct mec_control_data), MEC_CONTROL_DATA_ALIGN, 0,
	    &seg, 1, &rseg, BUS_DMA_NOWAIT)) != 0) {
		aprint_error(": unable to allocate control data, error = %d\n",
		    err);
		goto fail_0;
	}
	/*
	 * XXX needs re-think...
	 * control data structures contain whole RX data buffer, so
	 * BUS_DMA_COHERENT (which disables cache) may cause some performance
	 * issue on copying data from the RX buffer to mbuf on normal memory,
	 * though we have to make sure all bus_dmamap_sync(9) ops are called
	 * properly in that case.
	 */
	if ((err = bus_dmamem_map(sc->sc_dmat, &seg, rseg,
	    sizeof(struct mec_control_data),
	    (void **)&sc->sc_control_data, /*BUS_DMA_COHERENT*/ 0)) != 0) {
		aprint_error(": unable to map control data, error = %d\n", err);
		goto fail_1;
	}
	memset(sc->sc_control_data, 0, sizeof(struct mec_control_data));

	if ((err = bus_dmamap_create(sc->sc_dmat,
	    sizeof(struct mec_control_data), 1,
	    sizeof(struct mec_control_data), 0, 0, &sc->sc_cddmamap)) != 0) {
		aprint_error(": unable to create control data DMA map,"
		    " error = %d\n", err);
		goto fail_2;
	}
	if ((err = bus_dmamap_load(sc->sc_dmat, sc->sc_cddmamap,
	    sc->sc_control_data, sizeof(struct mec_control_data), NULL,
	    BUS_DMA_NOWAIT)) != 0) {
		aprint_error(": unable to load control data DMA map,"
		    " error = %d\n", err);
		goto fail_3;
	}

	/* create TX buffer DMA maps */
	for (i = 0; i < MEC_NTXDESC; i++) {
		if ((err = bus_dmamap_create(sc->sc_dmat,
		    MCLBYTES, 1, MCLBYTES, PAGE_SIZE, 0,
		    &sc->sc_txsoft[i].txs_dmamap)) != 0) {
			aprint_error(": unable to create tx DMA map %d,"
			    " error = %d\n", i, err);
			goto fail_4;
		}
	}

	callout_init(&sc->sc_tick_ch, 0);

	/* get Ethernet address from ARCBIOS */
	if ((macaddr = ARCBIOS->GetEnvironmentVariable("eaddr")) == NULL) {
		aprint_error(": unable to get MAC address!\n");
		goto fail_4;
	}
	/*
	 * On some machines the DS2502 chip storing the serial number/
	 * mac address is on the pci riser board - if this board is
	 * missing, ARCBIOS will not know a good ethernet address (but
	 * otherwise the machine will work fine).
	 */
	mac_is_fake = false;
	if (strcmp(macaddr, "ff:ff:ff:ff:ff:ff") == 0) {
		uint32_t ui = 0;
		const char * netaddr =
			ARCBIOS->GetEnvironmentVariable("netaddr");

		/*
		 * Create a MAC address by abusing the "netaddr" env var
		 */
		sc->sc_enaddr[0] = 0xf2;
		sc->sc_enaddr[1] = 0x0b;
		sc->sc_enaddr[2] = 0xa4;
		if (netaddr) {
			mac_is_fake = true;
			while (*netaddr) {
				int v = 0;
				while (*netaddr && *netaddr != '.') {
					if (*netaddr >= '0' && *netaddr <= '9')
						v = v*10 + (*netaddr - '0');
					netaddr++;
				}
				ui <<= 8;
				ui |= v;
				if (*netaddr == '.')
					netaddr++;
			}
		}
		memcpy(sc->sc_enaddr+3, ((uint8_t *)&ui)+1, 3);
	}
	if (!mac_is_fake)
		enaddr_aton(macaddr, sc->sc_enaddr);

	/* set the Ethernet address */
	address = 0;
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		address = address << 8;
		address |= sc->sc_enaddr[i];
	}
	bus_space_write_8(sc->sc_st, sc->sc_sh, MEC_STATION, address);

	/* reset device */
	mec_reset(sc);

	command = bus_space_read_8(sc->sc_st, sc->sc_sh, MEC_MAC_CONTROL);

	aprint_normal(": MAC-110 Ethernet, rev %u\n",
	    (u_int)((command & MEC_MAC_REVISION) >> MEC_MAC_REVISION_SHIFT));

	if (mac_is_fake)
		aprint_normal_dev(self,
		    "could not get ethernet address from firmware"
		    " - generated one from the \"netaddr\" environment"
		    " variable\n");
	aprint_normal_dev(self, "Ethernet address %s\n",
	    ether_sprintf(sc->sc_enaddr));

	/* Done, now attach everything */

	sc->sc_mii.mii_ifp = ifp;
	sc->sc_mii.mii_readreg = mec_mii_readreg;
	sc->sc_mii.mii_writereg = mec_mii_writereg;
	sc->sc_mii.mii_statchg = mec_statchg;

	/* Set up PHY properties */
	sc->sc_ethercom.ec_mii = &sc->sc_mii;
	ifmedia_init(&sc->sc_mii.mii_media, 0, ether_mediachange,
	    ether_mediastatus);
	mii_attach(self, &sc->sc_mii, 0xffffffff, MII_PHY_ANY,
	    MII_OFFSET_ANY, 0);

	child = LIST_FIRST(&sc->sc_mii.mii_phys);
	if (child == NULL) {
		/* No PHY attached */
		ifmedia_add(&sc->sc_mii.mii_media, IFM_ETHER | IFM_MANUAL,
		    0, NULL);
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER | IFM_MANUAL);
	} else {
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER | IFM_AUTO);
		sc->sc_phyaddr = child->mii_phy;
	}

	strcpy(ifp->if_xname, device_xname(self));
	ifp->if_softc = sc;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_ioctl = mec_ioctl;
	ifp->if_start = mec_start;
	ifp->if_watchdog = mec_watchdog;
	ifp->if_init = mec_init;
	ifp->if_stop = mec_stop;
	ifp->if_mtu = ETHERMTU;
	IFQ_SET_READY(&ifp->if_snd);

	if_attach(ifp);
	ether_ifattach(ifp, sc->sc_enaddr);

	/* establish interrupt */
	cpu_intr_establish(maa->maa_intr, maa->maa_intrmask, mec_intr, sc);

#if NRND > 0
	rnd_attach_source(&sc->sc_rnd_source, device_xname(self),
	    RND_TYPE_NET, 0);
#endif

	/* set shutdown hook to reset interface on powerdown */
	sc->sc_sdhook = shutdownhook_establish(mec_shutdown, sc);

	return;

	/*
	 * Free any resources we've allocated during the failed attach
	 * attempt.  Do this in reverse order and fall though.
	 */
 fail_4:
	for (i = 0; i < MEC_NTXDESC; i++) {
		if (sc->sc_txsoft[i].txs_dmamap != NULL)
			bus_dmamap_destroy(sc->sc_dmat,
			    sc->sc_txsoft[i].txs_dmamap);
	}
	bus_dmamap_unload(sc->sc_dmat, sc->sc_cddmamap);
 fail_3:
	bus_dmamap_destroy(sc->sc_dmat, sc->sc_cddmamap);
 fail_2:
	bus_dmamem_unmap(sc->sc_dmat, (void *)sc->sc_control_data,
	    sizeof(struct mec_control_data));
 fail_1:
	bus_dmamem_free(sc->sc_dmat, &seg, rseg);
 fail_0:
	return;
}

static int
mec_mii_readreg(device_t self, int phy, int reg)
{
	struct mec_softc *sc = device_private(self);
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	uint64_t val;
	int i;

	if (mec_mii_wait(sc) != 0)
		return 0;

	bus_space_write_8(st, sh, MEC_PHY_ADDRESS,
	    (phy << MEC_PHY_ADDR_DEVSHIFT) | (reg & MEC_PHY_ADDR_REGISTER));
	delay(25);
	bus_space_write_8(st, sh, MEC_PHY_READ_INITIATE, 1);
	delay(25);
	mec_mii_wait(sc);

	for (i = 0; i < 20; i++) {
		delay(30);

		val = bus_space_read_8(st, sh, MEC_PHY_DATA);

		if ((val & MEC_PHY_DATA_BUSY) == 0)
			return val & MEC_PHY_DATA_VALUE;
	}
	return 0;
}

static void
mec_mii_writereg(device_t self, int phy, int reg, int val)
{
	struct mec_softc *sc = device_private(self);
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;

	if (mec_mii_wait(sc) != 0) {
		printf("timed out writing %x: %x\n", reg, val);
		return;
	}

	bus_space_write_8(st, sh, MEC_PHY_ADDRESS,
	    (phy << MEC_PHY_ADDR_DEVSHIFT) | (reg & MEC_PHY_ADDR_REGISTER));

	delay(60);

	bus_space_write_8(st, sh, MEC_PHY_DATA, val & MEC_PHY_DATA_VALUE);

	delay(60);

	mec_mii_wait(sc);
}

static int
mec_mii_wait(struct mec_softc *sc)
{
	uint32_t busy;
	int i, s;

	for (i = 0; i < 100; i++) {
		delay(30);

		s = splhigh();
		busy = bus_space_read_8(sc->sc_st, sc->sc_sh, MEC_PHY_DATA);
		splx(s);

		if ((busy & MEC_PHY_DATA_BUSY) == 0)
			return 0;
#if 0
		if (busy == 0xffff) /* XXX ? */
			return 0;
#endif
	}

	printf("%s: MII timed out\n", device_xname(sc->sc_dev));
	return 1;
}

static void
mec_statchg(device_t self)
{
	struct mec_softc *sc = device_private(self);
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	uint32_t control;

	control = bus_space_read_8(st, sh, MEC_MAC_CONTROL);
	control &= ~(MEC_MAC_IPGT | MEC_MAC_IPGR1 | MEC_MAC_IPGR2 |
	    MEC_MAC_FULL_DUPLEX | MEC_MAC_SPEED_SELECT);

	/* must also set IPG here for duplex stuff ... */
	if ((sc->sc_mii.mii_media_active & IFM_FDX) != 0) {
		control |= MEC_MAC_FULL_DUPLEX;
	} else {
		/* set IPG */
		control |= MEC_MAC_IPG_DEFAULT;
	}

	bus_space_write_8(st, sh, MEC_MAC_CONTROL, control);
}

/*
 * XXX
 * maybe this function should be moved to common part
 * (sgimips/machdep.c or elsewhere) for all on-board network devices.
 */
static void
enaddr_aton(const char *str, uint8_t *eaddr)
{
	int i;
	char c;

	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		if (*str == ':')
			str++;

		c = *str++;
		if (isdigit(c)) {
			eaddr[i] = (c - '0');
		} else if (isxdigit(c)) {
			eaddr[i] = (toupper(c) + 10 - 'A');
		}
		c = *str++;
		if (isdigit(c)) {
			eaddr[i] = (eaddr[i] << 4) | (c - '0');
		} else if (isxdigit(c)) {
			eaddr[i] = (eaddr[i] << 4) | (toupper(c) + 10 - 'A');
		}
	}
}

static int
mec_init(struct ifnet *ifp)
{
	struct mec_softc *sc = ifp->if_softc;
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct mec_rxdesc *rxd;
	int i, rc;

	/* cancel any pending I/O */
	mec_stop(ifp, 0);

	/* reset device */
	mec_reset(sc);

	/* setup filter for multicast or promisc mode */
	mec_setfilter(sc);

	/* set the TX ring pointer to the base address */
	bus_space_write_8(st, sh, MEC_TX_RING_BASE, MEC_CDTXADDR(sc, 0));

	sc->sc_txpending = 0;
	sc->sc_txdirty = 0;
	sc->sc_txlast = MEC_NTXDESC - 1;

	/* put RX buffers into FIFO */
	for (i = 0; i < MEC_NRXDESC; i++) {
		rxd = &sc->sc_rxdesc[i];
		rxd->rxd_stat = 0;
		MEC_RXSTATSYNC(sc, i, BUS_DMASYNC_PREREAD);
		MEC_RXBUFSYNC(sc, i, ETHER_MAX_LEN, BUS_DMASYNC_PREREAD);
		bus_space_write_8(st, sh, MEC_MCL_RX_FIFO, MEC_CDRXADDR(sc, i));
	}
	sc->sc_rxptr = 0;

#if 0	/* XXX no info */
	bus_space_write_8(st, sh, MEC_TIMER, 0);
#endif

	/*
	 * MEC_DMA_TX_INT_ENABLE will be set later otherwise it causes
	 * spurious interrupts when TX buffers are empty
	 */
	bus_space_write_8(st, sh, MEC_DMA_CONTROL,
	    (MEC_RXD_DMAOFFSET << MEC_DMA_RX_DMA_OFFSET_SHIFT) |
	    (MEC_NRXDESC << MEC_DMA_RX_INT_THRESH_SHIFT) |
	    MEC_DMA_TX_DMA_ENABLE | /* MEC_DMA_TX_INT_ENABLE | */
	    MEC_DMA_RX_DMA_ENABLE | MEC_DMA_RX_INT_ENABLE);

	callout_reset(&sc->sc_tick_ch, hz, mec_tick, sc);

	if ((rc = ether_mediachange(ifp)) != 0)
		return rc;

	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;
	mec_start(ifp);

	return 0;
}

static void
mec_reset(struct mec_softc *sc)
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	uint64_t control;

	/* stop DMA first */
	bus_space_write_8(st, sh, MEC_DMA_CONTROL, 0);

	/* reset chip */
	bus_space_write_8(st, sh, MEC_MAC_CONTROL, MEC_MAC_CORE_RESET);
	delay(1000);
	bus_space_write_8(st, sh, MEC_MAC_CONTROL, 0);
	delay(1000);

	/* Default to 100/half and let auto-negotiation work its magic */
	control = MEC_MAC_SPEED_SELECT | MEC_MAC_FILTER_MATCHMULTI |
	    MEC_MAC_IPG_DEFAULT;

	bus_space_write_8(st, sh, MEC_MAC_CONTROL, control);
	/* stop DMA again for sanity */
	bus_space_write_8(st, sh, MEC_DMA_CONTROL, 0);

	DPRINTF(MEC_DEBUG_RESET, ("mec: control now %llx\n",
	    bus_space_read_8(st, sh, MEC_MAC_CONTROL)));
}

static void
mec_start(struct ifnet *ifp)
{
	struct mec_softc *sc = ifp->if_softc;
	struct mbuf *m0, *m;
	struct mec_txdesc *txd;
	struct mec_txsoft *txs;
	bus_dmamap_t dmamap;
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	uint64_t txdaddr;
	int error, firsttx, nexttx, opending;
	int len, bufoff, buflen, unaligned, txdlen;

	if ((ifp->if_flags & (IFF_RUNNING|IFF_OACTIVE)) != IFF_RUNNING)
		return;

	/*
	 * Remember the previous txpending and the first transmit descriptor.
	 */
	opending = sc->sc_txpending;
	firsttx = MEC_NEXTTX(sc->sc_txlast);

	DPRINTF(MEC_DEBUG_START,
	    ("mec_start: opending = %d, firsttx = %d\n", opending, firsttx));

	while (sc->sc_txpending < MEC_NTXDESC - 1) {
		/* Grab a packet off the queue. */
		IFQ_POLL(&ifp->if_snd, m0);
		if (m0 == NULL)
			break;
		m = NULL;

		/*
		 * Get the next available transmit descriptor.
		 */
		nexttx = MEC_NEXTTX(sc->sc_txlast);
		txd = &sc->sc_txdesc[nexttx];
		txs = &sc->sc_txsoft[nexttx];

		buflen = 0;
		bufoff = 0;
		txdaddr = 0; /* XXX gcc */
		txdlen = 0; /* XXX gcc */

		len = m0->m_pkthdr.len;

		DPRINTF(MEC_DEBUG_START,
		    ("mec_start: len = %d, nexttx = %d\n", len, nexttx));

		if (len < ETHER_PAD_LEN) {
			/*
			 * I don't know if MEC chip does auto padding,
			 * so if the packet is small enough,
			 * just copy it to the buffer in txdesc.
			 * Maybe this is the simple way.
			 */
			DPRINTF(MEC_DEBUG_START, ("mec_start: short packet\n"));

			IFQ_DEQUEUE(&ifp->if_snd, m0);
			bufoff = MEC_TXD_BUFSTART(ETHER_PAD_LEN);
			m_copydata(m0, 0, m0->m_pkthdr.len,
			    txd->txd_buf + bufoff);
			memset(txd->txd_buf + bufoff + len, 0,
			    ETHER_PAD_LEN - len);
			len = buflen = ETHER_PAD_LEN;

			txs->txs_flags = MEC_TXS_TXDBUF | buflen;
		} else {
			/*
			 * If the packet won't fit the buffer in txdesc,
			 * we have to use concatenate pointer to handle it.
			 * While MEC can handle up to three segments to
			 * concatenate, MEC requires that both the second and
			 * third segments have to be 8 byte aligned.
			 * Since it's unlikely for mbuf clusters, we use
			 * only the first concatenate pointer. If the packet
			 * doesn't fit in one DMA segment, allocate new mbuf
			 * and copy the packet to it.
			 *
			 * Besides, if the start address of the first segments
			 * is not 8 byte aligned, such part have to be copied
			 * to the txdesc buffer. (XXX see below comments)
	                 */
			DPRINTF(MEC_DEBUG_START, ("mec_start: long packet\n"));

			dmamap = txs->txs_dmamap;
			if (bus_dmamap_load_mbuf(sc->sc_dmat, dmamap, m0,
			    BUS_DMA_WRITE | BUS_DMA_NOWAIT) != 0) {
				DPRINTF(MEC_DEBUG_START,
				    ("mec_start: re-allocating mbuf\n"));
				MGETHDR(m, M_DONTWAIT, MT_DATA);
				if (m == NULL) {
					printf("%s: unable to allocate "
					    "TX mbuf\n",
					    device_xname(sc->sc_dev));
					break;
				}
				if (len > (MHLEN - MEC_ETHER_ALIGN)) {
					MCLGET(m, M_DONTWAIT);
					if ((m->m_flags & M_EXT) == 0) {
						printf("%s: unable to allocate "
						    "TX cluster\n",
						    device_xname(sc->sc_dev));
						m_freem(m);
						break;
					}
				}
				/*
				 * Each packet has the Ethernet header, so
				 * in many case the header isn't 4-byte aligned
				 * and data after the header is 4-byte aligned.
				 * Thus adding 2-byte offset before copying to
				 * new mbuf avoids unaligned copy and this may
				 * improve some performance.
				 * As noted above, unaligned part has to be
				 * copied to txdesc buffer so this may cause
				 * extra copy ops, but for now MEC always
				 * requires some data in txdesc buffer,
				 * so we always have to copy some data anyway.
				 */
				m->m_data += MEC_ETHER_ALIGN;
				m_copydata(m0, 0, len, mtod(m, void *));
				m->m_pkthdr.len = m->m_len = len;
				error = bus_dmamap_load_mbuf(sc->sc_dmat,
				    dmamap, m, BUS_DMA_WRITE | BUS_DMA_NOWAIT);
				if (error) {
					printf("%s: unable to load TX buffer, "
					    "error = %d\n",
					    device_xname(sc->sc_dev), error);
					m_freem(m);
					break;
				}
			}
			IFQ_DEQUEUE(&ifp->if_snd, m0);
			if (m != NULL) {
				m_freem(m0);
				m0 = m;
			}

			/* handle unaligned part */
			txdaddr = MEC_TXD_ROUNDUP(dmamap->dm_segs[0].ds_addr);
			txs->txs_flags = MEC_TXS_TXDPTR1;
			unaligned =
			    dmamap->dm_segs[0].ds_addr & (MEC_TXD_ALIGN - 1);
			DPRINTF(MEC_DEBUG_START,
			    ("mec_start: ds_addr = 0x%08x, unaligned = %d\n",
			    (u_int)dmamap->dm_segs[0].ds_addr, unaligned));
			if (unaligned != 0) {
				buflen = MEC_TXD_ALIGN - unaligned;
				bufoff = MEC_TXD_BUFSTART(buflen);
				DPRINTF(MEC_DEBUG_START,
				    ("mec_start: unaligned, "
				    "buflen = %d, bufoff = %d\n",
				    buflen, bufoff));
				memcpy(txd->txd_buf + bufoff,
				    mtod(m0, void *), buflen);
				txs->txs_flags |= MEC_TXS_TXDBUF | buflen;
			}
#if 1
			else {
				/*
				 * XXX needs hardware info XXX
				 * It seems MEC always requires some data
				 * in txd_buf[] even if buffer is
				 * 8-byte aligned otherwise DMA abort error
				 * occurs later...
				 */
				buflen = MEC_TXD_ALIGN;
				bufoff = MEC_TXD_BUFSTART(buflen);
				memcpy(txd->txd_buf + bufoff,
				    mtod(m0, void *), buflen);
				DPRINTF(MEC_DEBUG_START,
				    ("mec_start: aligned, "
				    "buflen = %d, bufoff = %d\n",
				    buflen, bufoff));
				txs->txs_flags |= MEC_TXS_TXDBUF | buflen;
				txdaddr += MEC_TXD_ALIGN;
			}
#endif
			txdlen  = len - buflen;
			DPRINTF(MEC_DEBUG_START,
			    ("mec_start: txdaddr = 0x%08llx, txdlen = %d\n",
			    txdaddr, txdlen));

			/*
			 * sync the DMA map for TX mbuf
			 *
			 * XXX unaligned part doesn't have to be sync'ed,
			 *     but it's harmless...
			 */
			bus_dmamap_sync(sc->sc_dmat, dmamap, 0,
			    dmamap->dm_mapsize,	BUS_DMASYNC_PREWRITE);
		}

#if NBPFILTER > 0
		/*
		 * Pass packet to bpf if there is a listener.
		 */
		if (ifp->if_bpf)
			bpf_mtap(ifp->if_bpf, m0);
#endif

		/*
		 * setup the transmit descriptor.
		 */
		txd->txd_cmd = (len - 1);

		/*
		 * Set MEC_TXCMD_TXINT every MEC_NTXDESC_INTR packets
		 * if more than half txdescs have been queued
		 * because TX_EMPTY interrupts will rarely happen
		 * if TX queue is so stacked.
		 */
		if (sc->sc_txpending > (MEC_NTXDESC / 2) &&
		    (nexttx & (MEC_NTXDESC_INTR - 1)) == 0)
			txd->txd_cmd |= MEC_TXCMD_TXINT;

		if (txs->txs_flags & MEC_TXS_TXDBUF)
			txd->txd_cmd |= TXCMD_BUFSTART(MEC_TXDESCSIZE - buflen);
		if (txs->txs_flags & MEC_TXS_TXDPTR1) {
			txd->txd_cmd |= MEC_TXCMD_PTR1;
			txd->txd_ptr[0] = TXPTR_LEN(txdlen - 1) | txdaddr;
			/*
			 * Store a pointer to the packet so we can
			 * free it later.
			 */
			txs->txs_mbuf = m0;
		} else {
			txd->txd_ptr[0] = 0;
			/*
			 * In this case all data are copied to buffer in txdesc,
			 * we can free TX mbuf here.
			 */
			m_freem(m0);
		}

		DPRINTF(MEC_DEBUG_START,
		    ("mec_start: txd_cmd = 0x%016llx, txd_ptr = 0x%016llx\n",
		    txd->txd_cmd, txd->txd_ptr[0]));
		DPRINTF(MEC_DEBUG_START,
		    ("mec_start: len = %d (0x%04x), buflen = %d (0x%02x)\n",
		    len, len, buflen, buflen));

		/* sync TX descriptor */
		MEC_TXDESCSYNC(sc, nexttx,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/* start TX */
		bus_space_write_8(st, sh, MEC_TX_RING_PTR, MEC_NEXTTX(nexttx));

		/* advance the TX pointer. */
		sc->sc_txpending++;
		sc->sc_txlast = nexttx;
	}

	if (sc->sc_txpending == MEC_NTXDESC - 1) {
		/* No more slots; notify upper layer. */
		ifp->if_flags |= IFF_OACTIVE;
	}

	if (sc->sc_txpending != opending) {
		/*
		 * If the transmitter was idle,
		 * reset the txdirty pointer and re-enable TX interrupt.
		 */
		if (opending == 0) {
			sc->sc_txdirty = firsttx;
			bus_space_write_8(st, sh, MEC_TX_ALIAS,
			    MEC_TX_ALIAS_INT_ENABLE);
		}

		/* Set a watchdog timer in case the chip flakes out. */
		ifp->if_timer = 5;
	}
}

static void
mec_stop(struct ifnet *ifp, int disable)
{
	struct mec_softc *sc = ifp->if_softc;
	struct mec_txsoft *txs;
	int i;

	DPRINTF(MEC_DEBUG_STOP, ("mec_stop\n"));

	ifp->if_timer = 0;
	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);

	callout_stop(&sc->sc_tick_ch);
	mii_down(&sc->sc_mii);

	/* release any TX buffers */
	for (i = 0; i < MEC_NTXDESC; i++) {
		txs = &sc->sc_txsoft[i];
		if ((txs->txs_flags & MEC_TXS_TXDPTR1) != 0) {
			bus_dmamap_unload(sc->sc_dmat, txs->txs_dmamap);
			m_freem(txs->txs_mbuf);
			txs->txs_mbuf = NULL;
		}
	}
}

static int
mec_ioctl(struct ifnet *ifp, u_long cmd, void *data)
{
	int s, error;

	s = splnet();

	error = ether_ioctl(ifp, cmd, data);
	if (error == ENETRESET) {
		/*
		 * Multicast list has changed; set the hardware filter
		 * accordingly.
		 */
		if (ifp->if_flags & IFF_RUNNING)
			error = mec_init(ifp);
		else
			error = 0;
	}

	/* Try to get more packets going. */
	mec_start(ifp);

	splx(s);
	return error;
}

static void
mec_watchdog(struct ifnet *ifp)
{
	struct mec_softc *sc = ifp->if_softc;

	printf("%s: device timeout\n", device_xname(sc->sc_dev));
	ifp->if_oerrors++;

	mec_init(ifp);
}

static void
mec_tick(void *arg)
{
	struct mec_softc *sc = arg;
	int s;

	s = splnet();
	mii_tick(&sc->sc_mii);
	splx(s);

	callout_reset(&sc->sc_tick_ch, hz, mec_tick, sc);
}

static void
mec_setfilter(struct mec_softc *sc)
{
	struct ethercom *ec = &sc->sc_ethercom;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct ether_multi *enm;
	struct ether_multistep step;
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	uint64_t mchash;
	uint32_t control, hash;
	int mcnt;

	control = bus_space_read_8(st, sh, MEC_MAC_CONTROL);
	control &= ~MEC_MAC_FILTER_MASK;

	if (ifp->if_flags & IFF_PROMISC) {
		control |= MEC_MAC_FILTER_PROMISC;
		bus_space_write_8(st, sh, MEC_MULTICAST, 0xffffffffffffffffULL);
		bus_space_write_8(st, sh, MEC_MAC_CONTROL, control);
		return;
	}

	mcnt = 0;
	mchash = 0;
	ETHER_FIRST_MULTI(step, ec, enm);
	while (enm != NULL) {
		if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
			/* set allmulti for a range of multicast addresses */
			control |= MEC_MAC_FILTER_ALLMULTI;
			bus_space_write_8(st, sh, MEC_MULTICAST,
			    0xffffffffffffffffULL);
			bus_space_write_8(st, sh, MEC_MAC_CONTROL, control);
			return;
		}

#define mec_calchash(addr)	(ether_crc32_be((addr), ETHER_ADDR_LEN) >> 26)

		hash = mec_calchash(enm->enm_addrlo);
		mchash |= 1 << hash;
		mcnt++;
		ETHER_NEXT_MULTI(step, enm);
	}

	ifp->if_flags &= ~IFF_ALLMULTI;

	if (mcnt > 0)
		control |= MEC_MAC_FILTER_MATCHMULTI;

	bus_space_write_8(st, sh, MEC_MULTICAST, mchash);
	bus_space_write_8(st, sh, MEC_MAC_CONTROL, control);
}

static int
mec_intr(void *arg)
{
	struct mec_softc *sc = arg;
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	uint32_t statreg, statack, txptr;
	int handled, sent;

	DPRINTF(MEC_DEBUG_INTR, ("mec_intr: called\n"));

	handled = sent = 0;

	for (;;) {
		statreg = bus_space_read_8(st, sh, MEC_INT_STATUS);

		DPRINTF(MEC_DEBUG_INTR,
		    ("mec_intr: INT_STAT = 0x%08x\n", statreg));

		statack = statreg & MEC_INT_STATUS_MASK;
		if (statack == 0)
			break;
		bus_space_write_8(st, sh, MEC_INT_STATUS, statack);

		handled = 1;

		if (statack &
		    (MEC_INT_RX_THRESHOLD |
		     MEC_INT_RX_FIFO_UNDERFLOW)) {
			mec_rxintr(sc);
		}

		if (statack &
		    (MEC_INT_TX_EMPTY |
		     MEC_INT_TX_PACKET_SENT |
		     MEC_INT_TX_ABORT)) {
			txptr = (statreg & MEC_INT_TX_RING_BUFFER_ALIAS)
			    >> MEC_INT_TX_RING_BUFFER_SHIFT;
			mec_txintr(sc, txptr);
			sent = 1;
			if ((statack & MEC_INT_TX_EMPTY) != 0) {
				/*
				 * disable TX interrupt to stop
				 * TX empty interrupt
				 */
				bus_space_write_8(st, sh, MEC_TX_ALIAS, 0);
				DPRINTF(MEC_DEBUG_INTR,
				    ("mec_intr: disable TX_INT\n"));
			}
		}

		if (statack &
		    (MEC_INT_TX_LINK_FAIL |
		     MEC_INT_TX_MEM_ERROR |
		     MEC_INT_TX_ABORT |
		     MEC_INT_RX_FIFO_UNDERFLOW |
		     MEC_INT_RX_DMA_UNDERFLOW)) {
			printf("%s: mec_intr: interrupt status = 0x%08x\n",
			    device_xname(sc->sc_dev), statreg);
			mec_init(ifp);
			break;
		}
	}

	if (sent && !IFQ_IS_EMPTY(&ifp->if_snd)) {
		/* try to get more packets going */
		mec_start(ifp);
	}

#if NRND > 0
	if (handled)
		rnd_add_uint32(&sc->sc_rnd_source, statreg);
#endif

	return handled;
}

static void
mec_rxintr(struct mec_softc *sc)
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct mbuf *m;
	struct mec_rxdesc *rxd;
	uint64_t rxstat;
	u_int len;
	int i;

	DPRINTF(MEC_DEBUG_RXINTR, ("mec_rxintr: called\n"));

	for (i = sc->sc_rxptr;; i = MEC_NEXTRX(i)) {
		rxd = &sc->sc_rxdesc[i];

		MEC_RXSTATSYNC(sc, i, BUS_DMASYNC_POSTREAD);
		rxstat = rxd->rxd_stat;

		DPRINTF(MEC_DEBUG_RXINTR,
		    ("mec_rxintr: rxstat = 0x%016llx, rxptr = %d\n",
		    rxstat, i));
		DPRINTF(MEC_DEBUG_RXINTR, ("mec_rxintr: rxfifo = 0x%08x\n",
		    (u_int)bus_space_read_8(st, sh, MEC_RX_FIFO)));

		if ((rxstat & MEC_RXSTAT_RECEIVED) == 0) {
			MEC_RXSTATSYNC(sc, i, BUS_DMASYNC_PREREAD);
			break;
		}

		len = rxstat & MEC_RXSTAT_LEN;

		if (len < ETHER_MIN_LEN ||
		    len > (MCLBYTES - MEC_ETHER_ALIGN)) {
			/* invalid length packet; drop it. */
			DPRINTF(MEC_DEBUG_RXINTR,
			    ("mec_rxintr: wrong packet\n"));
 dropit:
			ifp->if_ierrors++;
			rxd->rxd_stat = 0;
			MEC_RXSTATSYNC(sc, i, BUS_DMASYNC_PREREAD);
			bus_space_write_8(st, sh, MEC_MCL_RX_FIFO,
			    MEC_CDRXADDR(sc, i));
			continue;
		}

		if (rxstat &
		    (MEC_RXSTAT_BADPACKET |
		     MEC_RXSTAT_LONGEVENT |
		     MEC_RXSTAT_INVALID   |
		     MEC_RXSTAT_CRCERROR  |
		     MEC_RXSTAT_VIOLATION)) {
			printf("%s: mec_rxintr: status = 0x%016llx\n",
			    device_xname(sc->sc_dev), rxstat);
			goto dropit;
		}

		/*
		 * The MEC includes the CRC with every packet.  Trim
		 * it off here.
		 */
		len -= ETHER_CRC_LEN;

		/*
		 * now allocate an mbuf (and possibly a cluster) to hold
		 * the received packet.
		 */
		MGETHDR(m, M_DONTWAIT, MT_DATA);
		if (m == NULL) {
			printf("%s: unable to allocate RX mbuf\n",
			    device_xname(sc->sc_dev));
			goto dropit;
		}
		if (len > (MHLEN - MEC_ETHER_ALIGN)) {
			MCLGET(m, M_DONTWAIT);
			if ((m->m_flags & M_EXT) == 0) {
				printf("%s: unable to allocate RX cluster\n",
				    device_xname(sc->sc_dev));
				m_freem(m);
				m = NULL;
				goto dropit;
			}
		}

		/*
		 * Note MEC chip seems to insert 2 byte padding at the top of
		 * RX buffer, but we copy whole buffer to avoid unaligned copy.
		 */
		MEC_RXBUFSYNC(sc, i, len, BUS_DMASYNC_POSTREAD);
		memcpy(mtod(m, void *), rxd->rxd_buf, MEC_ETHER_ALIGN + len);
		MEC_RXBUFSYNC(sc, i, ETHER_MAX_LEN, BUS_DMASYNC_PREREAD);
		m->m_data += MEC_ETHER_ALIGN;

		/* put RX buffer into FIFO again */
		rxd->rxd_stat = 0;
		MEC_RXSTATSYNC(sc, i, BUS_DMASYNC_PREREAD);
		bus_space_write_8(st, sh, MEC_MCL_RX_FIFO, MEC_CDRXADDR(sc, i));

		m->m_pkthdr.rcvif = ifp;
		m->m_pkthdr.len = m->m_len = len;

		ifp->if_ipackets++;

#if NBPFILTER > 0
		/*
		 * Pass this up to any BPF listeners, but only
		 * pass it up the stack if it's for us.
		 */
		if (ifp->if_bpf)
			bpf_mtap(ifp->if_bpf, m);
#endif

		/* Pass it on. */
		(*ifp->if_input)(ifp, m);
	}

	/* update RX pointer */
	sc->sc_rxptr = i;
}

static void
mec_txintr(struct mec_softc *sc, uint32_t txptr)
{
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct mec_txdesc *txd;
	struct mec_txsoft *txs;
	bus_dmamap_t dmamap;
	uint64_t txstat;
	int i;
	u_int col;

	DPRINTF(MEC_DEBUG_TXINTR, ("mec_txintr: called\n"));

	for (i = sc->sc_txdirty; i != txptr && sc->sc_txpending != 0;
	    i = MEC_NEXTTX(i), sc->sc_txpending--) {
		txd = &sc->sc_txdesc[i];

		MEC_TXDESCSYNC(sc, i,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

		txstat = txd->txd_stat;
		DPRINTF(MEC_DEBUG_TXINTR,
		    ("mec_txintr: dirty = %d, txstat = 0x%016llx\n",
		    i, txstat));
		if ((txstat & MEC_TXSTAT_SENT) == 0) {
			MEC_TXCMDSYNC(sc, i, BUS_DMASYNC_PREREAD);
			break;
		}

		txs = &sc->sc_txsoft[i];
		if ((txs->txs_flags & MEC_TXS_TXDPTR1) != 0) {
			dmamap = txs->txs_dmamap;
			bus_dmamap_sync(sc->sc_dmat, dmamap, 0,
			    dmamap->dm_mapsize, BUS_DMASYNC_POSTWRITE);
			bus_dmamap_unload(sc->sc_dmat, dmamap);
			m_freem(txs->txs_mbuf);
			txs->txs_mbuf = NULL;
		}

		col = (txstat & MEC_TXSTAT_COLCNT) >> MEC_TXSTAT_COLCNT_SHIFT;
		ifp->if_collisions += col;

		if ((txstat & MEC_TXSTAT_SUCCESS) == 0) {
			printf("%s: TX error: txstat = 0x%016llx\n",
			    device_xname(sc->sc_dev), txstat);
			ifp->if_oerrors++;
		} else
			ifp->if_opackets++;
	}

	/* update the dirty TX buffer pointer */
	sc->sc_txdirty = i;
	DPRINTF(MEC_DEBUG_INTR,
	    ("mec_txintr: sc_txdirty = %2d, sc_txpending = %2d\n",
	    sc->sc_txdirty, sc->sc_txpending));

	/* cancel the watchdog timer if there are no pending TX packets */
	if (sc->sc_txpending == 0)
		ifp->if_timer = 0;
	if (sc->sc_txpending < MEC_NTXDESC - MEC_NTXDESC_RSVD)
		ifp->if_flags &= ~IFF_OACTIVE;
}

static void
mec_shutdown(void *arg)
{
	struct mec_softc *sc = arg;

	mec_stop(&sc->sc_ethercom.ec_if, 1);
	/* make sure to stop DMA etc. */
	mec_reset(sc);
}
