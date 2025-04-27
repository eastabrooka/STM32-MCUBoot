/* bootloader.ld */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 32K    /* Bootloader in first 32KB */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 320K   /* SRAM1 + SRAM2 combined */
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))   /* Startup vector table */
    } > FLASH

    .text :
    {
        *(.text*)              /* All code */
        *(.rodata*)            /* Read-only data */
        *(.glue_7) *(.glue_7t) /* ARM ABI support */
        *(.eh_frame*)
    } > FLASH

    /* Provide start address for .data initialization in RAM */
    _sidata = LOADADDR(.data);

    .data : AT(LOADADDR(.text) + SIZEOF(.text))
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /* Optional: define a stack */
    ._stack (NOLOAD) :
    {
        . = ALIGN(8);
        KEEP(*(.stack*))
    } > RAM
}

/* Define end symbol for _sbrk */
PROVIDE(end = _ebss);
