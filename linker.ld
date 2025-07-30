ENTRY(_start)

MEMORY {
  FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
  /* Vector table at start of flash */
  .vectors : {
    KEEP(*(.vectors))
  } > FLASH

  /* Code and read-only data in flash */
  .text : {
    *(.text.boot)
    *(.text*)
    *(.rodata*)
  } > FLASH

  /* Initialized data in RAM, load from flash */
  .data : AT (LOADADDR(.text) + SIZEOF(.text)) {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  /* Zero-initialized data */
  .bss (NOLOAD) : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  /* Align and define stack top */
  .stack (NOLOAD) : {
    . = ALIGN(8);
    _stack_top = ORIGIN(RAM) + LENGTH(RAM); /* Very top of RAM */
  } > RAM
}
