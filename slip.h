#pragma once
#include <stdint.h>

void slip_send(const uint8_t* data, uint16_t len);
void slip_poll(void);

