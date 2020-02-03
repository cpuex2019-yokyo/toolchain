OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS
{
  . = 0x80000000;
  .text :
  {
    *(.text)
    . = ALIGN(0x1000);
  }
  . = ALIGN(0x1000);
  PROVIDE(etext = .);
  .data : {
    *(.data)
  }
  .bss : {
    *(.bss)
    *(.sbss*)
     PROVIDE(end = .);
  }
}
