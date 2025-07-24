.syntax unified
.thumb
.global switch_context

// void switch_context(uint32_t** old_sp, uint32_t* new_sp)
switch_context:
    // store current task context (R4-R11) to current stack
    mrs r2, psp
    stmdb r2!, {r4-r11}
    str r2, [r0]       // save new SP to *old_sp

    ldr r2, [r1]       // load new SP from new_sp
    ldmia r2!, {r4-r11}
    msr psp, r2
    bx lr
