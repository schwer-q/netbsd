# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-binutils,v 1.5 2006/02/02 20:06:04 skrll Exp 
#
G_libbfd_la_DEPENDENCIES=elf32-hppa.lo elf32.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo elf32-gen.lo cpu-hppa.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo  cache.lo coffgen.lo corefile.lo format.lo init.lo libbfd.lo  opncls.lo reloc.lo section.lo syms.lo targets.lo hash.lo  linker.lo srec.lo binary.lo tekhex.lo ihex.lo stabs.lo  stab-syms.lo merge.lo dwarf2.lo simple.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H
G_INCLUDES=-D_GNU_SOURCE  -DNETBSD_CORE   -I. -I${GNUHOSTDIST}/bfd -I${GNUHOSTDIST}/bfd/../include  -I${GNUHOSTDIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf32_hppa_nbsd_vec -DSELECT_VECS='&bfd_elf32_hppa_nbsd_vec,&bfd_elf32_hppa_vec,&bfd_elf32_hppa_linux_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_hppa_arch' -DHAVE_bfd_elf32_hppa_nbsd_vec -DHAVE_bfd_elf32_hppa_vec -DHAVE_bfd_elf32_hppa_linux_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
