
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)

set(TOOLCHAIN_PREFIX arm-none-eabi)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)

set(CMAKE_C_FLAGS_INIT "-Wall -nostdlib -ffreestanding -O2 -mcpu=cortex-m0 -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-Wall -nostdlib -ffreestanding -mcpu=cortex-m0 -mthumb")
