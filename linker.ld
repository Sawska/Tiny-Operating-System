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
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  .stack (NOLOAD) : {
    . = ALIGN(8);
    _stack_top = . + 0x1000;
  } > RAM

  .vectors : {
    KEEP(*(.vectors))
} > RAM

}

