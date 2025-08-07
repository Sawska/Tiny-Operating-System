ENTRY(_start)

MEMORY {
  FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
  .vectors : {
    KEEP(*(.vectors))
  } > FLASH


  .text : {
    *(.text.boot)
    *(.text*)
    *(.rodata*)
  } > FLASH

  .data : AT (LOADADDR(.text) + SIZEOF(.text)) {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss (NOLOAD) : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM


  .stack (NOLOAD) : {
    . = ALIGN(8);
    _stack_top = ORIGIN(RAM) + LENGTH(RAM); /* Very top of RAM */
  } > RAM
}
