/* s_logbf.c -- float version of s_logb.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$Id: s_logbf.c,v 1.3 1994/09/22 16:44:55 jtc Exp $";
#endif

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	float logbf(float x)
#else
	float logbf(x)
	float x;
#endif
{
	int32_t ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;			/* high |x| */
	if(ix==0) return (float)-1.0/fabsf(x);
	if(ix>=0x7f800000) return x*x;
	if((ix>>=23)==0) 			/* IEEE 754 logb */
		return -126.0; 
	else
		return (float) (ix-127); 
}
