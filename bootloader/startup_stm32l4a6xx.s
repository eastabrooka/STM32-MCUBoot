    .syntax unified
    .cpu cortex-m4
    .thumb

    .section .isr_vector, "a", %progbits
    .type   g_pfnVectors, %object
    .size   g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word   _estack             /* Initial Stack Pointer */
    .word   Reset_Handler       /* Reset Handler */
    .word   Default_Handler     /* NMI Handler */
    .word   Default_Handler     /* HardFault Handler */
    .word   Default_Handler     /* MemManage Handler */
    .word   Default_Handler     /* BusFault Handler */
    .word   Default_Handler     /* UsageFault Handler */
    .word   0                   /* Reserved */
    .word   0                   /* Reserved */
    .word   0                   /* Reserved */
    .word   0                   /* Reserved */
    .word   Default_Handler     /* SVCall Handler */
    .word   Default_Handler     /* Debug Monitor Handler */
    .word   0                   /* Reserved */
    .word   Default_Handler     /* PendSV Handler */
    .word   Default_Handler     /* SysTick Handler */

    /* More interrupts could be added here if needed */

    .text
    .thumb_func
Reset_Handler:
    /* Initialize data and bss */
    LDR   r0, =_sidata
    LDR   r1, =_sdata
    LDR   r2, =_edata

copy_data:
    CMP   r1, r2
    ITTT  lt
    LDRlt r3, [r0], #4
    STRlt r3, [r1], #4
    BLT   copy_data

    LDR   r0, =_sbss
    LDR   r1, =_ebss
    MOVS  r2, #0

zero_bss:
    CMP   r0, r1
    IT   lt
    STRlt r2, [r0], #4
    BLT   zero_bss

    /* Call main */
    BL    main

    /* If main returns, loop forever */
hang:
    B hang

Default_Handler:
    B hang

/* Symbols provided by linker script */
    .extern _estack
    .extern _sidata
    .extern _sdata
    .extern _edata
    .extern _sbss
    .extern _ebss
