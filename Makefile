#	$NetBSD: Makefile,v 1.264 2008/12/05 19:02:54 cube Exp $

#
# This is the top-level makefile for building NetBSD. For an outline of
# how to build a snapshot or release, as well as other release engineering
# information, see http://www.NetBSD.org/developers/releng/index.html
#
# Not everything you can set or do is documented in this makefile. In
# particular, you should review the files in /usr/share/mk (especially
# bsd.README) for general information on building programs and writing
# Makefiles within this structure, and see the comments in src/etc/Makefile
# for further information on installation and release set options.
#
# Variables listed below can be set on the make command line (highest
# priority), in /etc/mk.conf (middle priority), or in the environment
# (lowest priority).
#
# Variables:
#   DESTDIR is the target directory for installation of the compiled
#	software. It defaults to /. Note that programs are built against
#	libraries installed in DESTDIR.
#   MKMAN, if `no', will prevent building of manual pages.
#   MKOBJDIRS, if not `no', will build object directories at
#	an appropriate point in a build.
#   MKSHARE, if `no', will prevent building and installing
#	anything in /usr/share.
#   MKUPDATE, if not `no', will avoid a `make cleandir' at the start of
#	`make build', as well as having the effects listed in
#	/usr/share/mk/bsd.README.
#   NOCLEANDIR, if defined, will avoid a `make cleandir' at the start
#	of the `make build'.
#   NOINCLUDES will avoid the `make includes' usually done by `make build'.
#
#   See mk.conf(5) for more details.
#
#
# Targets:
#   build:
#	Builds a full release of NetBSD in DESTDIR, except for the
#	/etc configuration files.
#	If BUILD_DONE is set, this is an empty target.
#   distribution:
#	Builds a full release of NetBSD in DESTDIR, including the /etc
#	configuration files.
#   buildworld:
#	As per `make distribution', except that it ensures that DESTDIR
#	is not the root directory.
#   installworld:
#	Install the distribution from DESTDIR to INSTALLWORLDDIR (which
#	defaults to the root directory).  Ensures that INSTALLWORLDDIR
#	is the not root directory if cross compiling.
#   release:
#	Does a `make distribution', and then tars up the DESTDIR files
#	into ${RELEASEDIR}/${RELEASEMACHINEDIR}, in release(7) format.
#	(See etc/Makefile for more information on this.)
#   regression-tests:
#	Runs the regression tests in "regress" on this host.
#   sets:
#	Populate ${RELEASEDIR}/${RELEASEMACHINEDIR}/binary/sets
#	from ${DESTDIR}
#   sourcesets:
#	Populate ${RELEASEDIR}/source/sets from ${NETBSDSRCDIR}
#   syspkgs:
#	Populate ${RELEASEDIR}/${RELEASEMACHINEDIR}/binary/syspkgs
#	from ${DESTDIR}
#   iso-image:
#	Create CD-ROM image in RELEASEDIR/iso.
#	RELEASEDIR must already have been populated by `make release'
#	or equivalent.
#   iso-image-source:
#	Create CD-ROM image with source in RELEASEDIR/iso.
#	RELEASEDIR must already have been populated by
#	`make release sourcesets' or equivalent.
#
# Targets invoked by `make build,' in order:
#   cleandir:        cleans the tree.
#   do-top-obj:      creates the top level object directory.
#   do-tools-obj:    creates object directories for the host toolchain.
#   do-tools:        builds host toolchain.
#   obj:             creates object directories.
#   do-distrib-dirs: creates the distribution directories.
#   includes:        installs include files.
#   do-tools-compat: builds the "libnbcompat" library; needed for some
#                    random host tool programs in the source tree.
#   do-lib-csu:      builds and installs prerequisites from lib/csu.
#   do-libgcc:       builds and installs prerequisites from
#                    gnu/lib/crtstuff${LIBGCC_EXT} (if necessary) and
#                    gnu/lib/libgcc${LIBGCC_EXT}.
#   do-libpcc:       builds and install prerequisites from
#                    external/bsd/pcc/crtstuff and external/bsd/pcc/libpcc.
#   do-lib-libc:     builds and installs prerequisites from lib/libc.
#   do-lib:          builds and installs prerequisites from lib.
#   do-gnu-lib:      builds and installs prerequisites from gnu/lib.
#   do-external-lib: builds and installs prerequisites from external/lib.
#   do-sys-rump-fs-lib:  builds and installs prerequisites from sys/rump/fs/lib
#   do-sys-rump-net-lib: builds and installs prerequisites from sys/rump/net/lib
#   do-ld.so:        builds and installs prerequisites from libexec/ld.*_so.
#   do-compat-lib-csu: builds and installs prerequisites from compat/lib/csu
#                    if ${MKCOMPAT} != "no".
#   do-compat-libgcc: builds and installs prerequisites from
#                    compat/gnu/lib/crtstuff${LIBGCC_EXT} (if necessary) and
#                    compat/gnu/lib/libgcc${LIBGCC_EXT} if ${MKCOMPAT} != "no".
#   do-compat-lib-libc: builds and installs prerequisites from compat/lib/libc
#                    if ${MKCOMPAT} != "no".
#   do-build:        builds and installs the entire system.
#   do-x11:          builds and installs X11 if ${MKX11} != "no"; either
#                    X11R7 from src/external/mit/xorg if ${X11FLAVOUR} == "Xorg"
#                    or X11R6 from src/x11
#   do-obsolete:     installs the obsolete sets (for the postinstall-* targets).
#

