/*	$NetBSD: asc.c,v 1.1 2000/08/12 22:58:55 wdk Exp $	*/
/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Wayne Knowles
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

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <sys/malloc.h>

#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsiconf.h>
#include <dev/scsipi/scsi_message.h>

#include <machine/cpu.h>
#include <machine/autoconf.h>
#include <machine/mainboard.h>
#include <machine/bus.h>

#include <mipsco/obio/rambo.h>

#include <dev/ic/ncr53c9xreg.h>
#include <dev/ic/ncr53c9xvar.h>

struct asc_softc {
	struct ncr53c9x_softc	sc_ncr53c9x;	/* glue to MI code */
        struct evcnt		sc_intrcnt; 	/* Interrupt counter */
	bus_space_tag_t		sc_bst;
	bus_space_handle_t	sc_bsh;		/* NCR 53c94 registers */
	bus_space_handle_t	dm_bsh;		/* RAMBO registers */
	bus_dma_tag_t		sc_dmat;
        bus_dmamap_t		sc_dmamap;
        caddr_t			*sc_dmaaddr;
	size_t			*sc_dmalen;
	size_t			sc_dmasize;
        size_t			sc_blkcnt;
	int			sc_flags;
#define DMA_IDLE	0x0
#define	DMA_PULLUP	0x1
#define	DMA_ACTIVE	0x2
#define	DMA_MAPLOADED	0x4    
        u_int32_t		dm_mode;
        int			dm_curseg;
};

static int	ascmatch  __P((struct device *, struct cfdata *, void *));
static void	ascattach __P((struct device *, struct device *, void *));

static struct asc_softc  *asc0;

struct cfattach asc_ca = {
	sizeof(struct asc_softc), ascmatch, ascattach
};

/*
 * Functions and the switch for the MI code.
 */
static u_char	asc_read_reg __P((struct ncr53c9x_softc *, int));
static void	asc_write_reg __P((struct ncr53c9x_softc *, int, u_char));
static int	asc_dma_isintr __P((struct ncr53c9x_softc *));
static void	asc_dma_reset __P((struct ncr53c9x_softc *));
static int	asc_dma_intr __P((struct ncr53c9x_softc *));
static int	asc_dma_setup __P((struct ncr53c9x_softc *, caddr_t *,
				    size_t *, int, size_t *));
static void	asc_dma_go __P((struct ncr53c9x_softc *));
static void	asc_dma_stop __P((struct ncr53c9x_softc *));
static int	asc_dma_isactive __P((struct ncr53c9x_softc *));

static struct ncr53c9x_glue asc_glue = {
	asc_read_reg,
	asc_write_reg,
	asc_dma_isintr,
	asc_dma_reset,
	asc_dma_intr,
	asc_dma_setup,
	asc_dma_go,
	asc_dma_stop,
	asc_dma_isactive,
	NULL,			/* gl_clear_latched_intr */
};

#define MAX_SCSI_XFER   (64*1024)
#define	MAX_DMA_SZ	MAX_SCSI_XFER
#define	DMA_SEGS	(MAX_DMA_SZ/NBPG)

static int
ascmatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	return 1;
}

