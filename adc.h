#ifndef TINY_OS_ADC_H
#define TINY_OS_ADC_H

#include <stdint.h>

void adc_init(void);
uint16_t adc_read(uint8_t channel);

#endif // TINY_OS_ADC_H
