/* Definitions to make GDB run on Convex Unix (4bsd)
   Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 

	$Id: m-convex.h,v 1.2 1993/08/02 17:39:52 mycroft Exp $
*/

/* Describe the endian nature of this machine.  */
#define BITS_BIG_ENDIAN
#define BYTES_BIG_ENDIAN
#define WORDS_BIG_ENDIAN

/* Include certain files for dbxread.c  */
#include <convex/filehdr.h>
#include <convex/opthdr.h>
#include <convex/scnhdr.h>
#include <nlist.h>

#define LONG_LONG
#define ATTACH_DETACH
#define HAVE_WAIT_STRUCT
#define NO_SIGINTERRUPT

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#define NAMES_HAVE_UNDERSCORE

/* Debugger information will be in DBX format.  */

#define READ_DBX_FORMAT

/* There is come problem with the debugging symbols generated by the
   compiler such that the debugging symbol for the first line of a
   function overlap with the function prologue.  */
#define PROLOGUE_FIRSTLINE_OVERLAP

/* When convex pcc says CHAR or SHORT, it provides the correct address.  */

#define BELIEVE_PCC_PROMOTION 1

/* Symbol types to ignore.  */
/* 0xc4 is N_MONPT.  Use the numeric value for the benefit of people
   with (rather) old OS's.  */
#define IGNORE_SYMBOL(TYPE) \
    (((TYPE) & ~N_EXT) == N_TBSS       \
     || ((TYPE) & ~N_EXT) == N_TDATA   \
     || ((TYPE) & ~N_EXT) == 0xc4)

/* Use SIGCONT rather than SIGTSTP because convex Unix occasionally
   turkeys SIGTSTP.  I think.  */

#define STOP_SIGNAL SIGCONT

/* Convex ld sometimes omits _etext.
   Get text segment end from a.out header in this case.  */

extern unsigned text_end;
#define END_OF_TEXT_DEFAULT text_end

/* Use csh to do argument expansion so we get ~ and such.  */

