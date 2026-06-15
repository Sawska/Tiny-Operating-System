// ----------------------------------------------------------------------------
// RP2040 second-stage bootloader (generic 03h), self-contained version.
//
// The RP2040 boot ROM copies the first 256 bytes of flash into SRAM, verifies
// the CRC32 stored in the last 4 bytes, and (if valid) executes this code. Its
// job is to configure the SSI/XIP hardware so the external QSPI flash can be
// read via the 0x18000000 XIP window, then hand off to the application's
// vector table at 0x10000100.
//
// This is a literal-value transcription of pico-sdk's boot2_generic_03h.S +
// exit_from_boot2.S, with the SSI register offsets/fields inlined so it builds
// without the Pico SDK headers. The "03h" command works on essentially any
// SPI flash (slow but universal), so no flash-chip-specific tuning is needed.
//
// Must be position-independent and <= 252 bytes (the build pads to 256 and
// appends the CRC32). All memory access is via PC-relative literal loads.
// ----------------------------------------------------------------------------

.syntax unified
.cpu cortex-m0plus
.thumb

// SSI / XIP register addresses (RP2040 datasheet)
.equ XIP_SSI_BASE,        0x18000000
.equ SSI_CTRLR0_OFFSET,   0x00
.equ SSI_CTRLR1_OFFSET,   0x04
.equ SSI_SSIENR_OFFSET,   0x08
.equ SSI_BAUDR_OFFSET,    0x14
.equ SSI_SPI_CTRLR0_OFFSET, 0xf4

// CTRLR0:    SPI_FRF=std(0)<<21 | DFS_32=31<<16 | TMOD=EEPROM_READ(3)<<8
.equ CTRLR0_XIP,      0x001f0300
// SPI_CTRLR0: XIP_CMD=0x03<<24 | ADDR_L=6<<2 | INST_L=2<<8 | TRANS_TYPE=1C1A(0)
.equ SPI_CTRLR0_XIP,  0x03000218

.equ PICO_FLASH_SPI_CLKDIV, 4

.equ VTOR_ADDR,       0xe000ed08      // M0PLUS VTOR
.equ APP_VTABLE,      0x10000100      // flash + 0x100

.section .boot2_src, "ax"
.global _stage2_boot
.type _stage2_boot, %function
.thumb_func
_stage2_boot:
    push {lr}

    ldr r3, =XIP_SSI_BASE

    // Disable SSI to allow configuration
    movs r1, #0
    str r1, [r3, #SSI_SSIENR_OFFSET]

    // Baud rate
    movs r1, #PICO_FLASH_SPI_CLKDIV
    str r1, [r3, #SSI_BAUDR_OFFSET]

    // CTRLR0: 32-bit data frames, EEPROM read mode
    ldr r1, =CTRLR0_XIP
    str r1, [r3, #SSI_CTRLR0_OFFSET]

    // SPI_CTRLR0: 03h command prefix, 24 address bits
    ldr r1, =SPI_CTRLR0_XIP
    ldr r0, =(XIP_SSI_BASE + SSI_SPI_CTRLR0_OFFSET)
    str r1, [r0]

    // NDF = 0 (single 32-bit read per XIP access)
    movs r1, #0
    str r1, [r3, #SSI_CTRLR1_OFFSET]

    // Re-enable SSI -- XIP is now live
    movs r1, #1
    str r1, [r3, #SSI_SSIENR_OFFSET]

    // ---- exit_from_boot2 ----
    // If entered from the boot ROM, the pushed lr is 0 -> vector into the flash
    // image. Otherwise (called from RAM) just return to the caller.
    pop {r0}
    cmp r0, #0
    beq vector_into_flash
    bx r0

vector_into_flash:
    ldr r0, =APP_VTABLE
    ldr r1, =VTOR_ADDR
    str r0, [r1]                 // point VTOR at the app vector table
    ldmia r0!, {r1, r2}          // r1 = initial SP, r2 = reset handler
    msr msp, r1
    bx r2

.ltorg
