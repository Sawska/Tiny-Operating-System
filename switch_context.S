.syntax unified
.thumb

.thumb_func
.global switch_context
switch_context:
    mrs r2, psp
    push {r4-r7}
    mov r3, sp
    str r3, [r0]
    ldr r3, [r1]
    mov sp, r3
    pop {r4-r7}
    mov r3, sp
    msr psp, r3
    bx lr