/* Doesn't work.  */
/* #define SHELL_FILE "/bin/csh" */

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.
   Convex prolog is:
       [sub.w #-,sp]		in one of 3 possible sizes
       [mov psw,-		fc/vc main program prolog
        and #-,-		  (skip it because the "mov psw" saves the
	mov -,psw]		   T bit, so continue gets a surprise trap)
       [and #-,sp]		fc/vc O2 main program prolog
       [ld.- -(ap),-]		pcc/gcc register arg loads
*/

#define SKIP_PROLOGUE(pc)  \
{ int op, ix;								\
  op = read_memory_integer (pc, 2);					\
  if ((op & 0xffc7) == 0x5ac0) pc += 2;					\
  else if (op == 0x1580) pc += 4;					\
  else if (op == 0x15c0) pc += 6;					\
  if ((read_memory_integer (pc, 2) & 0xfff8) == 0x7c40			\
      && (read_memory_integer (pc + 2, 2) & 0xfff8) == 0x1240		\
      && (read_memory_integer (pc + 8, 2) & 0xfff8) == 0x7c48)		\
    pc += 10;								\
  if (read_memory_integer (pc, 2) == 0x1240) pc += 6;			\
  for (;;) {								\
    op = read_memory_integer (pc, 2);					\
    ix = (op >> 3) & 7;							\
    if (ix != 6) break;							\
    if ((op & 0xfcc0) == 0x3000) pc += 4;				\
    else if ((op & 0xfcc0) == 0x3040) pc += 6;				\
    else if ((op & 0xfcc0) == 0x2800) pc += 4;				\
    else if ((op & 0xfcc0) == 0x2840) pc += 6;				\
    else break;}}

/* Immediately after a function call, return the saved pc.
   (ignore frame and return *$sp so we can handle both calls and callq) */

#define SAVED_PC_AFTER_CALL(frame) \
    read_memory_integer (read_register (SP_REGNUM), 4)

/* Address of end of stack space.
   This is ((USRSTACK + 0xfff) & -0x1000)) from <convex/vmparam.h> but
   that expression depends on the kernel version; instead, fetch a
   page-zero pointer and get it from that.  This will be invalid if
   they ever change the way bkpt signals are delivered.  */

#define STACK_END_ADDR (0xfffff000 & *(unsigned *) 0x80000050)

/* User-mode traps push an extended rtn block,
   then fault with one of the following PCs */

#define is_trace_pc(pc)  ((unsigned) ((pc) - (*(int *) 0x80000040)) <= 4)
#define is_arith_pc(pc)  ((unsigned) ((pc) - (*(int *) 0x80000044)) <= 4)
#define is_break_pc(pc)  ((unsigned) ((pc) - (*(int *) 0x80000050)) <= 4)

/* We need to manipulate trap bits in the psw */

#define PSW_TRAP_FLAGS	0x69670000
#define PSW_T_BIT	0x08000000
#define PSW_S_BIT	0x01000000

/* Stack grows downward.  */

#define INNER_THAN <

/* Sequence of bytes for breakpoint instruction. (bkpt)  */

#define BREAKPOINT {0x7d,0x50}

/* Amount PC must be decremented by after a breakpoint.
   This is often the number of bytes in BREAKPOINT but not always.
   (The break PC needs to be decremented by 2, but we do it when the
   break frame is recognized and popped.  That way gdb can tell breaks
   from trace traps with certainty.) */

#define DECR_PC_AFTER_BREAK 0

/* Nonzero if instruction at PC is a return instruction. (rtn or rtnq) */

#define ABOUT_TO_RETURN(pc) \
    ((read_memory_integer (pc, 2) & 0xffe0) == 0x7c80)

/* Return 1 if P points to an invalid floating point value. */

#define INVALID_FLOAT(p,len)   0

/* Largest integer type */

#define LONGEST long long

/* Name of the builtin type for the LONGEST type above.  */

#undef BUILTIN_TYPE_LONGEST
#define BUILTIN_TYPE_LONGEST builtin_type_long_long

/* Say how long (ordinary) registers are.  */

#define REGISTER_TYPE long long

/* Number of machine registers */

#define NUM_REGS 26

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

#define REGISTER_NAMES {"pc","psw","fp","ap","a5","a4","a3","a2","a1","sp",\
			"s7","s6","s5","s4","s3","s2","s1","s0",\
			"S7","S6","S5","S4","S3","S2","S1","S0"}

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define S0_REGNUM 25		/* the real S regs */
#define S7_REGNUM 18
#define s0_REGNUM 17		/* low-order halves of S regs */
#define s7_REGNUM 10
#define SP_REGNUM 9 		/* A regs */
#define A1_REGNUM 8
#define A5_REGNUM 4
#define AP_REGNUM 3
#define FP_REGNUM 2		/* Contains address of executing stack frame */
#define PS_REGNUM 1		/* Contains processor status */
#define PC_REGNUM 0		/* Contains program counter */

/* convert dbx stab register number (from `r' declaration) to a gdb REGNUM */

#define STAB_REG_TO_REGNUM(value) \
      ((value) < 8 ? S0_REGNUM - (value) : SP_REGNUM - ((value) - 8))

/* Vector register numbers, not handled as ordinary regs.
   They are treated as convenience variables whose values are read
   from the inferior when needed.  */

#define V0_REGNUM 0
#define V7_REGNUM 7
#define VM_REGNUM 8
#define VS_REGNUM 9
#define VL_REGNUM 10

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */
#define REGISTER_BYTES (4*10 + 8*8)

/* Index within `registers' of the first byte of the space for
   register N.
   NB: must match structure of struct syscall_context for correct operation */

#define REGISTER_BYTE(N) ((N) < s7_REGNUM ? 4*(N) : \
			  (N) < S7_REGNUM ? 44 + 8 * ((N)-s7_REGNUM) : \
			                    40 + 8 * ((N)-S7_REGNUM))

