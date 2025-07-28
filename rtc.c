#include "rtc.h"

#define RTC_BASE         0x4005C000

#define RTC_CTRL         (*(volatile uint32_t*)(RTC_BASE + 0x00))
#define RTC_SETUP_0      (*(volatile uint32_t*)(RTC_BASE + 0x04))
#define RTC_SETUP_1      (*(volatile uint32_t*)(RTC_BASE + 0x08))
#define RTC_SETUP_2      (*(volatile uint32_t*)(RTC_BASE + 0x0C))
#define RTC_IRQ          (*(volatile uint32_t*)(RTC_BASE + 0x10))
#define RTC_RTC_0        (*(volatile uint32_t*)(RTC_BASE + 0x14))
#define RTC_RTC_1        (*(volatile uint32_t*)(RTC_BASE + 0x18))
#define RTC_RTC_2        (*(volatile uint32_t*)(RTC_BASE + 0x1C))
#define RTC_RTC_3        (*(volatile uint32_t*)(RTC_BASE + 0x20))

#define RTC_CTRL_ENABLE        (1 << 0)
#define RTC_CTRL_LOAD          (1 << 1)

void rtc_init(void) {
    RTC_CTRL = RTC_CTRL_ENABLE | RTC_CTRL_LOAD;
}

void rtc_set_time(const rtc_time_t* t) {
    uint32_t d1 = ((t->sec  % 60) << 0)  |
                  ((t->min  % 60) << 8)  |
                  ((t->hour % 24) << 16);

    uint32_t d2 = ((t->day   % 32) << 0) |
                  ((t->month % 13) << 8) |
                  (((t->year - 2000) & 0xFF) << 16);

    RTC_SETUP_0 = d1;
    RTC_SETUP_1 = d2;
    RTC_CTRL = RTC_CTRL_ENABLE | RTC_CTRL_LOAD;
}

void rtc_get_time(rtc_time_t* t) {
    uint32_t d1 = RTC_RTC_0;
    uint32_t d2 = RTC_RTC_1;

    t->sec  = (d1 >> 0) & 0xFF;
    t->min  = (d1 >> 8) & 0xFF;
    t->hour = (d1 >> 16) & 0xFF;

    t->day   = (d2 >> 0) & 0xFF;
    t->month = (d2 >> 8) & 0xFF;
    t->year  = 2000 + ((d2 >> 16) & 0xFF);
}