.if ${.MAKEFLAGS:M${.CURDIR}/share/mk} == ""
.MAKEFLAGS: -m ${.CURDIR}/share/mk
.endif

#
# If _SRC_TOP_OBJ_ gets set here, we will end up with a directory that may
# not be the top level objdir, because "make obj" can happen in the *middle*
# of "make build" (long after <bsd.own.mk> is calculated it).  So, pre-set
# _SRC_TOP_OBJ_ here so it will not be added to ${.MAKEOVERRIDES}.
#
_SRC_TOP_OBJ_=

.include <bsd.own.mk>

#
# Sanity check: make sure that "make build" is not invoked simultaneously
# with a standard recursive target.
#

.if make(build) || make(release) || make(snapshot)
.for targ in ${TARGETS:Nobj:Ncleandir}
.if make(${targ}) && !target(.BEGIN)
.BEGIN:
	@echo 'BUILD ABORTED: "make build" and "make ${targ}" are mutually exclusive.'
	@false
.endif
.endfor
.endif

#
# _SUBDIR is used to set SUBDIR, after removing directories that have
# BUILD_${dir}=no, or that have no ${dir}/Makefile.
#
_SUBDIR=	tools lib include gnu external bin games libexec sbin usr.bin
_SUBDIR+=	usr.sbin share rescue sys etc tests compat .WAIT distrib regress

.for dir in ${_SUBDIR}
.if "${dir}" == ".WAIT" \
	|| (${BUILD_${dir}:Uyes} != "no" && exists(${dir}/Makefile))
SUBDIR+=	${dir}
.endif
.endfor

.if exists(regress)
regression-tests: .PHONY .MAKE
	@echo Running regression tests...
	${MAKEDIRTARGET} regress regress
.endif

.if ${MKUNPRIVED} != "no"
NOPOSTINSTALL=	# defined
.endif

afterinstall: .PHONY .MAKE
.if ${MKMAN} != "no"
	${MAKEDIRTARGET} share/man makedb
.endif
.if (${MKUNPRIVED} != "no" && ${MKINFO} != "no")
	${MAKEDIRTARGET} gnu/usr.bin/texinfo/install-info infodir-meta
.endif
.if !defined(NOPOSTINSTALL)
	${MAKEDIRTARGET} . postinstall-check
.endif

_POSTINSTALL=	${.CURDIR}/usr.sbin/postinstall/postinstall

postinstall-check: .PHONY
	@echo "   === Post installation checks ==="
	MAKE=${MAKE:Q} ${HOST_SH} ${_POSTINSTALL} -s ${.CURDIR} -d ${DESTDIR}/ check; if [ $$? -gt 1 ]; then exit 1; fi
	@echo "   ================================"

postinstall-fix: .NOTMAIN .PHONY
	@echo "   === Post installation fixes ==="
	MAKE=${MAKE:Q} ${HOST_SH} ${_POSTINSTALL} -s ${.CURDIR} -d ${DESTDIR}/ fix
	@echo "   ==============================="

