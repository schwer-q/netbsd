/* Native-dependent code for Motorola m68k's running NetBSD, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>
#include <machine/pcb.h>

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"

void
fetch_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;

  ptrace (PT_GETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);
  memcpy (&registers[REGISTER_BYTE (0)], &inferior_registers,
	  sizeof (inferior_registers));

  ptrace (PT_GETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);
  memcpy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
	  sizeof (inferior_fp_registers));

  registers_fetched ();
}

void
store_inferior_registers (regno)
     int regno;
{
  struct reg inferior_registers;
  struct fpreg inferior_fp_registers;

  memcpy (&inferior_registers, &registers[REGISTER_BYTE (0)],
	  sizeof (inferior_registers));
  ptrace (PT_SETREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_registers, 0);

  memcpy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	  sizeof (inferior_fp_registers));
  ptrace (PT_SETFPREGS, inferior_pid,
	  (PTRACE_ARG3_TYPE) & inferior_fp_registers, 0);
}

struct md_core
{
  struct reg intreg;
  struct fpreg freg;
};

static void
fetch_core_registers (core_reg_sect, core_reg_size, which, ignore)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     CORE_ADDR ignore;
{
  struct md_core *core_reg = (struct md_core *) core_reg_sect;

  /* Integer registers */
  memcpy (&registers[REGISTER_BYTE (0)],
	  &core_reg->intreg, sizeof (struct reg));
  /* Floating point registers */
  memcpy (&registers[REGISTER_BYTE (FP0_REGNUM)],
	  &core_reg->freg, sizeof (struct fpreg));
}

#ifdef	FETCH_KCORE_REGISTERS
/*
 * Get registers from a kernel crash dump or live kernel.
 * Called by kcore-nbsd.c:get_kcore_registers().
 */
void
fetch_kcore_registers (pcb)
     struct pcb *pcb;
{
  int i, *ip, tmp=0;

  /* D0,D1 */
  ip = &tmp;
  supply_register(0, (char *)ip);
  supply_register(1, (char *)ip);
  /* D2-D7 */
  ip = &pcb->pcb_regs[0];
  for (i = 2; i < 8; i++, ip++)
    supply_register(i, (char *)ip);

  /* A0,A1 */
  ip = &tmp;
  supply_register(8, (char *)ip);
  supply_register(9, (char *)ip);
  /* A2-A7 */
  ip = &pcb->pcb_regs[6];
  for (i = 10; i < 16; i++, (char *)ip++)
    supply_register(i, (char *)ip);

  /* PS (sr) */
  tmp = pcb->pcb_ps & 0xFFFF;
  supply_register(PS_REGNUM, (char *)&tmp);

  /* PC (use return address) */
  tmp = pcb->pcb_regs[10] + 4;
  if (target_read_memory(tmp, (char *)&tmp, sizeof(tmp)))
    tmp = 0;
  supply_register(PC_REGNUM, (char *)&tmp);

  /* The kernel does not use the FPU, so ignore it. */
  registers_fetched ();
}
#endif	/* FETCH_KCORE_REGISTERS */

/* Register that we are able to handle m68knbsd core file formats.
   FIXME: is this really bfd_target_unknown_flavour? */
   
static struct core_fns m68knbsd_core_fns =
{  
  bfd_target_unknown_flavour,           /* core_flavour */
  default_check_format,                 /* check_format */
  default_core_sniffer,                 /* core_sniffer */
  fetch_core_registers,                 /* core_read_registers */
  NULL                                  /* next */
}; 
   
void
_initialize_m68knbsd_nat ()
{
  add_core_fns (&m68knbsd_core_fns);
}
