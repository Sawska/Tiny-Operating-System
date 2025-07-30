#include "i2c.h"
#include "memory.h"
#include "gpio.h"

#define I2C0_BASE 0x40044000
#define I2C0_IC_CON (*(volatile uint32_t*)(I2C0_BASE + 0x00))
#define I2C0_IC_TAR (*(volatile uint32_t*)(I2C0_BASE + 0x04))
#define I2C0_IC_DATA_CMD (*(volatile uint32_t*)(I2C0_BASE + 0x10))
#define I2C0_IC_STATUS (*(volatile uint32_t*)(I2C0_BASE + 0x70))
#define I2C0_IC_ENABLE (*(volatile uint32_t*)(I2C0_BASE + 0x6C))
#define I2C0_IC_CLR_INTR (*(volatile uint32_t*)(I2C0_BASE + 0x40))

#define I2C_READ 1
#define I2C_WRITE 0

#define GPIO_BASE 0x40014000
#define GPIO_CTRL(gpio) (*(volatile uint32_t*)(GPIO_BASE + 0x04 + (gpio) * 8))


void i2c_init() {
    
    I2C0_IC_ENABLE = 0;
    I2C0_IC_CON = (1 << 0) | (1 << 5);  
    I2C0_IC_TAR = 0x00; 

    
    GPIO_CTRL(4) = 3;
    GPIO_CTRL(5) = 3;

    I2C0_IC_ENABLE = 1;
    delay_cycles(10000);
}

void i2c_write_byte(uint8_t addr, uint8_t data) {
    I2C0_IC_TAR = addr;
    while (!(I2C0_IC_STATUS & (1 << 0))) {} 
    I2C0_IC_DATA_CMD = data;
    delay_cycles(1000);
}

uint8_t i2c_read_byte(uint8_t addr) {
    I2C0_IC_TAR = addr;
    I2C0_IC_DATA_CMD = (1 << 8); 
    while (!(I2C0_IC_STATUS & (1 << 3))) {}
    return I2C0_IC_DATA_CMD & 0xFF;
}

int i2c_write_register(uint8_t addr, uint8_t reg, uint8_t data) {
    I2C0_IC_TAR = addr;
    while (!(I2C0_IC_STATUS & (1 << 0))) {}
    I2C0_IC_DATA_CMD = reg;
    while (!(I2C0_IC_STATUS & (1 << 0))) {}
    I2C0_IC_DATA_CMD = data;
    return 0;
}

int i2c_read_register(uint8_t addr, uint8_t reg, uint8_t* data) {
    I2C0_IC_TAR = addr;
    while (!(I2C0_IC_STATUS & (1 << 0))) {}
    I2C0_IC_DATA_CMD = reg;
    while (!(I2C0_IC_STATUS & (1 << 0))) {}
    I2C0_IC_DATA_CMD = (1 << 8); 
    while (!(I2C0_IC_STATUS & (1 << 3))) {}
    *data = I2C0_IC_DATA_CMD & 0xFF;
    return 0;
}
