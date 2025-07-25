#ifndef LCD_SPI_H
#define LCD_SPI_H

#include <stdint.h>

void lcd_init_spi(void);
void lcd_send_command_spi(uint8_t cmd);
void lcd_send_data_spi(uint8_t data);

#endif // LCD_SPI_H
