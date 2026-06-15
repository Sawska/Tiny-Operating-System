#!/usr/bin/env bash
# Regenerate boot2_padded.S from boot2.S.
#
# The RP2040 boot ROM requires a 256-byte second-stage bootloader at the start
# of flash, ending in a CRC32 it verifies before executing. This script
# assembles boot2.S, extracts the raw machine code, then pads it to 256 bytes
# and appends the checksum (same -s 0xffffffff seed the pico-sdk uses).
#
# Run from the repo root after editing boot2.S:  ./tools/build_boot2.sh
set -euo pipefail
cd "$(dirname "$0")/.."

tmp="$(mktemp -d)"
arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -nostdlib -ffreestanding -c boot2.S -o "$tmp/boot2.o"
arm-none-eabi-ld -Ttext=0x0 "$tmp/boot2.o" -o "$tmp/boot2.elf"
arm-none-eabi-objcopy -O binary "$tmp/boot2.elf" "$tmp/boot2.bin"

size=$(wc -c < "$tmp/boot2.bin")
if [ "$size" -gt 252 ]; then
    echo "boot2 too large: $size bytes (max 252)" >&2
    exit 1
fi

python3 tools/pad_checksum -s 0xffffffff "$tmp/boot2.bin" boot2_padded.S
echo "Wrote boot2_padded.S ($size bytes of code + padding + CRC32)"
rm -rf "$tmp"
