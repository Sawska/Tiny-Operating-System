void kernel_main(void) {
    volatile unsigned int* led = (unsigned int*)(0xd0000000 + 0x00000010); // SIO GPIO OUT
    volatile unsigned int* gpio_oe = (unsigned int*)(0xd0000000 + 0x00000020); // OE register

    *gpio_oe |= (1 << 25); 

    while (1) {
        *led |= (1 << 25);  
        for (volatile int i = 0; i < 500000; i++); // delay

        *led &= ~(1 << 25); 
        for (volatile int i = 0; i < 500000; i++); // delay
    }
}
