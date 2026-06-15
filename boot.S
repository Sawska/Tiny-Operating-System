.syntax unified
.cpu cortex-m0
.thumb

.section .text.boot

.thumb_func
.global _start
.type _start, %function
.extern kernel_main
.extern _sbss
.extern _ebss
.extern _sdata
.extern _edata
.extern _sidata
.extern _stack_top

_start:
    /* Set stack pointer */
    ldr r0, =_stack_top
    mov sp, r0

    /* Copy .data from its load address in FLASH to its run address in RAM.
     * Without this, initialized globals (e.g. heap_ptr, current_task) are
     * never set and the kernel reads garbage. */
    ldr r0, =_sidata        /* source (FLASH) */
    ldr r1, =_sdata         /* dest start (RAM) */
    ldr r2, =_edata         /* dest end   (RAM) */

copy_data:
    cmp r1, r2
    bcs data_done
    ldr r3, [r0]
    str r3, [r1]
    adds r0, #4
    adds r1, #4
    b   copy_data

data_done:
    /* Zero .bss section */
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0

zero_bss:
    cmp r1, r2
    bcc clear_word
    b   bss_done

clear_word:
    str r3, [r1]
    adds r1, #4
    b   zero_bss

bss_done:
    /* Call kernel_main */
    bl kernel_main

hang:
    b hang

.thumb_func
.global enable_irq
.type enable_irq, %function
enable_irq:
    cpsie i
    bx lr
