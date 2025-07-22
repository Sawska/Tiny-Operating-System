cmake_minimum_required(VERSION 3.13)
project(pico_os C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb -nostdlib -ffreestanding")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")

add_executable(pico_os boot.S kernel.c)

target_link_options(pico_os PRIVATE -T${CMAKE_SOURCE_DIR}/linker.ld)
