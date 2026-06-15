#include "gpio.h"
#include "memory.h"
#include "scheduler.h"
#include "mailbox.h"
#include "uart.h"
#include "interrupts.h"
#include "i2c.h"
#include "lcd_i2c.h"
#include "lcd_spi.h"
#include "ipc.h"
#include "log.h"

Mailbox box;
IPCQueue sensor_to_logger;






void logger_task() {
    uint32_t msg;
    while (1) {
        if (ipc_receive(&sensor_to_logger, &msg)) {
            uart_puts("Logger received: ");
            uart_putc('0' + (msg % 10));
            uart_putc('\n');
        }
        for (volatile int i = 0; i < 100000; i++);
    }
}




int dht_read_bit(int pin) {
    while (!gpio_get(pin)); 
    delay_cycles(300);      
    return gpio_get(pin);
}

int dht_read_byte(int pin) {
    int byte = 0;
    for (int i = 0; i < 8; ++i) {
        byte <<= 1;
        byte |= dht_read_bit(pin);
        while (gpio_get(pin));
    }
    return byte;
}

void dht11_task() {
    int pin = 14; 
    while (1) {
        gpio_set_output(pin);
        gpio_set(pin, 0);
        delay_cycles(30000); 

        gpio_set_input(pin); 

        delay_cycles(3000); 

        while (gpio_get(pin));
        while (!gpio_get(pin));
        while (gpio_get(pin));

        int h_int = dht_read_byte(pin);
        int h_dec = dht_read_byte(pin);
        int t_int = dht_read_byte(pin);
        int t_dec = dht_read_byte(pin);
        int checksum = dht_read_byte(pin);

        if (((h_int + h_dec + t_int + t_dec) & 0xFF) == checksum) {
            uart_puts("DHT11: Temp = ");
            uart_putc('0' + (t_int / 10));
            uart_putc('0' + (t_int % 10));
            uart_puts(" C\n");

            uart_puts("DHT11: Hum = ");
            uart_putc('0' + (h_int / 10));
            uart_putc('0' + (h_int % 10));
            uart_puts(" %\n");
        } else {
            uart_puts("DHT11: checksum error\n");
        }

        for (volatile int i = 0; i < 1000000; i++); 
    }
}



void sensor_task() {
    gpio_init(15); 
    while (1) {
        int value = gpio_get(15);
        uart_puts("Sensor: value = ");
        uart_putc(value ? '1' : '0');
        uart_putc('\n');
        for (volatile int i = 0; i < 100000; ++i);
    }
}

void button_task() {
    int pin = 15;
    gpio_set_input(pin);
    while (1) {
        int value = gpio_get(pin);
        uart_puts("Button: ");
        uart_puts(value ? "RELEASED\n" : "PRESSED\n");
        for (volatile int i = 0; i < 500000; ++i); 
    }
}


void hc_sr04_task() {
    int trig = 16, echo = 17;

    gpio_set_output(trig);
    gpio_set_input(echo);

    while (1) {
        
        gpio_set(trig, 1);
        delay_cycles(300);
        gpio_set(trig, 0);

        
        while (!gpio_get(echo));
        volatile int start = 0;
        while (gpio_get(echo)) {
            start++;
        }

        
        int distance_cm = (start * 340) / (2 * 10000); 

        uart_puts("Distance: ");
        uart_putc('0' + (distance_cm / 10));
        uart_putc('0' + (distance_cm % 10));
        uart_puts(" cm\n");

        for (volatile int i = 0; i < 1000000; ++i);
    }
}




void task1() {
    int count = 0;
    while (1) {
        mailbox_send(&box, count++);
        uart_puts("Task1: sent value\n");
        for (volatile int i = 0; i < 100000; i++);
    }
}

void task2() {
    uint32_t msg = 0;
    while (1) {
        if (mailbox_receive(&box, &msg)) {
            uart_puts("Task2: received value = ");
            uart_putc('0' + (msg % 10));
            uart_putc('\n');
        }
        for (volatile int i = 0; i < 100000; i++);
    }
}

void led_task() {
    int pin = 18;
    gpio_set_output(pin);
    while (1) {
        gpio_set(pin, 1);
        uart_puts("LED ON\n");
        for (volatile int i = 0; i < 500000; i++);

        gpio_set(pin, 0);
        uart_puts("LED OFF\n");
        for (volatile int i = 0; i < 500000; i++);
    }
}