/* Number of bytes of storage in the actual machine representation
   for register N. */

#define REGISTER_RAW_SIZE(N) ((N) < S7_REGNUM ? 4 : 8)

/* Number of bytes of storage in the program's representation
   for register N.   */

#define REGISTER_VIRTUAL_SIZE(N) REGISTER_RAW_SIZE(N)

/* Largest value REGISTER_RAW_SIZE can have.  */

#define MAX_REGISTER_RAW_SIZE 8

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE 8

/* Nonzero if register N requires conversion
   from raw format to virtual format.  */

#define REGISTER_CONVERTIBLE(N) 0

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,FROM,TO)	\
   bcopy ((FROM), (TO), REGISTER_RAW_SIZE (REGNUM));

/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define REGISTER_CONVERT_TO_RAW(REGNUM,FROM,TO)	\
  bcopy ((FROM), (TO), REGISTER_RAW_SIZE (REGNUM));

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#define REGISTER_VIRTUAL_TYPE(N) \
   ((N) < S7_REGNUM ? builtin_type_int : builtin_type_long_long)

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */

#define STORE_STRUCT_RETURN(ADDR, SP) \
  { write_register (A1_REGNUM, (ADDR)); }

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  bcopy (&((char *) REGBUF) [REGISTER_BYTE (S0_REGNUM) + \
			     8 - TYPE_LENGTH (TYPE)],\
	 VALBUF, TYPE_LENGTH (TYPE))

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

#define STORE_RETURN_VALUE(TYPE,VALBUF) \
    write_register_bytes (REGISTER_BYTE (S0_REGNUM), VALBUF, 8)

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */

#define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) \
    (*(int *) & ((char *) REGBUF) [REGISTER_BYTE (s0_REGNUM)])

/* Compensate for lack of `vprintf' function.  */
#ifndef HAVE_VPRINTF
#define vprintf(format, ap) _doprnt (format, ap, stdout)
#endif /* not HAVE_VPRINTF */

/* Define trapped internal variable hooks to read and write
   vector and communication registers.  */

#define IS_TRAPPED_INTERNALVAR is_trapped_internalvar
#define VALUE_OF_TRAPPED_INTERNALVAR value_of_trapped_internalvar
#define SET_TRAPPED_INTERNALVAR set_trapped_internalvar

extern struct value *value_of_trapped_internalvar ();

/* Hooks to read data from soff exec and core files,
   and to describe the files.  */

#define XFER_CORE_FILE
#define FILES_INFO_HOOK print_maps

/* Hook to call after creating inferior process.  */

#define CREATE_INFERIOR_HOOK create_inferior_hook

/* Hook to call to print a typeless integer value, normally printed in decimal.
   For convex, use hex instead if the number looks like an address.  */

#define PRINT_TYPELESS_INTEGER decout

/* For the native compiler, variables for a particular lexical context
   are listed after the beginning LBRAC instead of before in the
   executables list of symbols.  Using "gcc_compiled." to distinguish
   between GCC and native compiler doesn't work on Convex because the
   linker sorts the symbols to put "gcc_compiled." in the wrong place.
   desc is nonzero for native, zero for gcc.   */
#define VARIABLES_INSIDE_BLOCK(desc) (desc != 0)

/* Pcc occaisionally puts an SO where there should be an SOL.   */
#define PCC_SOL_BROKEN

/* Cannot execute with pc on the stack.  */
#define CANNOT_EXECUTE_STACK

/* Describe the pointer in each stack frame to the previous stack frame
   (its caller).  */

/* FRAME_CHAIN takes a frame_info with a frame's nominal address in fi->frame,
   and produces the frame's chain-pointer.

   FRAME_CHAIN_COMBINE takes the chain pointer and the frame's nominal address
   and produces the nominal address of the caller frame.

   However, if FRAME_CHAIN_VALID returns zero,
   it means the given frame is the outermost one and has no caller.
   In that case, FRAME_CHAIN_COMBINE is not used.  */

