/*	$NetBSD: amgen.h,v 1.1.1.1 2001/01/27 08:08:51 itojun Exp $	*/

/* Copyright (C) RSA Data Security, Inc. created 1994, 1996.  This is an
   unpublished work protected as such under copyright law.  This work
   contains proprietary, confidential, and trade secret information of
   RSA Data Security, Inc.  Use, disclosure or reproduction without the
   express written authorization of RSA Data Security, Inc. is
   prohibited.
 */

struct B_KeyInfoType;

typedef struct {
  int (*Query) PROTO_LIST
    ((unsigned int *, unsigned int *, unsigned int *, struct B_KeyInfoType **,
      POINTER));
  int (*Init) PROTO_LIST ((POINTER, POINTER, POINTER, A_SURRENDER_CTX *));
  int (*Generate) PROTO_LIST
    ((POINTER, POINTER *, unsigned char *, A_SURRENDER_CTX *));
} A_GENERATE_ALGA;

