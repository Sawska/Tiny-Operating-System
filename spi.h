#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void spi_init();
void spi_send(uint8_t data);
uint8_t spi_receive();
void spi_send_buffer(uint8_t* data, uint32_t length);
void spi_receive_buffer(uint8_t* buffer, uint32_t length);

#endif // SPI_H
