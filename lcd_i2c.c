#include "lcd_i2c.h"
#include "i2c.h"

static uint8_t LCD_ADDR = 0x3C; 

void lcd_init(uint8_t i2c_addr) {
    LCD_ADDR = i2c_addr;
    i2c_write_register(LCD_ADDR, 0x00, 0x01); 
}

void lcd_clear() {
    i2c_write_register(LCD_ADDR, 0x00, 0x01);
}

void lcd_write_char(char c) {
    i2c_write_register(LCD_ADDR, 0x40, c); 
}

void lcd_write_string(const char* str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}