/* (caller fp is saved at 8(fp)) */

#define FRAME_CHAIN(fi)   (read_memory_integer ((fi)->frame + 8, 4))

#define FRAME_CHAIN_VALID(chain, thisframe) \
  (chain != 0 && (outside_startup_file (FRAME_SAVED_PC (thisframe))))

#define FRAME_CHAIN_COMBINE(chain, thisframe) (chain)

/* Define other aspects of the stack frame.  */

/* A macro that tells us whether the function invocation represented
   by FI does not have a frame on the stack associated with it.  If it
   does not, FRAMELESS is set to 1, else 0.
   On convex, check at the return address for `callq' -- if so, frameless,
   otherwise, not.  */

#define FRAMELESS_FUNCTION_INVOCATION(FI, FRAMELESS) \
{ 									\
  extern CORE_ADDR text_start, text_end;				\
  CORE_ADDR call_addr = SAVED_PC_AFTER_CALL (FI);			\
  (FRAMELESS) = (call_addr >= text_start && call_addr < text_end	\
		 && read_memory_integer (call_addr - 6, 1) == 0x22);	\
}

#define FRAME_SAVED_PC(fi) (read_memory_integer ((fi)->frame, 4))

#define FRAME_ARGS_ADDRESS(fi) (read_memory_integer ((fi)->frame + 12, 4))

#define FRAME_LOCALS_ADDRESS(fi) (fi)->frame

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */

#define FRAME_NUM_ARGS(numargs, fi)  \
{ numargs = read_memory_integer (FRAME_ARGS_ADDRESS (fi) - 4, 4); \
  if (numargs < 0 || numargs >= 256) numargs = -1;}

/* Return number of bytes at start of arglist that are not really args.  */

#define FRAME_ARGS_SKIP 0

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.  */

/* Normal (short) frames save only PC, FP, (callee's) AP.  To reasonably
   handle gcc and pcc register variables, scan the code following the
   call for the instructions the compiler inserts to reload register
   variables from stack slots and record the stack slots as the saved
   locations of those registers.  This will occasionally identify some
   random load as a saved register; this is harmless.  vc does not
   declare its register allocation actions in the stabs.  */

