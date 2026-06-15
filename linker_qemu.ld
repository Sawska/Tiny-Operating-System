/* Linker script for QEMU 'lm3s6965evb' (Cortex-M3).
 * Flash is mapped at 0x0 (the vector table must live here so the core can
 * fetch the initial SP and reset vector on power-up). 64K of SRAM at the
 * usual Cortex-M location, which also matches what memory.c expects. */
ENTRY(_start)

MEMORY {
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
  .text : {
    KEEP(*(.vectors))      /* vector table MUST be first, at address 0x0 */
    *(.text.boot)
    *(.text*)
    *(.rodata*)
  } > FLASH

  /* .data lives in RAM at runtime but is stored (loaded) in FLASH right
   * after .text. boot.S copies it across at startup. */
  _sidata = LOADADDR(.data);
  .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM AT > FLASH

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  . = ALIGN(8);
  _stack_top = ORIGIN(RAM) + LENGTH(RAM);   /* top of RAM, grows down */
}