postinstall-fix-obsolete: .NOTMAIN .PHONY
	@echo "   === Removing obsolete files ==="
	MAKE=${MAKE:Q} ${HOST_SH} ${_POSTINSTALL} -s ${.CURDIR} -d ${DESTDIR}/ fix obsolete
	@echo "   ==============================="


#
# Targets (in order!) called by "make build".
#
.if defined(HAVE_GCC)
.if ${HAVE_GCC} == "3"
LIBGCC_EXT=3
.else
LIBGCC_EXT=4
.endif
.endif

BUILDTARGETS+=	check-tools
.if ${MKUPDATE} == "no" && !defined(NOCLEANDIR)
BUILDTARGETS+=	cleandir
.endif
.if ${MKOBJDIRS} != "no"
BUILDTARGETS+=	do-top-obj
.endif
.if ${USETOOLS} == "yes"	# {
.if ${MKOBJDIRS} != "no"
BUILDTARGETS+=	do-tools-obj
.endif
BUILDTARGETS+=	do-tools
.endif # USETOOLS		# }
.if ${MKOBJDIRS} != "no"
BUILDTARGETS+=	obj
.endif
.if !defined(NODISTRIBDIRS)
BUILDTARGETS+=	do-distrib-dirs
.endif
.if !defined(NOINCLUDES)
BUILDTARGETS+=	includes
.endif
BUILDTARGETS+=	do-tools-compat
BUILDTARGETS+=	do-lib-csu
.if ${MKGCC} != "no"
BUILDTARGETS+=	do-libgcc
.endif
.if ${MKPCC} != "no"
BUILDTARGET+=	do-libpcc
.endif
BUILDTARGETS+=	do-lib-libc
BUILDTARGETS+=	do-lib do-gnu-lib do-external-lib
BUILDTARGETS+=	do-sys-rump-fs-lib do-sys-rump-net-lib
.if ${MKCOMPAT} != "no"
BUILDTARGETS+=	do-compat-lib-csu
BUILDTARGETS+=	do-compat-libgcc
BUILDTARGETS+=	do-compat-lib-libc
.endif
BUILDTARGETS+=	do-ld.so
BUILDTARGETS+=	do-build
.if ${MKX11} != "no"
BUILDTARGETS+=	do-x11
.endif
BUILDTARGETS+=	do-obsolete

#
# Enforce proper ordering of some rules.
#

.ORDER:		${BUILDTARGETS}
includes-lib:	.PHONY includes-include includes-sys
includes-gnu:	.PHONY includes-lib

#
# Build the system and install into DESTDIR.
#

START_TIME!=	date

build: .PHONY .MAKE
.if defined(BUILD_DONE)
	@echo "Build already installed into ${DESTDIR}"
.else
	@echo "Build started at: ${START_TIME}"
.for tgt in ${BUILDTARGETS}
	${MAKEDIRTARGET} . ${tgt}
.endfor
	${MAKEDIRTARGET} etc install-etc-release
	@echo   "Build started at:  ${START_TIME}"
	@printf "Build finished at: " && date
.endif

#
# Build a full distribution, but not a release (i.e. no sets into
# ${RELEASEDIR}).  "buildworld" enforces a build to ${DESTDIR} != /
#

distribution buildworld: .PHONY .MAKE
.if make(buildworld) && \
    (!defined(DESTDIR) || ${DESTDIR} == "" || ${DESTDIR} == "/")
	@echo "Won't make ${.TARGET} with DESTDIR=/"
	@false
.endif
	${MAKEDIRTARGET} . build NOPOSTINSTALL=1
	${MAKEDIRTARGET} etc distribution INSTALL_DONE=1
.if defined(DESTDIR) && ${DESTDIR} != "" && ${DESTDIR} != "/"
	${MAKEDIRTARGET} . postinstall-fix-obsolete
	${MAKEDIRTARGET} distrib/sets checkflist
.endif
	@echo   "make ${.TARGET} started at:  ${START_TIME}"
	@printf "make ${.TARGET} finished at: " && date