#define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)		\
{ register int regnum;							\
  register int frame_length =	/* 3 short, 2 long, 1 extended, 0 context */\
      (read_memory_integer ((frame_info)->frame + 4, 4) >> 25) & 3;	\
  register CORE_ADDR frame_fp =						\
      read_memory_integer ((frame_info)->frame + 8, 4);			\
  register CORE_ADDR next_addr;						\
  bzero (&frame_saved_regs, sizeof frame_saved_regs);			\
  (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 0;		\
  (frame_saved_regs).regs[PS_REGNUM] = (frame_info)->frame + 4;		\
  (frame_saved_regs).regs[FP_REGNUM] = (frame_info)->frame + 8;		\
  (frame_saved_regs).regs[AP_REGNUM] = frame_fp + 12;			\
  next_addr = (frame_info)->frame + 12;					\
  if (frame_length < 3)							\
    for (regnum = A5_REGNUM; regnum < SP_REGNUM; ++regnum)		\
      (frame_saved_regs).regs[regnum] = (next_addr += 4);		\
  if (frame_length < 2)							\
    (frame_saved_regs).regs[SP_REGNUM] = (next_addr += 4);		\
  next_addr -= 4;							\
  if (frame_length < 3)							\
    for (regnum = S7_REGNUM; regnum < S0_REGNUM; ++regnum)		\
      (frame_saved_regs).regs[regnum] = (next_addr += 8);		\
  if (frame_length < 2)							\
    (frame_saved_regs).regs[S0_REGNUM] = (next_addr += 8);		\
  else									\
    (frame_saved_regs).regs[SP_REGNUM] = next_addr + 8;			\
  if (frame_length == 3) {						\
    CORE_ADDR pc = read_memory_integer ((frame_info)->frame, 4);	\
    int op, ix, disp;							\
    op = read_memory_integer (pc, 2);					\
    if ((op & 0xffc7) == 0x1480) pc += 4; 	/* add.w #-,sp */	\
    else if ((op & 0xffc7) == 0x58c0) pc += 2;	/* add.w #-,sp */	\
    op = read_memory_integer (pc, 2);					\
    if ((op & 0xffc7) == 0x2a06) pc += 4;	/* ld.w -,ap */		\
    for (;;) {								\
      op = read_memory_integer (pc, 2);					\
      ix = (op >> 3) & 7;						\
      if ((op & 0xfcc0) == 0x2800) {		/* ld.- -,ak */		\
        regnum = SP_REGNUM - (op & 7);					\
	disp = read_memory_integer (pc + 2, 2);				\
	pc += 4;}							\
      else if ((op & 0xfcc0) == 0x2840) {	/* ld.- -,ak */		\
        regnum = SP_REGNUM - (op & 7);					\
	disp = read_memory_integer (pc + 2, 4);				\
	pc += 6;}							\
      if ((op & 0xfcc0) == 0x3000) {		/* ld.- -,sk */		\
        regnum = S0_REGNUM - (op & 7);					\
	disp = read_memory_integer (pc + 2, 2);				\
	pc += 4;}							\
      else if ((op & 0xfcc0) == 0x3040) {	/* ld.- -,sk */		\
        regnum = S0_REGNUM - (op & 7);					\
	disp = read_memory_integer (pc + 2, 4);				\
	pc += 6;}							\
      else if ((op & 0xff00) == 0x7100) {	/* br crossjump */	\
        pc += 2 * (char) op;						\
        continue;}							\
      else if (op == 0x0140) {			/* jmp crossjump */	\
        pc = read_memory_integer (pc + 2, 4);				\
        continue;}							\
      else break;							\
      if ((frame_saved_regs).regs[regnum])				\
	break;								\
      if (ix == 7) disp += frame_fp;					\
      else if (ix == 6) disp += read_memory_integer (frame_fp + 12, 4);	\
      else if (ix != 0) break;						\
      (frame_saved_regs).regs[regnum] =					\
	disp - 8 + (1 << ((op >> 8) & 3));				\
      if (regnum >= S7_REGNUM)						\
        (frame_saved_regs).regs[regnum - S0_REGNUM + s0_REGNUM] =	\
	  disp - 4 + (1 << ((op >> 8) & 3));				\
    }									\
  }									\
}

/* Things needed for making the inferior call functions.  */

/* Push an empty stack frame, to record the current PC, etc.  */

#define PUSH_DUMMY_FRAME \
{ register CORE_ADDR sp = read_register (SP_REGNUM);			\
  register int regnum;				    			\
  char buf[8];					    			\
  long word;								\
  for (regnum = S0_REGNUM; regnum >= S7_REGNUM; --regnum) { 		\
    read_register_bytes (REGISTER_BYTE (regnum), buf, 8); 		\
    sp = push_bytes (sp, buf, 8);}		    			\
  for (regnum = SP_REGNUM; regnum >= FP_REGNUM; --regnum) { 		\
    word = read_register (regnum);					\
    sp = push_bytes (sp, &word, 4);}   					\
  word = (read_register (PS_REGNUM) &~ (3<<25)) | (1<<25);		\
  sp = push_bytes (sp, &word, 4); 					\
  word = read_register (PC_REGNUM);					\
  sp = push_bytes (sp, &word, 4);   					\
  write_register (SP_REGNUM, sp);		    			\
  write_register (FP_REGNUM, sp);		    			\
  write_register (AP_REGNUM, sp);}

/* Discard from the stack the innermost frame, restoring all registers.  */

#define POP_FRAME  do {\
  register CORE_ADDR fp = read_register (FP_REGNUM);	    \
  register int regnum;					    \
  register int frame_length =	/* 3 short, 2 long, 1 extended, 0 context */ \
      (read_memory_integer (fp + 4, 4) >> 25) & 3;          \
  char buf[8];					            \
  write_register (PC_REGNUM, read_memory_integer (fp, 4));  \
  write_register (PS_REGNUM, read_memory_integer (fp += 4, 4));  \
  write_register (FP_REGNUM, read_memory_integer (fp += 4, 4));  \
  write_register (AP_REGNUM, read_memory_integer (fp += 4, 4));  \
  if (frame_length < 3) 				     \
    for (regnum = A5_REGNUM; regnum < SP_REGNUM; ++regnum)   \
      write_register (regnum, read_memory_integer (fp += 4, 4)); \
  if (frame_length < 2)					     \
    write_register (SP_REGNUM, read_memory_integer (fp += 4, 4)); \
  fp -= 4;							\
  if (frame_length < 3)  					\
    for (regnum = S7_REGNUM; regnum < S0_REGNUM; ++regnum) {	\
      read_memory (fp += 8, buf, 8);				\
      write_register_bytes (REGISTER_BYTE (regnum), buf, 8);}   \
  if (frame_length < 2)	{					\
    read_memory (fp += 8, buf, 8);				\
    write_register_bytes (REGISTER_BYTE (regnum), buf, 8);}     \
  else write_register (SP_REGNUM, fp + 8);			\
  flush_cached_frames ();					\
  set_current_frame (create_new_frame (read_register (FP_REGNUM), \
				       read_pc ())); 		\
} while (0)

