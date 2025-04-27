/* bootloader.ld */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 32K    /* Bootloader in first 32KB */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 320K   /* SRAM1 + SRAM2 combined */
}

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

    .init_array :
    {
        KEEP(*(SORT(.init_array*)))
    } > FLASH

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        *(.data*)              /* Initialized data, will be copied to RAM */
    } > RAM

    .bss :
    {
        *(.bss*)
        *(COMMON)
    } > RAM

    /* Optional: define a stack */
    ._stack (NOLOAD) :
    {
        . = ALIGN(8);
        KEEP(*(.stack*))
    } > RAM
}

/* End of bootloader.ld */
