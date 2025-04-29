/* 
*******************************************************************************
**
**  File        : LinkerScript.ld 
**  Author      : STM32CubeMX 
**  Abstract    : Linker script for STM32L4A6ZGTx series 
*******************************************************************************
*/

/* Entry Point */ 
ENTRY(Reset_Handler) 
 
_estack = ORIGIN(RAM) + LENGTH(RAM);    /* end of RAM */ 
_Min_Heap_Size = 0x200;      /* required heap size */ 
_Min_Stack_Size = 0x400;     /* required stack size */
 
/* Specify the memory areas */ 
MEMORY 
{ 
  RAM (xrw)             : ORIGIN = 0x20000000, LENGTH = 320K 
  RAM2 (xrw)            : ORIGIN = 0x10000000, LENGTH = 64K 
  FLASH (rx)            : ORIGIN = 0x08000000, LENGTH = 128K

  /* extra flash regions for bootloader and update handling */
  FLASH_PRIMARY (rx)    : ORIGIN = 0x08020000, LENGTH = 128K 
  FLASH_SECONDARY (rx)  : ORIGIN = 0x08040000, LENGTH = 128K 
  FLASH_SCRATCH (rx)    : ORIGIN = 0x08060000, LENGTH = 16K 
}
 
/* Define output sections */ 
SECTIONS 
{ 
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector))
    . = ALIGN(8);
  } >FLASH
 
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
  } >FLASH
 
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(8);
  } >FLASH
 
  .ARM.extab :
  {
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >FLASH
 
  .ARM :
  {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >FLASH
 
  .preinit_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
    . = ALIGN(8);
  } >FLASH
 
  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
    . = ALIGN(8);
  } >FLASH
 
  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
    . = ALIGN(8);
  } >FLASH
 
  _sidata = LOADADDR(.data);
 
  .data :
  {
    . = ALIGN(8);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(8);
    _edata = .;
  } >RAM AT> FLASH
 
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
