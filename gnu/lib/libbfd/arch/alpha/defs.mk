# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-binutils,v 1.3 2003/12/04 14:38:00 mrg Exp 
#
G_libbfd_la_DEPENDENCIES=elf64-alpha.lo elf64.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo coff-alpha.lo ecoff.lo ecofflink.lo elf64-gen.lo elf32-gen.lo elf32.lo cpu-alpha.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo  cache.lo coffgen.lo corefile.lo format.lo init.lo libbfd.lo opncls.lo  reloc.lo section.lo syms.lo targets.lo hash.lo linker.lo srec.lo  binary.lo tekhex.lo ihex.lo stabs.lo stab-syms.lo merge.lo dwarf2.lo  simple.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H -I. -I/usr/src/tools/binutils/../../gnu/dist/binutils/bfd -I.
G_INCLUDES=-D_GNU_SOURCE  -DNETBSD_CORE   -I. -I/usr/src/tools/binutils/../../gnu/dist/binutils/bfd -I/usr/src/tools/binutils/../../gnu/dist/binutils/bfd/../include  -I/usr/src/tools/binutils/../../gnu/dist/binutils/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf64_alpha_vec -DSELECT_VECS='&bfd_elf64_alpha_vec,&ecoffalpha_little_vec,&bfd_elf64_little_generic_vec,&bfd_elf64_big_generic_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_alpha_arch' -DHAVE_bfd_elf64_alpha_vec -DHAVE_ecoffalpha_little_vec -DHAVE_bfd_elf64_little_generic_vec -DHAVE_bfd_elf64_big_generic_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
