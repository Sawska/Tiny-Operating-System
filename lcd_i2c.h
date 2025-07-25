#include <stdint.h>
#ifndef LCD_I2C_H
#define LCD_I2C_H

void lcd_init(uint8_t i2c_addr);
void lcd_clear();
void lcd_write_char(char c);
void lcd_write_string(const char* str);

#endif
