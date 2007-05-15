/* Native-dependent code for NetBSD/amd64.

   Copyright (C) 2003, 2004 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "target.h"

#include "gdb_assert.h"

#include "nbsd-nat.h"
#include "amd64-tdep.h"
#include "amd64-nat.h"

#include <machine/frame.h>
#include <machine/pcb.h>

/* Mapping between the general-purpose registers in NetBSD/amd64
   `struct reg' format and GDB's register cache layout for
   NetBSD/i386.

   Note that most (if not all) NetBSD/amd64 registers are 64-bit,
   while the NetBSD/i386 registers are all 32-bit, but since we're
   little-endian we get away with that.  */

/* From <machine/reg.h>.  */
static int amd64nbsd32_r_reg_offset[] =
{
  14 * 8,			/* %eax */
  3 * 8,			/* %ecx */
  2 * 8,			/* %edx */
  13 * 8,			/* %ebx */
  24 * 8,			/* %esp */
  12 * 8,			/* %ebp */
  1 * 8,			/* %esi */
  0 * 8,			/* %edi */
  21 * 8,			/* %eip */
  23 * 8,			/* %eflags */
  22 * 8,			/* %cs */
  25 * 8,			/* %ss */
  18 * 8,			/* %ds */
  17 * 8,			/* %es */
  16 * 8,			/* %fs */
  15 * 8			/* %gs */
};


static int
amd64nbsd_supply_pcb (struct regcache *regcache, struct pcb *pcb)
{
  struct switchframe sf;
  int regnum;

  /* The following is true for NetBSD/amd64:

     The pcb contains the stack pointer at the point of the context
     switch in cpu_switch().  At that point we have a stack frame as
     described by `struct switchframe', which for NetBSD/amd64 has the
     following layout:

     interrupt level
     %r15
     %r14
     %r13
     %r12
     %rbp
     %rbx
     return address

     Together with %rsp in the pcb, this accounts for all callee-saved
     registers specified by the psABI.  From this information we
     reconstruct the register state as it would look when we just
     returned from cpu_switch().

     For kernel core dumps the pcb is saved by savectx().  In that case
     the stack frame only contains the return address, and there is no way
     to recover the other registers.  */

  /* The stack pointer shouldn't be zero.  */
  if (pcb->pcb_rsp == 0)
    return 0;

  /* Read the stack frame, and check its validity.  */
  read_memory (pcb->pcb_rsp, (gdb_byte *) &sf, sizeof sf);
  if (sf.sf_rbp == pcb->pcb_rbp)
    {
      /* Yes, we have a frame that matches cpu_switch().  */
      pcb->pcb_rsp += sizeof (struct switchframe);
      regcache_raw_supply (regcache, 12, &sf.sf_r12);
      regcache_raw_supply (regcache, 13, &sf.sf_r13);
      regcache_raw_supply (regcache, 14, &sf.sf_r14);
      regcache_raw_supply (regcache, 15, &sf.sf_r15);
      regcache_raw_supply (regcache, AMD64_RBX_REGNUM, &sf.sf_rbx);
      regcache_raw_supply (regcache, AMD64_RIP_REGNUM, &sf.sf_rip);
    }
  else
    {
      /* No, the pcb must have been last updated by savectx().  */
      pcb->pcb_rsp += 8;
      regcache_raw_supply (regcache, AMD64_RIP_REGNUM, &sf);
    }

  regcache_raw_supply (regcache, AMD64_RSP_REGNUM, &pcb->pcb_rsp);
  regcache_raw_supply (regcache, AMD64_RBP_REGNUM, &pcb->pcb_rbp);
  regcache_raw_supply (regcache, AMD64_FS_REGNUM, &pcb->pcb_fs);
  regcache_raw_supply (regcache, AMD64_GS_REGNUM, &pcb->pcb_gs);

  return 1;
}
/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_amd64nbsd_nat (void);

void
_initialize_amd64nbsd_nat (void)
{
  struct target_ops *t;

  amd64_native_gregset32_reg_offset = amd64nbsd32_r_reg_offset;
  amd64_native_gregset32_num_regs = ARRAY_SIZE (amd64nbsd32_r_reg_offset);
  amd64_native_gregset64_reg_offset = amd64nbsd_r_reg_offset;

  /* Add some extra features to the common *BSD/amd64 target.  */
  t = amd64bsd_target ();
  t->to_pid_to_exec_file = nbsd_pid_to_exec_file;
  add_target (t);

  /* Support debugging kernel virtual memory images.  */
  bsd_kvm_add_target (amd64nbsd_supply_pcb);

}
