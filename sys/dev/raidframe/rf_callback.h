/*	$NetBSD: rf_callback.h,v 1.1 1998/11/13 04:20:26 oster Exp $	*/
/*
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Mark Holland
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

/*****************************************************************************************
 *
 * callback.h -- header file for callback.c
 *
 * the reconstruction code must manage concurrent I/Os on multiple drives.
 * it sometimes needs to suspend operation on a particular drive until some
 * condition occurs.  we can't block the thread, of course, or we wouldn't
 * be able to manage our other outstanding I/Os.  Instead we just suspend
 * new activity on the indicated disk, and create a callback descriptor and
 * put it someplace where it will get invoked when the condition that's
 * stalling us has cleared.  When the descriptor is invoked, it will call
 * a function that will restart operation on the indicated disk.
 *
 ****************************************************************************************/

/* :  
 * Log: rf_callback.h,v 
 * Revision 1.8  1996/08/01 15:57:28  jimz
 * minor cleanup
 *
 * Revision 1.7  1996/07/27  23:36:08  jimz
 * Solaris port of simulator
 *
 * Revision 1.6  1996/06/10  11:55:47  jimz
 * Straightened out some per-array/not-per-array distinctions, fixed
 * a couple bugs related to confusion. Added shutdown lists. Removed
 * layout shutdown function (now subsumed by shutdown lists).
 *
 * Revision 1.5  1996/05/23  21:46:35  jimz
 * checkpoint in code cleanup (release prep)
 * lots of types, function names have been fixed
 *
 * Revision 1.4  1996/05/18  19:51:34  jimz
 * major code cleanup- fix syntax, make some types consistent,
 * add prototypes, clean out dead code, et cetera
 *
 * Revision 1.3  1996/05/17  16:30:46  jimz
 * add prototypes
 *
 * Revision 1.2  1995/12/01  15:15:55  root
 * added copyright info
 *
 */

#ifndef _RF__RF_CALLBACK_H_
#define _RF__RF_CALLBACK_H_

#include "rf_types.h"

struct RF_CallbackDesc_s {
  void              (*callbackFunc)(RF_CBParam_t); /* function to call */
  RF_CBParam_t        callbackArg;     /* args to give to function, or just info about this callback  */
  RF_CBParam_t        callbackArg2;
  RF_RowCol_t         row;             /* disk row and column IDs to give to the callback func */
  RF_RowCol_t         col;
  RF_CallbackDesc_t  *next;            /* next entry in list */
};

int                 rf_ConfigureCallback(RF_ShutdownList_t **listp);
RF_CallbackDesc_t  *rf_AllocCallbackDesc(void);
void                rf_FreeCallbackDesc(RF_CallbackDesc_t *p);

#endif /* !_RF__RF_CALLBACK_H_ */
