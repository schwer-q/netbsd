/* $NetBSD: dec_3000_500.c,v 1.19 1998/02/13 00:12:46 thorpej Exp $ */

/*
 * Copyright (c) 1994, 1995, 1996 Carnegie-Mellon University.
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
 * Additional Copyright (c) 1997 by Matthew Jacob for NASA/Ames Research Center
 */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: dec_3000_500.c,v 1.19 1998/02/13 00:12:46 thorpej Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <machine/rpb.h>

#include <machine/autoconf.h>
#include <machine/conf.h>

#include <dev/tc/tcvar.h>

#include <alpha/tc/tcdsvar.h>

#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsiconf.h>

void dec_3000_500_init __P((void));
static void dec_3000_500_device_register __P((struct device *, void *));

static const char dec_3000_500_sp[] = "DEC 3000/400 (\"Sandpiper\")";
static const char dec_3000_500_sf[] = "DEC 3000/500 (\"Flamingo\")";

const struct alpha_variation_table dec_3000_500_variations[] = {
	{ SV_ST_SANDPIPER, dec_3000_500_sp },
	{ SV_ST_FLAMINGO, dec_3000_500_sf },
	{ SV_ST_HOTPINK, "DEC 3000/500X (\"Hot Pink\")" },
	{ SV_ST_FLAMINGOPLUS, "DEC 3000/800 (\"Flamingo+\")" },
	{ SV_ST_SANDPLUS, "DEC 3000/600 (\"Sandpiper+\")" },
	{ SV_ST_SANDPIPER45, "DEC 3000/700 (\"Sandpiper45\")" },
	{ SV_ST_FLAMINGO45, "DEC 3000/900 (\"Flamingo45\")" },
	{ 0, NULL },
};

void
dec_3000_500_init()
{
	u_int64_t variation;

	platform.family = "DEC 3000/500 (\"Flamingo\")";

	if ((platform.model = alpha_dsr_sysname()) == NULL) {
		variation = hwrpb->rpb_variation & SV_ST_MASK;
		if (variation == SV_ST_ULTRA) {
			/* These are really the same. */
			variation = SV_ST_FLAMINGOPLUS;
		}
		if ((platform.model = alpha_variation_name(variation,
		    dec_3000_500_variations)) == NULL) {
			/*
			 * This is how things used to be done.
			 */
			if (variation == SV_ST_RESERVED) {
				if (hwrpb->rpb_variation & SV_GRAPHICS)
					platform.model = dec_3000_500_sf;
				else
					platform.model = dec_3000_500_sp;
			} else
				platform.model = alpha_unknown_sysname();
		}
	}

	platform.iobus = "tcasic";
	platform.device_register = dec_3000_500_device_register;
}

static void
dec_3000_500_device_register(dev, aux)
	struct device *dev;
	void *aux;
{
	static int found, initted, scsiboot, netboot;
	static struct device *scsidev;
	struct bootdev_data *b = bootdev_data;
	struct device *parent = dev->dv_parent;
	struct cfdata *cf = dev->dv_cfdata;
	struct cfdriver *cd = cf->cf_driver;

	if (found)
		return;

	if (!initted) {
		scsiboot = (strcmp(b->protocol, "SCSI") == 0);
		netboot = (strcmp(b->protocol, "BOOTP") == 0);
#if 0
		printf("scsiboot = %d, netboot = %d\n", scsiboot, netboot);
#endif
		initted =1;
	}

	if (scsiboot && (strcmp(cd->cd_name, "asc") == 0)) {
		if (b->slot == 6 &&
		    strcmp(parent->dv_cfdata->cf_driver->cd_name, "tcds")
		      == 0) {
			struct tcdsdev_attach_args *tcdsdev = aux;

			if (tcdsdev->tcdsda_slot == b->channel) {
				scsidev = dev;
#if 0
				printf("\nscsidev = %s\n", dev->dv_xname);
#endif
			}
		}
	}

	if (scsiboot &&
	    (strcmp(cd->cd_name, "sd") == 0 ||
	     strcmp(cd->cd_name, "st") == 0 ||
	     strcmp(cd->cd_name, "cd") == 0)) {
		struct scsipibus_attach_args *sa = aux;

		if (scsidev == NULL)
			return;

		if (parent->dv_parent != scsidev)
			return;

		if (b->unit / 100 != sa->sa_sc_link->scsipi_scsi.target)
			return;

		/* XXX LUN! */

		switch (b->boot_dev_type) {
		case 0:
			if (strcmp(cd->cd_name, "sd") &&
			    strcmp(cd->cd_name, "cd"))
				return;
			break;
		case 1:
			if (strcmp(cd->cd_name, "st"))
				return;
			break;
		default:
			return;
		}

		/* we've found it! */
		booted_device = dev;
#if 0
		printf("\nbooted_device = %s\n", booted_device->dv_xname);
#endif
		found = 1;
	}

	if (netboot) {
                if (b->slot == 7 && strcmp(cd->cd_name, "le") == 0 &&
		    strcmp(parent->dv_cfdata->cf_driver->cd_name, "ioasic")
		     == 0) {
			/*
			 * no need to check ioasic_attach_args, since only
			 * one le on ioasic.
			 */

			booted_device = dev;
#if 0
			printf("\nbooted_device = %s\n", booted_device->dv_xname);
#endif
			found = 1;
			return;
		}

		/*
		 * XXX GENERIC SUPPORT FOR TC NETWORK BOARDS
		 */
        }
}
