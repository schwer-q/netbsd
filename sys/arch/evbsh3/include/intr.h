/*	$NetBSD: intr.h,v 1.2 2000/02/24 19:01:26 msaitoh Exp $	*/
#ifndef _EVBSH3_INTR_H_
#define _EVBSH3_INTR_H_

#include <sh3/intr.h>

/* Soft interrupt masks. */
#define	SIR_CLOCK	31
#define	SIR_NET		30
#define	SIR_SERIAL	29

#define SIR_LOW		29
#define SIR_HIGH	31

/* IRQ */
#define TMU1_IRQ	2
#define SCI_IRQ		6
#define SCIF_IRQ	7

#define IRQ_LOW  2
#define IRQ_HIGH 15

#endif /* _EVBSH3_INTR_H_ */
