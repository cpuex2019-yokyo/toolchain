OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
   . = 0x80000000;
  .text : {
    build/entry.o(.text) 
    . = 0x02000000;
    *(.text)
  }
  
  . = ALIGN(0x1000);
  
  .data : {
    *(.data)
  }
  .bss : {
    *(.bss)
    *(.sbss*)
  }
}
