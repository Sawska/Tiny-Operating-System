cmake_minimum_required(VERSION 3.16)
project(TinyOperatingSystem C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# QEMU=ON builds a kernel that boots and runs under
# `qemu-system-arm -M lm3s6965evb` (Cortex-M3). QEMU=OFF (default) builds the
# original Raspberry Pi Pico (RP2040) image.
option(QEMU "Build for the QEMU lm3s6965evb board" OFF)

if(QEMU)
    # ---------------- QEMU lm3s6965evb (Cortex-M3) ----------------
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker_qemu.ld)

    set(KERNEL_SOURCES
        boot.S
        vectors_qemu.c
        scheduler.c
        memory.c
        mailbox.c
        ipc.c
        uart.c
        log.c
        syscall_dispatch.c
        main_qemu.c
    )

    include_directories(${CMAKE_SOURCE_DIR})
    add_executable(kernel.elf ${KERNEL_SOURCES})

    target_compile_definitions(kernel.elf PRIVATE BOARD_LM3S)

    target_compile_options(kernel.elf PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -Wall
        -Wextra
        -O2
        -ffreestanding
        -fno-builtin
    )
    target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m3 -mthumb -ffreestanding>
    )

    target_link_options(kernel.elf PRIVATE
        "-T${LINKER_SCRIPT}"
        "-nostdlib"
        "-Wl,--gc-sections"
        "-mthumb"
        "-mcpu=cortex-m3"
    )
    target_link_libraries(kernel.elf PRIVATE gcc)

    add_custom_command(TARGET kernel.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary kernel.elf kernel.bin
        COMMENT "Generating kernel.bin")
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "kernel.bin")

else()
    # ---------------- Raspberry Pi Pico / RP2040 (Cortex-M0+) ----------------
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)

    set(KERNEL_SOURCES
        boot2_padded.S
        boot.S
        kernel.c
        memory.c
        ipc.c
        mailbox.c
        lcd_i2c.c
        lcd_spi.c
        fs.c
        scheduler.c
        sync.c
        syscall.c
        syscall_dispatch.c
        uart.c
        vectors.c
        i2c.c
        spi.c
        log.c
    )

    include_directories(${CMAKE_SOURCE_DIR})
    add_executable(kernel.elf ${KERNEL_SOURCES})

    target_compile_options(kernel.elf PRIVATE
        -mcpu=cortex-m0
        -mthumb
        -mthumb-interwork
        -Wall
        -Wextra
        -O2
        -ffreestanding
        -fno-builtin
    )
    target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m0 -mthumb -ffreestanding>
    )

    target_link_options(kernel.elf PRIVATE
        "-T${LINKER_SCRIPT}"
        "-nostdlib"
        "-Wl,--gc-sections"
        "-mthumb"
        "-mcpu=cortex-m0"
    )
    target_link_libraries(kernel.elf PRIVATE gcc)

    add_custom_command(TARGET kernel.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary kernel.elf kernel.bin
        COMMAND /opt/homebrew/bin/elf2uf2-rs kernel.elf kernel.uf2
        COMMENT "Generating kernel.bin and kernel.uf2")
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "kernel.bin;kernel.uf2")
endif()
