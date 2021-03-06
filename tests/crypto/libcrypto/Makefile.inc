# $NetBSD: Makefile.inc,v 1.10 2018/09/28 23:40:45 christos Exp $

.include <bsd.own.mk>
.include "../Makefile.inc"
TESTSDIR=	${TESTSBASE}/crypto/libcrypto
ATFFILE=	no

.if defined(HELPER_NAME)
HELPER_SRCS?=	${HELPER_NAME}.c

PROG=		h_${HELPER_NAME}
SRCS=		${HELPER_SRCS}
MAN=		# empty
BINDIR=		${TESTSDIR}

CWARNFLAGS.clang+=	-Wno-format

CPPFLAGS+=	-DOPENSSL_FIPS
CPPFLAGS+=	-I${OPENSSLSRC} -I${OPENSSLSRC}/crypto
CPPFLAGS+=	-I${OPENSSLSRC}/include -I${OPENSSLSRC}/../include
CPPFLAGS+=	-I${OPENSSLSRC}/crypto/include
CRYPTODIST=	${NETBSDSRCDIR}/crypto
.include "${NETBSDSRCDIR}/crypto/Makefile.openssl"
.if ${HAVE_OPENSSL} != 10
.PATH:	${OPENSSLSRC}/test ${OPENSSLSRC}/test/testutil
.else
.PATH: ${OPENSSLSRC}/crypto/${HELPER_DIR}
.endif
PROGDPLIBS+=	cryptotest ${OPENSSLSRC}/../lib/libcryptotest
PROGDPLIBS+=	crypto ${OPENSSLSRC}/../lib/libcrypto
DPADD+=		${LIBCRYPT}
LDADD+=		-lcrypt


.include <bsd.test.mk>
.include <bsd.prog.mk>
.endif
