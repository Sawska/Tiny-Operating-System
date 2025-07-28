#include "adc.h"

#define RESETS_BASE      0x4000C000
#define RESETS_RESET     (*(volatile uint32_t*)(RESETS_BASE + 0x0))
#define RESETS_RESET_DONE (*(volatile uint32_t*)(RESETS_BASE + 0x8))

#define ADC_BASE         0x4004C000
#define ADC_CS           (*(volatile uint32_t*)(ADC_BASE + 0x00))
#define ADC_RESULT       (*(volatile uint32_t*)(ADC_BASE + 0x04))

#define RESETS_RESET_ADC     (1 << 30)

#define ADC_CS_EN            (1 << 0)
#define ADC_CS_START_ONCE    (1 << 3)
#define ADC_CS_TS_EN         (1 << 4)
#define ADC_CS_READY         (1 << 8)

#define ADC_CS_AINSEL_LSB    12
#define ADC_CS_AINSEL_MASK   (0x7 << ADC_CS_AINSEL_LSB)

void adc_init(void) {
    RESETS_RESET |= RESETS_RESET_ADC;
    RESETS_RESET &= ~RESETS_RESET_ADC;
    while (!(RESETS_RESET_DONE & RESETS_RESET_ADC));

    ADC_CS = ADC_CS_EN;
}

uint16_t adc_read(uint8_t channel) {
    if (channel > 4) return 0;

    ADC_CS &= ~ADC_CS_AINSEL_MASK;
    ADC_CS |= (channel << ADC_CS_AINSEL_LSB);

    
    ADC_CS |= ADC_CS_START_ONCE;

    
    while (!(ADC_CS & ADC_CS_READY));

    return (uint16_t)(ADC_RESULT & 0xFFF);
}
