#!/usr/bin/env bash
#
# Build the QEMU image and boot it on an emulated Cortex-M3 board.
# Requires: arm-none-eabi-gcc, cmake, qemu-system-arm.
#
# Exit QEMU with Ctrl-A then X.
set -euo pipefail

cd "$(dirname "$0")"

cmake -B build-qemu -DQEMU=ON -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake >/dev/null
cmake --build build-qemu

echo ">>> Booting Tiny-OS in QEMU (Ctrl-A X to quit)..."
exec qemu-system-arm -M lm3s6965evb -nographic -kernel build-qemu/kernel.elf
