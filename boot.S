.section .text.boot
.global _start

.extern kernel_main
.extern _sbss
.extern _ebss
.extern _stack_top

_start:
    ldr sp, =_stack_top

    ldr r0, =_sbss
    ldr r1, =_ebss
zero_bss:
    cmp r0, r1
    itt lt
    strlt r2, [r0], 
    blt zero_bss

    bl kernel_main

hang:
    b hang

.global enable_irq
enable_irq:
    cpsie i
    bx lr
    
.extern SVC_Handler
.word SVC_Handler