#
# Install the distribution from $DESTDIR to $INSTALLWORLDDIR (defaults to `/')
# If installing to /, ensures that the host's operating system is NetBSD and
# the host's `uname -m` == ${MACHINE}.
#

HOST_UNAME_S!=	uname -s
HOST_UNAME_M!=	uname -m

installworld: .PHONY .MAKE
.if (!defined(DESTDIR) || ${DESTDIR} == "" || ${DESTDIR} == "/")
	@echo "Can't make ${.TARGET} to DESTDIR=/"
	@false
.endif
.if !defined(INSTALLWORLDDIR) || \
    ${INSTALLWORLDDIR} == "" || ${INSTALLWORLDDIR} == "/"
.if (${HOST_UNAME_S} != "NetBSD")
	@echo "Won't cross-make ${.TARGET} from ${HOST_UNAME_S} to NetBSD with INSTALLWORLDDIR=/"
	@false
.endif
.if (${HOST_UNAME_M} != ${MACHINE})
	@echo "Won't cross-make ${.TARGET} from ${HOST_UNAME_M} to ${MACHINE} with INSTALLWORLDDIR=/"
	@false
.endif
.endif
	${MAKEDIRTARGET} distrib/sets installsets \
	    INSTALLDIR=${INSTALLWORLDDIR:U/} INSTALLSETS=
	${MAKEDIRTARGET} . postinstall-check DESTDIR=${INSTALLWORLDDIR}
	@echo   "make ${.TARGET} started at:  ${START_TIME}"
	@printf "make ${.TARGET} finished at: " && date

#
# Create sets from $DESTDIR or $NETBSDSRCDIR into $RELEASEDIR
#

.for tgt in sets sourcesets syspkgs
${tgt}: .PHONY .MAKE
	${MAKEDIRTARGET} distrib/sets ${tgt}
.endfor

#
# Build a release or snapshot (implies "make distribution").  Note that
# in this case, the set lists will be checked before the tar files
# are made.
#

release snapshot: .PHONY .MAKE
	${MAKEDIRTARGET} . distribution
	${MAKEDIRTARGET} etc release DISTRIBUTION_DONE=1
	@echo   "make ${.TARGET} started at:  ${START_TIME}"
	@printf "make ${.TARGET} finished at: " && date

#
# Create a CD-ROM image.
#

iso-image: .PHONY
	${MAKEDIRTARGET} distrib iso_image
	${MAKEDIRTARGET} etc iso-image
	@echo   "make ${.TARGET} started at:  ${START_TIME}"
	@printf "make ${.TARGET} finished at: " && date

iso-image-source: .PHONY
	${MAKEDIRTARGET} distrib iso_image CDSOURCE=true
	${MAKEDIRTARGET} etc iso-image
	@echo   "make ${.TARGET} started at:  ${START_TIME}"
	@printf "make ${.TARGET} finished at: " && date

#
# Special components of the "make build" process.
#

check-tools: .PHONY
.if ${TOOLCHAIN_MISSING} != "no" && !defined(EXTERNAL_TOOLCHAIN)
	@echo '*** WARNING:  Building on MACHINE=${MACHINE} with missing toolchain.'
	@echo '*** May result in a failed build or corrupt binaries!'
.elif defined(EXTERNAL_TOOLCHAIN)
	@echo '*** Using external toolchain rooted at ${EXTERNAL_TOOLCHAIN}.'
.endif
.if defined(NBUILDJOBS)
	@echo '*** WARNING: NBUILDJOBS is obsolete; use -j directly instead!'
.endif

do-distrib-dirs: .PHONY .MAKE
.if !defined(DESTDIR) || ${DESTDIR} == ""
	${MAKEDIRTARGET} etc distrib-dirs DESTDIR=/
.else
	${MAKEDIRTARGET} etc distrib-dirs DESTDIR=${DESTDIR}
.endif

.for targ in cleandir obj includes
do-${targ}: .PHONY ${targ}
	@true
.endfor

.if defined(HAVE_GCC)
BUILD_CC_LIB= gnu/lib/crtstuff${LIBGCC_EXT}
BUILD_CC_LIB+= gnu/lib/libgcc${LIBGCC_EXT}
.elif defined(HAVE_PCC)
BUILD_CC_LIB+= external/bsd/pcc/crtstuff
BUILD_CC_LIB+= external/bsd/pcc/libpcc
.endif

