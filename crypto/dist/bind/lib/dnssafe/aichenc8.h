/*	$NetBSD: aichenc8.h,v 1.1.1.1 2001/01/27 08:08:49 itojun Exp $	*/

/* Copyright (C) RSA Data Security, Inc. created 1993, 1996.  This is an
   unpublished work protected as such under copyright law.  This work
   contains proprietary, confidential, and trade secret information of
   RSA Data Security, Inc.  Use, disclosure or reproduction without the
   express written authorization of RSA Data Security, Inc. is
   prohibited.
 */

#ifndef _AICHENC8_H_
#define _AICHENC8_H_ 1

#include "aichencr.h"

extern B_AlgorithmInfoTypeVTable AITChooseEncrypt8_V_TABLE;

int AIT_8AddInfo PROTO_LIST
  ((THIS_ALGORITHM_INFO_TYPE *, B_Algorithm *, POINTER));

#endif
