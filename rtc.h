#pragma once
#include <stdint.h>

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

void rtc_init(void);
void rtc_set_time(const rtc_time_t* time);
void rtc_get_time(rtc_time_t* time);