static void
ascattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct confargs *ca = aux;
	struct asc_softc *esc = (void *)self;
	struct ncr53c9x_softc *sc = &esc->sc_ncr53c9x;

	/*
	 * Set up glue for MI code early; we use some of it here.
	 */
	sc->sc_glue = &asc_glue;

	esc->sc_bst = ca->ca_bustag;
	esc->sc_dmat = ca->ca_dmatag;

	if (bus_space_map(ca->ca_bustag, ca->ca_addr,
			  16*4,	/* sizeof (ncr53c9xreg) */
			  BUS_SPACE_MAP_LINEAR,
			  &esc->sc_bsh) != 0) {
		printf(": cannot map registers\n");
		return;
	}

	if (bus_space_map(ca->ca_bustag, RAMBO_BASE, sizeof(struct rambo_ch),
			  BUS_SPACE_MAP_LINEAR,
			  &esc->dm_bsh) != 0) {
		printf(": cannot map dma registers\n");
		return;
	}

        if (bus_dmamap_create(esc->sc_dmat, MAX_DMA_SZ,
			      DMA_SEGS, MAX_DMA_SZ, RB_BOUNDRY,
			      BUS_DMA_WAITOK,
			      &esc->sc_dmamap) != 0) {
		printf(": failed to create dmamap\n");
		return;
        }

	evcnt_attach_dynamic(&esc->sc_intrcnt, EVCNT_TYPE_INTR, NULL,
			     self->dv_xname, "intr");

	esc->sc_flags = DMA_IDLE;
	asc_dma_reset(sc);

	/* Other settings */
	sc->sc_id = 7;
	sc->sc_freq = 24;	/* 24 MHz clock */
	
	/*
	 * Setup for genuine NCR 53C94 SCSI Controller
	 */

	sc->sc_cfg1 = sc->sc_id | NCRCFG1_PARENB;
	sc->sc_cfg2 = NCRCFG2_SCSI2; /* | NCRCFG2_FE */
	sc->sc_cfg3 = 0; /* NCRCFG3_CDB; */
	sc->sc_rev = NCR_VARIANT_NCR53C94;

	sc->sc_minsync = (1000 / sc->sc_freq) * 5 / 4;
	sc->sc_maxxfer = MAX_SCSI_XFER;

#ifdef OLDNCR
	if (!NCR_READ_REG(sc, NCR_CFG3)) {
		printf(" [old revision]");
		sc->sc_cfg2 = 0;
		sc->sc_cfg3 = 0;
		sc->sc_minsync = 0;
	}
#endif

	ncr53c9x_attach(sc, NULL, NULL);

	ncr53c9x_dmaselect = 0;
	asc0 = esc;
}

/*
 * Glue functions.
 */

u_char
asc_read_reg(sc, reg)
	struct ncr53c9x_softc *sc;
	int reg;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

	return bus_space_read_1(esc->sc_bst, esc->sc_bsh, reg * 4 + 3);
}

void
asc_write_reg(sc, reg, val)
	struct ncr53c9x_softc *sc;
	int reg;
	u_char val;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

	bus_space_write_1(esc->sc_bst, esc->sc_bsh, reg * 4 + 3, val);
}

void
dma_status(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;
	int    count;
	int    stat;
	void   *addr;
	u_int32_t  tc;

	tc = (asc_read_reg(sc, NCR_TCM)<<8) + asc_read_reg(sc, NCR_TCL);
	count = bus_space_read_2(esc->sc_bst, esc->dm_bsh, RAMBO_BLKCNT);
	stat  = bus_space_read_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE);
	addr  = (void *)
	        bus_space_read_4(esc->sc_bst, esc->dm_bsh, RAMBO_CADDR);

	NCR_DMA(("rambo status: cnt=%x addr=%p stat=%08x tc=%04x "
		 "ncr_stat=0x%02x ncr_fifo=0x%02x\n",
		 count, addr, stat, tc, 
		 asc_read_reg(sc, NCR_STAT),
		 asc_read_reg(sc, NCR_FFLAG)));
}

int
asc_dma_isintr(sc)
	struct ncr53c9x_softc *sc;
{
	return NCR_READ_REG(sc, NCR_STAT) & NCRSTAT_INT;
}

void
asc_dma_reset(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

 	bus_space_write_2(esc->sc_bst, esc->dm_bsh, RAMBO_BLKCNT, 0);
	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE,
			  RB_CLRFIFO|RB_CLRERROR);
	DELAY(10);
 	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE, 0);

	if (esc->sc_flags & DMA_MAPLOADED)
		bus_dmamap_unload(esc->sc_dmat, esc->sc_dmamap);

	esc->sc_flags = DMA_IDLE;
}

/*
 * Setup a DMA transfer
 */

