/*	$NetBSD: conf-glue.c,v 1.23 2000/01/10 03:24:36 simonb Exp $	*/

/*
 * conf-glue.c:
 * A hand-edited ioconf.c, as generated by config.old program
 *
 * Seriously munged  to support old-stype pmax configurations
 * configured and compiled with new config.
 * Since new config doesn't yet support most pmax drivers,
 * this file contains an driver configuration table produced
 * by config.old run on a `generic' configuration.
 * This table uses device counts produced by the new config
 * ``needs-count'' keyword, that tells us at compile time
 * the maximum number of each device  that were configured.
 *
 * THIS MAY NOT WORK FOR ALL MACHINES.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <pmax/dev/device.h>

#define C (char *)

/*
 * Get old-style count of how many devices were configured
 * for each driver.
 */
#include "asc.h"
#include "sii.h"
#include "rz.h"
#include "tz.h"

/* declarations for glue to 4.4bsd pmax port SCSI drivers and autoconfig */
#if NASC > 0
extern struct pmax_driver ascdriver;	/* XXX */
#endif
#if NSII > 0
extern struct pmax_driver siidriver;	/* XXX */
#endif
#if NRZ > 0
extern struct pmax_driver rzdriver;	/* XXX */
#endif
#if NTZ > 0
extern struct pmax_driver tzdriver;	/* XXX */
#endif


/*
 * A fixed-at-compile-time disk configuration for the pmax-specific
 * scsi code.
 *
 * If you want to change this, fix the pmax kernel to use new-style
 * config code and the machine-independent NetBSD scsi drivers.
 */

struct pmax_scsi_device scsi_dinit[] = {
/*driver,	cdriver,	unit,	ctlr,	drive,	slave,	dk,	flags*/

#if NSII > 0
# if NRZ > 0
{ &rzdriver,	&siidriver,	0,	0,	0,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	1,	0,	1,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	2,	0,	2,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	3,	0,	3,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	4,	0,	4,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	5,	0,	5,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	6,	0,	6,	0,	1,	0x0 },
{ &rzdriver,	&siidriver,	7,	0,	7,	0,	1,	0x0 },
# endif /* NTZ */

# if NTZ > 0
{ &tzdriver,	&siidriver,	0,	0,	5,	0,	0,	0x0 },
{ &tzdriver,	&siidriver,	1,	0,	6,	0,	0,	0x0 },
# endif /* NTZ */
#endif /* NSII */


#if NASC > 0
# if NRZ > 0
{ &rzdriver,	&ascdriver,	0,	0,	0,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	1,	0,	1,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	2,	0,	2,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	3,	0,	3,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	4,	0,	4,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	5,	0,	5,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	6,	0,	6,	0,	1,	0x0 },

# if NRZ > 7
{ &rzdriver,	&ascdriver,	8,	1,	0,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	9,	1,	1,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	10,	1,	2,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	11,	1,	3,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	12,	1,	4,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	13,	1,	5,	0,	1,	0x0 },
{ &rzdriver,	&ascdriver,	14,	1,	6,	0,	1,	0x0 },
# endif /* NRZ > 7 */
# endif /* NRZ */

# if NTZ > 0
{ &tzdriver,	&ascdriver,	0,	0,	5,	0,	0,	0x0 },
{ &tzdriver,	&ascdriver,	1,	0,	6,	0,	0,	0x0 },

# if NTZ > 2
{ &tzdriver,	&ascdriver,	2,	1,	5,	0,	0,	0x0 },
{ &tzdriver,	&ascdriver,	3,	1,	6,	0,	0,	0x0 },
# endif /* NTZ > 2 */
# endif /* NTZ */
#endif /* NASC */

 { 0 }
};


static int	nomatch __P((struct device *parent, struct cfdata *cf,
		    void *aux));
static void	noattach __P((struct device *parent, struct device *self,
		    void *aux));


/* placeholder definitions for new-style scsi bus/disk/tape drivers */

struct cfattach oldscsibus_ca = { 0, nomatch, noattach };

struct cfattach rz_ca = { 0, nomatch, noattach };

struct cfattach tz_ca = { 0, nomatch, noattach };


#define MAX_SCSI 4
static int nscsi;
static struct pmax_ctlr pmax_scsi_table[MAX_SCSI+1] = {
/*	driver,		unit,	addr,		flags */

	{ NULL, },  { NULL, }, { NULL, }, { NULL, },
	{ NULL, } /* sentinel */
};

/*
 * Callback for scsi controllers to register themselves with this
 * config glue.  Construct an old-style pmax autoconfiguration
 * SCSI-driver table entry for a  DECstation SCSI controller, and add it
 * to the table of known SCSI drivers.  Needed for old-style pmax
 * SCSI-bus probing.  configure() will call us back to probe
 * each known controller for the statically-configured drives, above.
 */
void
pmax_add_scsi(dp, unit)
	struct pmax_driver *dp;
	int unit;
{
	struct pmax_ctlr *cp  = &pmax_scsi_table[nscsi++];
	if (nscsi > MAX_SCSI) {
		panic("Too many old-style SCSI adaptors\n");
	}
	cp->pmax_driver = dp;
	cp->pmax_unit = unit;
}

/*
 * Configure scsi devices on old-style pmax scsi drivers.
 * Interrupts must be enabled or this will hang.
 *
 * Called by configure() after all possible controllers have been
 * found.  The controllers really should invoke new-style
 * autoconfiguration on themselves, probing their SCSI buses,
 * but the pmax drivers don't yet have polled SCSI.
 */
void
configure_scsi()
{
	struct pmax_ctlr *cp;
	struct pmax_scsi_device *dp;
	struct pmax_driver *drp;

	/* probe and initialize SCSI buses */
	for (cp = &pmax_scsi_table[0]; (drp = cp->pmax_driver) != NULL; cp++) {

		/* probe and initialize devices connected to controller */
		for (dp = scsi_dinit; (drp = dp->sd_driver) != NULL; dp++) {
			/* might want to get fancier later */
			if (dp->sd_cdriver != cp->pmax_driver ||
			    dp->sd_ctlr != cp->pmax_unit)
			    continue;	/* not connected */
			if (!(*drp->d_init)(dp))
			    continue;
			dp->sd_alive = 1;

		}
	}
}


/*
 * Match function in struct cfattach of old-conf drivers: never matches.
 */
static int
nomatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
#if /*def DEBUG*/ 0
	printf("nomatch  %s: %s not yet done\n",
		parent->dv_cfdata->cf_driver->cd_name,
		parent->dv_xname);
#endif
	return 0;
}


/*
 * Attach function in struct cfattach of old-conf drivers: never called.
 */
static void
noattach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
#ifdef DEBUG
	printf("new attach  %s from %s: not yet done\n",
		self->dv_xname, parent->dv_xname);
#else
	panic("Can't do new-config attach of old device %s\n",
		self->dv_xname);
#endif
	return;
}

