.syntax unified
.cpu cortex-m0
.thumb

.section .text.boot
.global _start

.extern kernel_main
.extern _sbss
.extern _ebss
.extern _stack_top

_start:
    /* Set stack pointer */
    ldr r0, =_stack_top
    mov sp, r0

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

.global enable_irq
enable_irq:
    cpsie i
    bx lr

.extern SVC_Handler
.word SVC_Handler
