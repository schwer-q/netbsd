/*	$NetBSD: platform.c,v 1.1 2001/06/20 14:35:27 nonaka Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by NONAKA Kimihiro.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#include <sys/param.h>

#include <machine/intr.h>
#include <machine/platform.h>
#include <machine/residual.h>

static struct platform platform_unknown = {
	NULL,			/* model */
	platform_generic_match,	/* match */
	pci_intr_nofixup,	/* pci_intr_fixup */
	ext_intr,		/* ext_intr */
	cpu_setup_unknown,	/* cpu_setup */
	reset_unknown,		/* reset */
};

static struct plattab plattab_unknown = {
	NULL,	0
};

struct platform *platform = &platform_unknown;

int
ident_platform(void)
{
	struct plattab *p;
	int matched = -1, match = 0;
	int i, rv;

	if (res == NULL)
		return 0;

	if (strncmp(res->VitalProductData.PrintableModel, "IBM", 3) == 0)
		p = &plattab_ibm;
	else
		p = &plattab_unknown;

	for (i = 0; i < p->num; i++) {
		rv = (*p->platform[i]->match)(p->platform[i]);
		if (rv > match) {
			match = rv;
			matched = i;
		}
	}
	if (match)
		platform = p->platform[matched];
	return match;
}

int
platform_generic_match(struct platform *p)
{

	if (p->model == NULL)
		return 0;
	if (strcmp(res->VitalProductData.PrintableModel, p->model) == 0)
    		return 1;
	return 0;
}


/* ARGUSED */
void
pci_intr_nofixup(int busno, int device, int *intr)
{
}

/* ARGUSED */
void
cpu_setup_unknown(struct device *dev)
{
}

void
reset_unknown(void)
{
}