.if ${MKCOMPAT} != "no"
BUILD_COMPAT_LIBS=	compat/lib/csu ${BUILD_CC_LIB:S/^/compat\//} compat/lib/libc
.else
BUILD_COMPAT_LIBS=
.endif

.for dir in tools tools/compat lib/csu ${BUILD_CC_LIB} lib/libc lib/libdes lib gnu/lib external/lib sys/rump/fs/lib sys/rump/net/lib ${BUILD_COMPAT_LIBS}
do-${dir:S/\//-/g}: .PHONY .MAKE
.for targ in dependall install
	${MAKEDIRTARGET} ${dir} ${targ}
.endfor
.endfor

do-top-obj: .PHONY .MAKE
	${MAKEDIRTARGET} . obj NOSUBDIR=

do-tools-obj: .PHONY .MAKE
	${MAKEDIRTARGET} tools obj

do-libgcc: .PHONY .MAKE
.if defined(HAVE_GCC)
.if ${MKGCC} != "no"
.if (${HAVE_GCC} == "3" || ${HAVE_GCC} == "4")
	${MAKEDIRTARGET} . do-gnu-lib-crtstuff${LIBGCC_EXT}
.endif
	${MAKEDIRTARGET} . do-gnu-lib-libgcc${LIBGCC_EXT}
.endif
.endif

do-compat-libgcc: .PHONY .MAKE
.if defined(HAVE_GCC)
.if ${MKGCC} != "no"
	${MAKEDIRTARGET} . do-compat-gnu-lib-crtstuff${LIBGCC_EXT}
	${MAKEDIRTARGET} . do-compat-gnu-lib-libgcc${LIBGCC_EXT}
.endif
.endif

do-libpcc: .PHONY .MAKE
.if defined(HAVE_PCC)
.if ${MKPCC} != "no"
	${MAKEDIRTARGET} . do-pcc-lib-crtstuff
	${MAKEDIRTARGET} . do-pcc-lib-libpcc
.endif
.endif

do-ld.so: .PHONY .MAKE
.for targ in dependall install
.if (${OBJECT_FMT} == "a.out")
	${MAKEDIRTARGET} libexec/ld.aout_so ${targ}
.endif
.if (${OBJECT_FMT} == "ELF")
	${MAKEDIRTARGET} libexec/ld.elf_so ${targ}
.endif
.endfor

do-build: .PHONY .MAKE
.for targ in dependall install
	${MAKEDIRTARGET} . ${targ} BUILD_tools=no BUILD_lib=no
.endfor

do-x11: .PHONY .MAKE
.if ${MKX11} != "no"
.if ${X11FLAVOUR} == "Xorg"
	${MAKEDIRTARGET} external/mit/xorg build
.else
	${MAKEDIRTARGET} x11 build
.endif
.else
	@echo "MKX11 is not enabled"
	@false
.endif

do-obsolete: .PHONY .MAKE
	${MAKEDIRTARGET} etc install-obsolete-lists

#
# Speedup stubs for some subtrees that don't need to run these rules.
# (Tells <bsd.subdir.mk> not to recurse for them.)
#

.for dir in bin etc distrib games libexec regress sbin usr.sbin tools
includes-${dir}: .PHONY
	@true
.endfor
.for dir in etc distrib regress
install-${dir}: .PHONY
	@true
.endfor

#
# XXX this needs to change when distrib Makefiles are recursion compliant
# XXX many distrib subdirs need "cd etc && make snap_pre snap_kern" first...
#
dependall-distrib depend-distrib all-distrib: .PHONY
	@true

.include <bsd.sys.mk>
.include <bsd.obj.mk>
.include <bsd.kernobj.mk>
.include <bsd.subdir.mk>

build-docs: .PHONY ${.CURDIR}/BUILDING
${.CURDIR}/BUILDING: doc/BUILDING.mdoc
	${_MKMSG_CREATE} ${.TARGET}
	${TOOL_GROFF} -mdoc -Tascii -P-bou $> >$@


#
# Display current make(1) parameters
#
params: .PHONY .MAKE
	${MAKEDIRTARGET} etc params
