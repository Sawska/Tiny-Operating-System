CFLAGS = -Wall -nostdlib -ffreestanding -O2 -mcpu=cortex-m0 -mthumb

all: kernel.uf2

kernel.elf: boot.o kernel.o memory.o
	arm-none-eabi-ld -T linker.ld -o $@ $^

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c -o $@ $<

boot.o: boot.S
	arm-none-eabi-gcc $(CFLAGS) -c -o $@ $<

kernel.uf2: kernel.elf
	arm-none-eabi-objcopy -O binary $< kernel.bin
	pico-sdk/tools/elf2uf2/elf2uf2 $< $@

clean:
	rm -f *.o *.elf *.bin *.uf2