/* This sequence of words is the instructions
     mov sp,ap
     pshea 69696969
     calls 32323232
     bkpt
   Note this is 16 bytes.  */

#define CALL_DUMMY {0x50860d4069696969LL,0x2140323232327d50LL}

#define CALL_DUMMY_START_OFFSET 0

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, type)   \
{ *(int *)((char *) dummyname + 4) = nargs;	\
  *(int *)((char *) dummyname + 10) = fun; }

/* Defs to read soff symbol tables, see dbxread.c */

#define NUMBER_OF_SYMBOLS    ((long) opthdr.o_nsyms)
#define STRING_TABLE_OFFSET  ((long) filehdr.h_strptr)
#define SYMBOL_TABLE_OFFSET  ((long) opthdr.o_symptr)
#define STRING_TABLE_SIZE    ((long) filehdr.h_strsiz)
#define SIZE_OF_TEXT_SEGMENT ((long) txthdr.s_size)
#define ENTRY_POINT          ((long) opthdr.o_entry)

#define READ_STRING_TABLE_SIZE(BUFFER) \
    (BUFFER = STRING_TABLE_SIZE)

#define DECLARE_FILE_HEADERS \
  FILEHDR filehdr;							\
  OPTHDR opthdr;							\
  SCNHDR txthdr

#define READ_FILE_HEADERS(DESC,NAME) \
{									\
  int n;								\
  val = myread (DESC, &filehdr, sizeof filehdr);			\
  if (val < 0)								\
    perror_with_name (NAME);						\
  if (! IS_SOFF_MAGIC (filehdr.h_magic))				\
    error ("%s: not an executable file.", NAME);			\
  lseek (DESC, 0L, 0);							\
  if (myread (DESC, &filehdr, sizeof filehdr) < 0)			\
    perror_with_name (NAME);						\
  if (myread (DESC, &opthdr, filehdr.h_opthdr) <= 0)			\
    perror_with_name (NAME);						\
  for (n = 0; n < filehdr.h_nscns; n++)					\
    {									\
      if (myread (DESC, &txthdr, sizeof txthdr) < 0)			\
	perror_with_name (NAME);					\
      if ((txthdr.s_flags & S_TYPMASK) == S_TEXT)			\
	break;								\
    }									\
}

/* Interface definitions for kernel debugger KDB.  */

/* (no kdb) */
