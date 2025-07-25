#include "spi.h"
#include "gpio.h"

#define SPI0_BASE      0x4003C000
#define SPI0_SSPCR0    (*(volatile uint32_t*)(SPI0_BASE + 0x000))
#define SPI0_SSPCR1    (*(volatile uint32_t*)(SPI0_BASE + 0x004))
#define SPI0_SSPDR     (*(volatile uint32_t*)(SPI0_BASE + 0x008))
#define SPI0_SSPSR     (*(volatile uint32_t*)(SPI0_BASE + 0x00C))
#define SPI0_SSPCPSR   (*(volatile uint32_t*)(SPI0_BASE + 0x010))
#define SPI0_SSPIMSC   (*(volatile uint32_t*)(SPI0_BASE + 0x014))
#define SPI0_SSPICR    (*(volatile uint32_t*)(SPI0_BASE + 0x020))

#define SPI_SSPCR1_SSE (1 << 1)
#define SPI_SSPSR_TNF  (1 << 1)
#define SPI_SSPSR_RNE  (1 << 2)

void spi_init() {
    
    gpio_set_function(16, 1);
    gpio_set_function(17, 1);
    gpio_set_function(18, 1);
    gpio_set_function(19, 1);

    SPI0_SSPCR1 = 0;
    SPI0_SSPCPSR = 2;
    SPI0_SSPCR0 = (0x7 << 0);
    SPI0_SSPCR1 = SPI_SSPCR1_SSE;
}

void spi_send(uint8_t data) {
    while (!(SPI0_SSPSR & SPI_SSPSR_TNF));
    SPI0_SSPDR = data;
}

uint8_t spi_receive() {
    while (!(SPI0_SSPSR & SPI_SSPSR_RNE));
    return SPI0_SSPDR;
}

void spi_send_buffer(uint8_t* data, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        spi_send(data[i]);
    }
}

void spi_receive_buffer(uint8_t* buffer, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        buffer[i] = spi_receive();
    }
}
