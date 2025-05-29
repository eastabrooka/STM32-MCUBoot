/* Entry Point */ 
ENTRY(Reset_Handler) 

_estack         = ORIGIN(RAM) + LENGTH(RAM);   /* end of RAM */ 
_Min_Heap_Size  = 0x200;                       /* heap size */ 
_Min_Stack_Size = 0x400;                       /* stack size */

/* Flash + RAM partitions, all on 2 KB-sector boundaries */
MEMORY
{
  /*  96 KB bootloader @ 0x0800_0000 */
  FLASH_BOOTLOADER (rx) : ORIGIN = 0x08000000, LENGTH = 96K

  /* 128 KB primary image @ 0x0801_8000 */
  FLASH_PRIMARY    (rx) : ORIGIN = 0x08018000, LENGTH = 128K

  /* 128 KB secondary image @ 0x0803_8000 */
  FLASH_SECONDARY  (rx) : ORIGIN = 0x08038000, LENGTH = 128K

  /*  16 KB scratch       @ 0x0805_8000 */
  FLASH_SCRATCH    (rx) : ORIGIN = 0x08058000, LENGTH = 16K

  /* RAM regions (unchanged) */
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 320K
  RAM2  (xrw) : ORIGIN = 0x10000000, LENGTH =  64K
}

/* Output sections: put ALL code/data in FLASH_BOOTLOADER */
SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector))
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .text :
  {
    . = ALIGN(8);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(8);
    _etext = .;
  } >FLASH_BOOTLOADER

  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .ARM.extab :
  {
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .ARM :
  {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .preinit_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
    . = ALIGN(8);
  } >FLASH_BOOTLOADER

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(8);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(8);
    _edata = .;
  } >RAM AT> FLASH_BOOTLOADER

  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /DISCARD/ :
  {
    libc.a(*)
    libm.a(*)
    libgcc.a(*)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
