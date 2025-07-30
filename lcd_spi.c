#include "lcd_spi.h"
#include "spi.h"
#include "gpio.h"

#define LCD_CS   17
#define LCD_DC   20
#define LCD_RST  21

static void lcd_reset_spi(void) {
    gpio_set(LCD_RST,0);
    for (volatile int i = 0; i < 10000; ++i); 
    gpio_set(LCD_RST,1);
}

void lcd_init_spi(void) {
    gpio_set_output(LCD_CS);
    gpio_set_output(LCD_DC);
    gpio_set_output(LCD_RST);

    lcd_reset_spi();

    gpio_set(LCD_CS,0);
    lcd_send_command_spi(0xAE);  
    lcd_send_command_spi(0xA1);  
    lcd_send_command_spi(0xAF);  
    gpio_set(LCD_CS, 1);
}

void lcd_send_command_spi(uint8_t cmd) {
    gpio_set(LCD_DC,0);  
    gpio_set(LCD_CS,0);
    spi_send(cmd);
    gpio_set(LCD_CS,1);
}

void lcd_send_data_spi(uint8_t data) {
    gpio_set(LCD_DC,1); 
    gpio_set(LCD_CS,0);
    spi_send(data);
    gpio_set(LCD_CS,1);
}
