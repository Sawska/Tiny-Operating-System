cmake_minimum_required(VERSION 3.16)
project(TinyOperatingSystem C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m0 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0 -mthumb -Wall -Wextra -O2")


set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)

set(KERNEL_SOURCES
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
)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(kernel.elf ${KERNEL_SOURCES})

target_link_options(kernel.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
)

add_custom_command(TARGET kernel.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary kernel.elf kernel.bin
    COMMAND ${CMAKE_SOURCE_DIR}/pico-sdk/tools/elf2uf2/elf2uf2 kernel.elf kernel.uf2
    COMMENT "Generating kernel.bin and kernel.uf2"
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "kernel.bin;kernel.uf2")



# -------------------------
# Add Tests
# -------------------------

enable_testing()


set(UNITY_DIR ${CMAKE_SOURCE_DIR}/Unity/src)
set(UNITY_SRC ${UNITY_DIR}/unity.c)
set(UNITY_INCLUDE ${UNITY_DIR})


file(GLOB TEST_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/tests
    tests/test_*.c
)

add_executable(tests
    ${TEST_SOURCES}
    ${UNITY_SRC}
)

target_include_directories(tests PRIVATE
    ${UNITY_INCLUDE}
    ${CMAKE_SOURCE_DIR}  
)


if(UNIX AND NOT APPLE)
    target_link_libraries(tests pthread)
endif()

add_test(NAME TinyOS_Tests COMMAND tests)
