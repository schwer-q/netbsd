/*	$NetBSD: autoconf.h,v 1.1 2000/08/12 22:58:06 wdk Exp $	*/

/*
 * Copyright (c) 1994, 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/*
 * Machine-dependent structures of autoconfiguration
 */

#include <machine/bus.h>

struct confargs;

/* Handle device interrupt for  given unit of a driver */

typedef void* intr_arg_t;		/* pointer to some softc */
typedef int (*intr_handler_t) __P((intr_arg_t));

struct confargs {
	const char	*ca_name;	/* Device name. */
	int		ca_addr;	/* Device address. */
	bus_space_tag_t	ca_bustag;	/* parent bus tag */
	bus_dma_tag_t	ca_dmatag;      /* parent bus dma */
	bus_addr_t	ca_paddr;	/* physical address */
};

/* Locator aliases */
#define cf_addr	cf_loc[0]

int	badaddr __P((void *, u_int));
