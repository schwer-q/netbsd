/*	$NetBSD: shl.x,v 1.3 2001/02/21 16:28:02 uch Exp $	*/

OUTPUT_FORMAT("coff-shl")
OUTPUT_ARCH(sh)
MEMORY
{
  ram : o = 0x8C001000, l = 16M
}
SECTIONS
{
  ROM = 0x80001000;

  .text :
  AT (ROM)
  {
    *(.text)
    *(.strings)
     _etext = . ; 
  }  > ram
  .tors :
  AT ( ROM + SIZEOF(.text))
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
  } > ram
  .data :
  AT ( ROM + SIZEOF(.text) + SIZEOF(.tors))
  {
    *(.data)
     _edata = . ; 
  }  > ram
  .bss :
  AT ( ROM + SIZEOF(.text) + SIZEOF(.tors) + SIZEOF(.data))
  {
     _bss_start = . ; 
    *(.bss)
    *(COMMON)
     _end = . ;  
  }  > ram
}
