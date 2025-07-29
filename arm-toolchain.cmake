set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(TOOLCHAIN_PREFIX arm-none-eabi)

find_program(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
find_program(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
find_program(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)

set(CMAKE_C_FLAGS_INIT "-Wall -nostdlib -ffreestanding -O2 -mcpu=cortex-m0plus -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-Wall -nostdlib -ffreestanding -mcpu=cortex-m0plus -mthumb")
