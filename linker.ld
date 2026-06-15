ENTRY(_start)

MEMORY {
  FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
  /* RP2040 second-stage bootloader: exactly 256 bytes at the very start of
   * flash (0x10000000), ending in the CRC32 the boot ROM checks. */
  .boot2 : {
    __boot2_start = .;
    KEEP(*(.boot2))
    __boot2_end = .;
  } > FLASH
  ASSERT(__boot2_end - __boot2_start == 256,
         "boot2 must be exactly 256 bytes (run tools/build_boot2.sh)")

  /* Application vector table lands at 0x10000100; boot2 points VTOR here. */
  .vectors : {
    KEEP(*(.vectors))
  } > FLASH


  .text : {
    *(.text.boot)
    *(.text*)
    *(.rodata*)
  } > FLASH

  _sidata = LOADADDR(.text) + SIZEOF(.text);
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