static int
asc_dma_setup(sc, addr, len, datain, dmasize)
	struct ncr53c9x_softc *sc;
	caddr_t *addr;
	size_t *len;
	int datain;
	size_t *dmasize;
{
	struct asc_softc *esc = (struct asc_softc *)sc;
	paddr_t paddr;
        size_t count, blocks;
	int prime, err;

#ifdef DIAGNOSTIC
	if (esc->sc_flags & DMA_ACTIVE) {
		dma_status(sc);
		panic("DMA active");
	}
#endif

	/* Flush FIFO from previous operation */

	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE,
			  RB_CLRFIFO|RB_CLRERROR);
 	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE, 0);

	esc->sc_dmaaddr = addr;
	esc->sc_dmalen  = len;
	esc->sc_dmasize = *dmasize;
	esc->sc_flags   = datain ? DMA_PULLUP : 0;

	NCR_DMA(("asc_dma_setup va=%p len=%d datain=%d count=%d\n",
		 *addr, *len, datain, esc->sc_dmasize));

	/* have dmamap for the transfering addresses */
	if (err=bus_dmamap_load(esc->sc_dmat, esc->sc_dmamap,
				*esc->sc_dmaaddr, esc->sc_dmasize,
				NULL /* kernel address */,   
				BUS_DMA_NOWAIT))
		panic("%s: bus_dmamap_load err=%d", sc->sc_dev.dv_xname, err);

	esc->sc_flags |= DMA_MAPLOADED;

	/* No cache flush required for R3000 processors */

	paddr  = esc->sc_dmamap->dm_segs[0].ds_addr;
	count  = esc->sc_dmamap->dm_segs[0].ds_len;
	blocks = (((u_int32_t)*addr & 0x3f) + count + 63) >> 6;

	esc->dm_mode = (datain ? RB_DMA_WR : RB_DMA_RD) | RB_DMA_ENABLE;
	if (esc->sc_dmamap->dm_nsegs > 1)
		esc->dm_mode |= RB_INT_ENABLE;	/* Requires DMA chaining */

	/* Load DMA transfer address */
 	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_LADDR,
			  paddr & ~0x3f);

	/* Set count to zero bytes as this will prevent DMA from starting */
 	bus_space_write_2(esc->sc_bst, esc->dm_bsh, RAMBO_BLKCNT, 0);

	/* Set transfer direction and enable DMA FIFO */
 	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE, esc->dm_mode);

	/* If non block-aligned transfer prime FIFO manually */ 
	prime = (u_int32_t)*esc->sc_dmaaddr & 0x3f;
	if (prime) {
		if (esc->sc_flags & DMA_PULLUP) {
			u_int16_t *p;
			p = (u_int16_t *)((u_int32_t)*esc->sc_dmaaddr & ~0x3f);
			/* Read from NCR 53c94 controller*/
			while (prime > 0) {
				bus_space_write_2(esc->sc_bst, esc->dm_bsh,
						  RAMBO_FIFO, *p++);
				prime -= 2;
			}
		} else {
			/* Fetch the first block */
			bus_space_write_2(esc->sc_bst, esc->dm_bsh,
					  RAMBO_BLKCNT, 1);
			while (prime > 0) {
				(void)bus_space_read_2(esc->sc_bst,
						       esc->dm_bsh,
						       RAMBO_FIFO);
				prime -= 2;
			}
		}
	}
	esc->sc_blkcnt = blocks;
	esc->dm_curseg = 0;
	return 0;
}

void
asc_dma_go(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

	/* Load block count to start transfer */
 	bus_space_write_2(esc->sc_bst, esc->dm_bsh,
			  RAMBO_BLKCNT, esc->sc_blkcnt);
	esc->sc_flags |= DMA_ACTIVE;
}

