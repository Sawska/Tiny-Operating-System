#include "slip.h"
#include "uart.h"

#define SLIP_END     0xC0
#define SLIP_ESC     0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD

#define SLIP_BUF_SIZE 256
static uint8_t slip_buf[SLIP_BUF_SIZE];
static uint16_t slip_index = 0;

void slip_send(const uint8_t* data, uint16_t len) {
    uart_putc(SLIP_END); // Start packet

    for (uint16_t i = 0; i < len; ++i) {
        switch (data[i]) {
            case SLIP_END:
                uart_putc(SLIP_ESC);
                uart_putc(SLIP_ESC_END);
                break;
            case SLIP_ESC:
                uart_putc(SLIP_ESC);
                uart_putc(SLIP_ESC_ESC);
                break;
            default:
                uart_putc(data[i]);
                break;
        }
    }

    uart_putc(SLIP_END); 
}

void slip_poll(void) {
    while (uart_available()) {
        uint8_t byte = uart_getc();

        if (byte == SLIP_END) {
            if (byte == SLIP_END) {
    if (slip_index > 0) {
        ip_input(slip_buf, slip_index); 
        slip_index = 0;
    }
}

        } else if (byte == SLIP_ESC) {
            
            while (!uart_available());
            uint8_t next = uart_getc();
            if (next == SLIP_ESC_END) byte = SLIP_END;
            else if (next == SLIP_ESC_ESC) byte = SLIP_ESC;
        }

        if (slip_index < SLIP_BUF_SIZE)
            slip_buf[slip_index++] = byte;
    }
}
