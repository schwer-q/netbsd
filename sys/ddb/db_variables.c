/*	$NetBSD: db_variables.c,v 1.30 2004/03/24 15:34:52 atatat Exp $	*/

/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: db_variables.c,v 1.30 2004/03/24 15:34:52 atatat Exp $");

#include "opt_ddbparam.h"

#include <sys/param.h>
#include <sys/proc.h>
#include <uvm/uvm_extern.h>
#include <sys/sysctl.h>

#include <machine/db_machdep.h>

#include <ddb/ddbvar.h>

#include <ddb/db_lex.h>
#include <ddb/db_variables.h>
#include <ddb/db_command.h>
#include <ddb/db_sym.h>
#include <ddb/db_extern.h>
#include <ddb/db_output.h>


/*
 * If this is non-zero, the DDB will be entered when the system
 * panics.  Initialize it so that it's patchable.
 */
#ifndef DDB_ONPANIC
#define DDB_ONPANIC	1
#endif
int		db_onpanic = DDB_ONPANIC;

/*
 * Can  DDB can be entered from the console?
 */
#ifndef	DDB_FROMCONSOLE
#define	DDB_FROMCONSOLE 1
#endif
int		db_fromconsole = DDB_FROMCONSOLE;

static int	db_rw_internal_variable(const struct db_variable *, db_expr_t *,
		    int);
static int	db_find_variable(const struct db_variable **);

/* XXX must all be ints for sysctl. */
const struct db_variable db_vars[] = {
	{ "radix",	(void *)&db_radix,	db_rw_internal_variable },
	{ "maxoff",	(void *)&db_maxoff,	db_rw_internal_variable },
	{ "maxwidth",	(void *)&db_max_width,	db_rw_internal_variable },
	{ "tabstops",	(void *)&db_tab_stop_width, db_rw_internal_variable },
	{ "lines",	(void *)&db_max_line,	db_rw_internal_variable },
	{ "onpanic",	(void *)&db_onpanic,	db_rw_internal_variable },
	{ "fromconsole", (void *)&db_fromconsole, db_rw_internal_variable },
};
const struct db_variable * const db_evars = db_vars + sizeof(db_vars)/sizeof(db_vars[0]);

/*
 * ddb command line access to the DDB variables defined above.
 */
static int
db_rw_internal_variable(const struct db_variable *vp, db_expr_t *valp, int rw)
{

	if (rw == DB_VAR_GET)
		*valp = *(int *)vp->valuep;
	else
		*(int *)vp->valuep = *valp;
	return (0);
}

/*
 * sysctl(3) access to the DDB variables defined above.
 */
SYSCTL_SETUP(sysctl_ddb_setup, "sysctl ddb subtree setup")
{

	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "ddb", NULL,
		       NULL, 0, NULL, 0,
		       CTL_DDB, CTL_EOL);

	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "radix", NULL,
		       NULL, 0, &db_radix, 0,
		       CTL_DDB, DDBCTL_RADIX, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "maxoff", NULL,
		       NULL, 0, &db_maxoff, 0,
		       CTL_DDB, DDBCTL_MAXOFF, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "maxwidth", NULL,
		       NULL, 0, &db_max_width, 0,
		       CTL_DDB, DDBCTL_MAXWIDTH, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "lines", NULL,
		       NULL, 0, &db_max_line, 0,
		       CTL_DDB, DDBCTL_LINES, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "tabstops", NULL,
		       NULL, 0, &db_tab_stop_width, 0,
		       CTL_DDB, DDBCTL_TABSTOPS, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "onpanic", NULL,
		       NULL, 0, &db_onpanic, 0,
		       CTL_DDB, DDBCTL_ONPANIC, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "fromconsole", NULL,
		       NULL, 0, &db_fromconsole, 0,
		       CTL_DDB, DDBCTL_FROMCONSOLE, CTL_EOL);
}

int
db_find_variable(const struct db_variable **varp)
{
	int	t;
	const struct db_variable *vp;

	t = db_read_token();
	if (t == tIDENT) {
		for (vp = db_vars; vp < db_evars; vp++) {
			if (!strcmp(db_tok_string, vp->name)) {
				*varp = vp;
				return (1);
			}
		}
		for (vp = db_regs; vp < db_eregs; vp++) {
			if (!strcmp(db_tok_string, vp->name)) {
				*varp = vp;
				return (1);
			}
		}
	}
	db_error("Unknown variable\n");
	/*NOTREACHED*/
	return 0;
}

int
db_get_variable(db_expr_t *valuep)
{
	const struct db_variable *vp;

	if (!db_find_variable(&vp))
		return (0);

	db_read_variable(vp, valuep);

	return (1);
}

int
db_set_variable(db_expr_t value)
{
	const struct db_variable *vp;

	if (!db_find_variable(&vp))
		return (0);

	db_write_variable(vp, &value);

	return (1);
}


void
db_read_variable(const struct db_variable *vp, db_expr_t *valuep)
{
	int (*func)(const struct db_variable *, db_expr_t *, int) = vp->fcn;

	if (func == FCN_NULL)
		*valuep = *(vp->valuep);
	else
		(*func)(vp, valuep, DB_VAR_GET);
}

void
db_write_variable(const struct db_variable *vp, db_expr_t *valuep)
{
	int (*func)(const struct db_variable *, db_expr_t *, int) = vp->fcn;

	if (func == FCN_NULL)
		*(vp->valuep) = *valuep;
	else
		(*func)(vp, valuep, DB_VAR_SET);
}

/*ARGSUSED*/
void
db_set_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	db_expr_t	value;
	db_expr_t	old_value;
	const struct db_variable *vp;
	int	t;

	t = db_read_token();
	if (t != tDOLLAR) {
		db_error("Unknown variable\n");
		/*NOTREACHED*/
	}
	if (!db_find_variable(&vp)) {
		db_error("Unknown variable\n");
		/*NOTREACHED*/
	}

	t = db_read_token();
	if (t != tEQ)
		db_unread_token(t);

	if (!db_expression(&value)) {
		db_error("No value\n");
		/*NOTREACHED*/
	}
	if (db_read_token() != tEOL) {
		db_error("?\n");
		/*NOTREACHED*/
	}

	db_read_variable(vp, &old_value);
	db_printf("$%s\t\t%s = ", vp->name, db_num_to_str(old_value));
	db_printf("%s\n", db_num_to_str(value));
	db_write_variable(vp, &value);
}
