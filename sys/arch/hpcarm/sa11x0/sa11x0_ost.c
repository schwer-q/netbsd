/*	$Id: sa11x0_ost.c,v 1.1 2001/02/23 03:48:21 ichiro Exp $ */

/*	$NetBSD: sa11x0_ost.c,v 1.1 2001/02/23 03:48:21 ichiro Exp $	*/
/*
 * Copyright (c) 1997 Mark Brinicombe.
 * Copyright (c) 1997 Causality Limited.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by IWAMOTO Toshihiro and Ichiro FUKUHARA.
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

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/time.h>
#include <sys/device.h>

#include <machine/bus.h>
#include <machine/irqhandler.h>
#include <machine/cpufunc.h>
#include <machine/katelib.h>
#include <hpcarm/sa11x0/sa11x0_reg.h> 
#include <hpcarm/sa11x0/sa11x0_var.h>
#include <hpcarm/sa11x0/sa11x0_ostreg.h>

static int	saost_match(struct device *, struct cfdata *, void *);
static void	saost_attach(struct device *, struct device *, void *);

int		gettick(void);
int		clockintr(void *);
void		rtcinit(void);

struct saost_softc {
	struct device		sc_dev;
	bus_addr_t		sc_baseaddr;
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;

	u_int32_t	sc_clock_count;
	void		*sc_clockintr;
	u_int32_t	sc_statclock_count;
	void		*sc_statclockintr;
};

static struct saost_softc *saost_sc = NULL;

#define TIMER_FREQUENCY         3686400         /* 3.6468MHz */
#define TICKS_PER_MICROSECOND   (TIMER_FREQUENCY/1000000)


struct cfattach saost_ca = {
	sizeof(struct saost_softc), saost_match, saost_attach
};

static int
saost_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	return(0);
}

void
saost_attach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct saost_softc *sc = (struct saost_softc*)self;
	struct sa11x0_attach_args *sa = aux;

	sc->sc_iot = sa->sa_iot;
	sc->sc_baseaddr = sa->sa_addr;

	saost_sc = sc;

	if(bus_space_map(sa->sa_iot, sa->sa_addr, sa->sa_size, 0, 
			&sc->sc_ioh))
		panic("%s: Cannot map registers\n", self->dv_xname);

	printf("%s: SA-11x0 OS Timer\n",  sc->sc_dev.dv_xname);
}

int
clockintr(arg)
	void *arg;
{
	struct clockframe *frame = arg;
	u_int32_t oscr, nextmatch, oldmatch;

	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh,
			SAOST_SR, 1);

	hardclock(frame);

	/* schedule next clock intr */
	oldmatch = saost_sc->sc_clock_count;
	nextmatch = oldmatch + TIMER_FREQUENCY / hz;
	oscr = bus_space_read_4(saost_sc->sc_iot, saost_sc->sc_ioh,
				SAOST_CR);
	/* XXX it will take some time to return from intr */
	oscr += 100;
	if ((nextmatch > oldmatch &&
	     (oscr > nextmatch || oscr < oldmatch)) ||
	    (nextmatch < oldmatch && oscr > nextmatch && oscr < oldmatch)) {
		/* we were late to handle this intr */
		/* XXX adjust clock to compensate lost hardclock() calls */

		/* XXX is this ok? */
		nextmatch = oscr + TIMER_FREQUENCY / hz;
	}
	saost_sc->sc_clock_count = nextmatch;
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh, SAOST_MR0,
			  nextmatch);
	return(-1);
}


void
setstatclockrate(hz)
	int hz;
{
	int timer_count, enable;
	timer_count = TIMER_FREQUENCY / hz;
	enable = bus_space_read_4(saost_sc->sc_iot, saost_sc->sc_ioh,
				SAOST_IR);
	enable |= 2;
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh,
			SAOST_IR, enable);
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh,
			SAOST_MR1, timer_count);
	saost_sc->sc_statclock_count = timer_count;
}

void
cpu_initclocks()
{
	/* initialize saost and setup irq */

	printf("clock: hz=%d stathz = %d profhz = %d\n", hz, stathz, profhz);

	/* Disable reset on timer 3 match */
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh, SAOST_CR, 0);
	/* Setup timer 1 and claim interrupt */

	saost_sc->sc_clock_count = TIMER_FREQUENCY / hz;
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh, SAOST_IR, 1);
	bus_space_write_4(saost_sc->sc_iot, saost_sc->sc_ioh, SAOST_MR0,
			  saost_sc->sc_clock_count);

	sa11x0_intr_establish(0, 26, 1, IPL_CLOCK, clockintr, 0);
}

int
gettick()
{
	int counter;
	u_int savedints;
	savedints = disable_interrupts(I32_bit);

	counter = bus_space_read_4(saost_sc->sc_iot, saost_sc->sc_ioh,
			SAOST_CR);

	restore_interrupts(savedints);
	return counter;
}

void
microtime(tvp)
	register struct timeval *tvp;
{
	int s = splhigh();
	int tm;
	int deltatm;
	static struct timeval lasttime;

	tm = bus_space_read_4(saost_sc->sc_iot, saost_sc->sc_ioh,
			SAOST_CR);

	deltatm = saost_sc->sc_clock_count - tm;

#ifdef DEBUG
	printf("deltatm = %d\n",deltatm);
#endif

	*tvp = time;
	tvp->tv_usec++;		/* XXX */
	while (tvp->tv_usec >= 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}

	if (tvp->tv_sec == lasttime.tv_sec &&
		tvp->tv_usec <= lasttime.tv_usec &&
		(tvp->tv_usec = lasttime.tv_usec + 1) >= 1000000)
	{
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
	lasttime = *tvp;
	splx(s);
}

void
delay(usecs)
	u_int usecs;
{
	int limit, tick, otick;

	usecs *= TICKS_PER_MICROSECOND;

	if (! saost_sc) {
		/* clock isn't initialized yet */
		int j;
		for(; usecs > 0; usecs--)
			for(j = 100; j > 0; j--)
				;
		return;
	}

	otick = gettick();
	limit = 0xffffffff;

	while (usecs > 0) {
		tick = gettick();
		if (tick < otick)
			usecs -= limit - (otick - tick);
		else
			usecs -= tick - otick;
		otick = tick;
	}
}

void
resettodr()
{
}

void
inittodr(base)
	time_t base;
{
	time.tv_sec = base;
	time.tv_usec = 0;
}
