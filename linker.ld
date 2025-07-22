ENTRY(_start)

MEMORY {
  RAM (rwx) : ORIGIN = 0x10000000, LENGTH = 256K
}

SECTIONS {
  .text : {
    *(.text.boot)
    *(.text*)
  } > RAM

  .rodata : {
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss : {
    *(.bss*)
    *(COMMON)
  } > RAM

  .stack (NOLOAD) : {
    . = ALIGN(8);
    _stack_top = . + 0x1000;  /* 4KB stack */
  } > RAM
}
