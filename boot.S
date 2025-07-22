.section .text.boot
.global _start

_start:
    ldr,sp = stack_top
    bl kernel_main

hang:
    b hang