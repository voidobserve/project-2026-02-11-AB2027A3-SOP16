#ifndef _BSP_RTC_TIME_H
#define _BSP_RTC_TIME_H

typedef struct tm_t_ {
    u16 year;
    u8 mon;
    u8 day;
    u8 weekday;
    u8 hour;
    u8 min;
    u8 sec;
} tm_t;

tm_t time_to_tm(u32 time);
u32 tm_to_time(tm_t tm);
u8 get_weekday(u16 year, u8 mon, u8 day);

#endif // _RTC_TEST_H