void buzzer_task() {
    int pin = 19;
    gpio_set_output(pin);
    while (1) {
        for (int i = 0; i < 200; ++i) {
            gpio_toggle(pin);
            delay_cycles(1000);
        }
        uart_puts("Buzzer: beeped\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void servo_task() {
    int pin = 20;
    gpio_set_output(pin);

    while (1) {
        for (int angle = 0; angle <= 180; angle += 30) {
            int pulse_width = 5000 + (angle * 27);

            gpio_set(pin, 1);
            delay_cycles(pulse_width);  
            gpio_set(pin, 0);
            delay_cycles(100000 - pulse_width);

            uart_puts("Servo angle set\n");
            for (volatile int d = 0; d < 200000; ++d);
        }
    }
}

void motor_task() {
    int pin = 21;
    gpio_set_output(pin);

    while (1) {
        gpio_set(pin, 1);
        uart_puts("DC Motor ON\n");
        for (volatile int i = 0; i < 1000000; i++);

        gpio_set(pin, 0);
        uart_puts("DC Motor OFF\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

#include "pwm.h"

void hw_servo_task_hardware_pwm() {
    int gpio = 20;      
    int slice = 2;

    pwm_set_gpio(gpio, slice, 1);
    pwm_configure(slice, 50, 7); 

    while (1) {
        for (int angle = 0; angle <= 180; angle += 30) {
            int duty = 5 + (angle * 5) / 180;
            pwm_set_duty(slice, duty);
            uart_puts("HW Servo moved\n");
            for (volatile int i = 0; i < 1000000; ++i);
        }
    }
}


void hw_motor_task_hardware_pwm() {
    int gpio = 21;
    int slice = 2;

    pwm_set_gpio(gpio, slice, 0);
    pwm_configure(slice, 1000, 0);

    while (1) {
        for (int duty = 0; duty <= 100; duty += 20) {
            pwm_set_duty(slice, duty);
            uart_puts("HW Motor duty = ");
            uart_putc('0' + (duty / 10));
            uart_putc('0' + (duty % 10));
            uart_putc('\n');
            for (volatile int i = 0; i < 1000000; ++i);
        }
    }
}

void irq_handler_io_bank0() {
    if (IO_BANK0_INTR(15)) {
        IO_BANK0_INTR(15) = IO_IRQ_EDGE_LOW;  
        uart_puts("Interrupt: Button Pressed\n");
    }
}

extern void enable_irq();

void lcd_example() {
     i2c_init();          
    lcd_init(0x3C);      
    lcd_clear();
    lcd_write_string("Hello, OS!");
}

void lcd_spi()  {
    lcd_init_spi();      
    lcd_send_command_spi(0xAE);  
    lcd_send_command_spi(0xA1);  
    lcd_send_command_spi(0xAF);  
    lcd_send_data_spi(0x00);     
    lcd_send_data_spi(0xFF);     
    lcd_write_string("Hello, SPI!");
}
/* ---- Raspberry Pi Pico hardware demo ----
 *
 * Two independent tasks blink LEDs at different rates. This deliberately avoids
 * UART (which needs PLL/clk_peri setup, not configured here) and the sensor
 * tasks above (which busy-wait on peripherals that may be absent). The onboard
 * LED on GPIO25 blinking proves boot2, the kernel, and the preemptive
 * round-robin scheduler are all working on real silicon. Wire an optional LED
 * to GPIO16 to see the second task's independent blink rate. */
void blink_onboard_task(void) {
    int led = 25;                 /* Pico onboard LED */
    gpio_set_output(led);
    while (1) {
        gpio_toggle(led);
        for (volatile int i = 0; i < 400000; ++i);
    }
}

void blink_external_task(void) {
    int led = 16;                 /* optional external LED */
    gpio_set_output(led);
    while (1) {
        gpio_toggle(led);
        for (volatile int i = 0; i < 120000; ++i);
    }
}

void kernel_main(void) __attribute__((noreturn, noinline, used, section(".text.boot"), target("thumb")));

void kernel_main(void) {
    scheduler_init();

    task_create(blink_onboard_task, 1, CAP_GPIO);
    task_create(blink_external_task, 2, CAP_GPIO);

    scheduler_start();

    while (1) { }  /* not reached */
}

