# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gcc,v 1.22 2006/06/25 03:06:15 mrg Exp 
# Generated from: NetBSD: mknative.common,v 1.8 2006/05/26 19:17:21 mrg Exp 
#
G_INCLUDES=-I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/. -I${GNUHOSTDIST}/gcc/../include -I${GNUHOSTDIST}/gcc/../libcpp/include 
G_CRTSTUFF_CFLAGS=-O2  -DIN_GCC    -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition  -isystem ./include  -I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/. -I${GNUHOSTDIST}/gcc/../include -I${GNUHOSTDIST}/gcc/../libcpp/include   -g0 -finhibit-size-directive -fno-inline-functions -fno-exceptions -fno-zero-initialized-in-bss -fno-unit-at-a-time 
G_CRTSTUFF_T_CFLAGS=-fPIC
G_tm_defines=NETBSD_ENABLE_PTHREADS TARGET_ENDIAN_DEFAULT=MASK_BIG_END
G_xm_file=
G_xm_defines=
