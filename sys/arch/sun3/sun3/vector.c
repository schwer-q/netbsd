/*
 * Copyright (c) 1993 Adam Glass
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Adam Glass.
 * 4. The name of the Author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Adam Glass ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Header: /cvsroot/src/sys/arch/sun3/sun3/Attic/vector.c,v 1.3 1993/08/28 15:38:34 glass Exp $
 */

#include "sys/systm.h"
#include "vector.h"

#define COPY_ENTRY16 COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, \
                     COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, \
                     COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, \
                     COPY_ENTRY, COPY_ENTRY, COPY_ENTRY, COPY_ENTRY

unsigned int vector_table[NVECTORS] = {
    COPY_ENTRY,                         /* 0: jmp 0x400:w (unused reset SSP)*/
    COPY_ENTRY,				/* 1: NOT USED (reset PC) */        
    COPY_ENTRY,    			/* 2: bus error */                  
    COPY_ENTRY,    			/* 3: address error */              
    COPY_ENTRY,    			/* 4: illegal instruction */        
    COPY_ENTRY,    			/* 5: zero divide */                
    COPY_ENTRY,    			/* 6: CHK instruction */            
    COPY_ENTRY,    			/* 7: TRAPV instruction */          
    COPY_ENTRY,    			/* 8: privilege violation */        
    COPY_ENTRY,    			/* 9: trace */                      
    COPY_ENTRY,    			/* 10: line 1010 emulator */        
    COPY_ENTRY,    			/* 11: line 1111 emulator */        
    COPY_ENTRY,    			/* 12: unassigned, reserved */      
    COPY_ENTRY,    			/* 13: coprocessor protocol violatio */
    COPY_ENTRY,    			/* 14: format error */              
    COPY_ENTRY,    			/* 15: uninitialized interrupt vecto */
    COPY_ENTRY,    			/* 16: unassigned, reserved */      
    COPY_ENTRY,    			/* 17: unassigned, reserved */      
    COPY_ENTRY,    			/* 18: unassigned, reserved */      
    COPY_ENTRY,    			/* 19: unassigned, reserved */      
    COPY_ENTRY,    			/* 20: unassigned, reserved */      
    COPY_ENTRY,    			/* 21: unassigned, reserved */      
    COPY_ENTRY,    			/* 22: unassigned, reserved */      
    COPY_ENTRY,    			/* 23: unassigned, reserved */      
    COPY_ENTRY,    			/* 24: spurious interrupt */        
    COPY_ENTRY,    			/* 25: level 1 interrupt autovector */
    COPY_ENTRY,    			/* 26: level 2 interrupt autovector */
    COPY_ENTRY,    			/* 27: level 3 interrupt autovector */
    COPY_ENTRY,    			/* 28: level 4 interrupt autovector */
    COPY_ENTRY,    			/* 29: level 5 interrupt autovector */
    COPY_ENTRY,    			/* 30: level 6 interrupt autovector */
    COPY_ENTRY,    			/* 31: level 7 interrupt autovector */
    COPY_ENTRY,    			/* 32: syscalls (at least on hp300)
    COPY_ENTRY,    			/* 33: sigreturn syscall or breakpoi */
    COPY_ENTRY,    			/* 34: breakpoint or sigreturn sysca */
    COPY_ENTRY,    			/* 35: TRAP instruction vector */   
    COPY_ENTRY,    			/* 36: TRAP instruction vector */   
    COPY_ENTRY,    			/* 37: TRAP instruction vector */   
    COPY_ENTRY,    			/* 38: TRAP instruction vector */   
    COPY_ENTRY,    			/* 39: TRAP instruction vector */   
    COPY_ENTRY,    			/* 40: TRAP instruction vector */   
    COPY_ENTRY,    			/* 41: TRAP instruction vector */   
    COPY_ENTRY,    			/* 42: TRAP instruction vector */   
    COPY_ENTRY,    			/* 43: TRAP instruction vector */   
    COPY_ENTRY,    			/* 44: TRAP instruction vector */   
    COPY_ENTRY,    			/* 45: TRAP instruction vector */   
    COPY_ENTRY,    			/* 46: TRAP instruction vector */   
    COPY_ENTRY,    			/* 47: TRAP instruction vector */   
    COPY_ENTRY,    			/* 48: FPCP branch/set on unordered */
    COPY_ENTRY,    			/* 49: FPCP inexact result */       
    COPY_ENTRY,    			/* 50: FPCP divide by zero */       
    COPY_ENTRY,    			/* 51: FPCP underflow */            
    COPY_ENTRY,    			/* 52: FPCP operand error */        
    COPY_ENTRY,    			/* 53: FPCP overflow */             
    COPY_ENTRY,    			/* 54: FPCP signalling NAN */       
    COPY_ENTRY,    			/* 55: FPCP unimplemented data type */
    COPY_ENTRY,    			/* 56: unassigned, reserved */      
    COPY_ENTRY,    			/* 57: unassigned, reserved */      
    COPY_ENTRY,    			/* 58: unassigned, reserved */      
    COPY_ENTRY,    			/* 59: unassigned, reserved */      
    COPY_ENTRY,    			/* 60: unassigned, reserved */      
    COPY_ENTRY,    			/* 61: unassigned, reserved */      
    COPY_ENTRY,    			/* 62: unassigned, reserved */      
    COPY_ENTRY,    			/* 63: unassigned, reserved */      

    COPY_ENTRY16,		        /* 64-79   */
    COPY_ENTRY16,		        /* 80-95   */
    COPY_ENTRY16,		        /* 96-111  */
    COPY_ENTRY16,		        /* 112-127 */

    COPY_ENTRY16,		        /* 128-143 */
    COPY_ENTRY16,		        /* 144-159 */
    COPY_ENTRY16,		        /* 160-175 */
    COPY_ENTRY16,		        /* 176-191 */

    COPY_ENTRY16,		        /* 192-207 */
    COPY_ENTRY16,		        /* 208-223 */
    COPY_ENTRY16,		        /* 224-239 */
    COPY_ENTRY16		        /* 240-255 */
    };


void set_vector_entry(entry, handler)
     int entry;
     void (*handler)();
{
    if ((entry <0) || (entry >= NVECTORS))
	panic("set_vector_entry: setting vector too high or low\n");
    vector_table[entry] = (unsigned int) handler;
}

unsigned int get_vector_entry(entry)
     int entry;
{
    if ((entry <0) || (entry >= NVECTORS))
	panic("get_vector_entry: setting vector too high or low\n");
    return vector_table[entry];
}