int
asc_dma_intr(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

	size_t      resid, len;
	int         trans;
	u_int32_t   status;
	u_int tcl, tcm;

#ifdef DIAGNOSTIC
	if (!(esc->sc_flags & DMA_ACTIVE)) {
		dma_status(sc);
		panic("DMA not active");
	}
#endif

#if 0
	if ((resid = (NCR_READ_REG(sc, NCR_FFLAG) & NCRFIFO_FF)) != 0) {
		NCR_DMA(("asc_intr: empty FIFO of %d ", resid));
		DELAY(10);
	}
#endif
	resid = (tcl = NCR_READ_REG(sc, NCR_TCL)) +
		((tcm = NCR_READ_REG(sc, NCR_TCM)) << 8);
	trans = esc->sc_dmasize - resid;
	if (trans < 0) {			/* transferred < 0 ? */
		printf("asc_intr: xfer (%d) > req (%d)\n",
		       trans, esc->sc_dmasize);
		trans = esc->sc_dmasize;
	}

	NCR_DMA(("asc_intr: tcl=%d, tcm=%d; trans=%d, resid=%d\n",
		 tcl, tcm, trans, resid));

	status = bus_space_read_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE);
	resid  = status & 0x1f;

	if (!(status & RB_FIFO_EMPTY)) { /* Data left in RAMBO FIFO */
		if (esc->sc_flags & DMA_PULLUP) { /* SCSI Read */
			paddr_t ptr;
			u_int16_t *p;

			/* take the address of block to fixed up */
			ptr = bus_space_read_4(esc->sc_bst, esc->dm_bsh,
					       RAMBO_CADDR);
			/* find the starting address of fractional data */
			p = (u_int16_t *)MIPS_PHYS_TO_KSEG1(ptr+(resid<<1));

			/* XXX - disable DMA xfer before flushing FIFO ? */
			len = RB_BLK_CNT - resid;
			while (len--) {
				bus_space_write_2(esc->sc_bst, esc->dm_bsh,
						  RAMBO_FIFO, *p++);
			}
		} else {		/* SCSI Write */
			bus_space_write_4(esc->sc_bst, esc->dm_bsh, 
					  RAMBO_MODE, RB_CLRFIFO);
		}		
	}

	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE, 0);

	bus_dmamap_sync(esc->sc_dmat, esc->sc_dmamap,
			0, esc->sc_dmasize,
			(esc->sc_flags & DMA_PULLUP)
			  ? BUS_DMASYNC_POSTREAD
			  : BUS_DMASYNC_POSTWRITE);
	bus_dmamap_unload(esc->sc_dmat, esc->sc_dmamap);
   
	*esc->sc_dmaaddr += trans;
	*esc->sc_dmalen  -= trans;

	esc->sc_flags = DMA_IDLE;

	return 0;
}


void
asc_dma_stop(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;

	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE, 0);
	if (esc->sc_flags & DMA_MAPLOADED)
		bus_dmamap_unload(esc->sc_dmat, esc->sc_dmamap);
	esc->sc_flags = DMA_IDLE;
}

int
asc_dma_isactive(sc)
	struct ncr53c9x_softc *sc;
{
	struct asc_softc *esc = (struct asc_softc *)sc;
	return (esc->sc_flags & DMA_ACTIVE)? 1 : 0;
}

void
rambo_dma_chain(esc)
	struct asc_softc *esc;
{
	int seg;
	size_t	count, blocks;
	paddr_t paddr;

	seg = ++esc->dm_curseg;

	/* XXX: Check rambo status */

#ifdef DIAGNOSTIC
	if (!(esc->sc_flags & DMA_ACTIVE) || seg > esc->sc_dmamap->dm_nsegs)
		panic("Unexpected DMA chaining intr");
#endif

	paddr  = esc->sc_dmamap->dm_segs[seg].ds_addr;
	count  = esc->sc_dmamap->dm_segs[seg].ds_len;
	blocks = (count + 63) >> 6;

	/* Disable DMA interrupt if last segment */
	if (seg+1 > esc->sc_dmamap->dm_nsegs) {
		bus_space_write_4(esc->sc_bst, esc->dm_bsh,
				  RAMBO_MODE, esc->dm_mode & ~RB_INT_ENABLE);
	}
	
	/* Load transfer address for next DMA chain */
 	bus_space_write_4(esc->sc_bst, esc->dm_bsh, RAMBO_LADDR, paddr);

	/* DMA restarts when we enter a new block count */
 	bus_space_write_2(esc->sc_bst, esc->dm_bsh, RAMBO_BLKCNT, blocks);
}    

void
asc_intr()
{
    register u_int32_t dma_stat;
    struct asc_softc *esc = asc0;
    struct ncr53c9x_softc *sc = (void *)asc0;

    esc->sc_intrcnt.ev_count++;

    /* Check for NCR 53c94 interrupt */
    if (NCR_READ_REG(sc, NCR_STAT) & NCRSTAT_INT) {
	    ncr53c9x_intr((struct ncr53c9x_softc *) asc0);
    }
    /* Check for RAMBO DMA Interrupt */
    dma_stat = bus_space_read_4(esc->sc_bst, esc->dm_bsh, RAMBO_MODE);
    if (dma_stat & RB_INTR_PEND) {
	    rambo_dma_chain(esc);
    }
}
