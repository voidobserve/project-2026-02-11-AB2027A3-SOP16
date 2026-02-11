#include "include.h"
#include "bsp_rtc_time.h"

//设定初始年份为 2020.1.1 星期三
#define TM_START_YEAR       2020
#define TM_START_WEEKDAY    3

AT(.rodata.rtc_time)
static const u8 tbl_month_days[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

AT(.rodata.rtc_time)
static const u8 tbl_month_delta[] = {
    0, 2, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17
};

//获取某年是否为闰年
AT(.text.rtc_time)
static u8 leap_year_day(u16 year)
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return 1;
    }
    return 0;
}

//获取TM_START_YEAR到某年前有多少个闰年
AT(.text.rtc_time)
static u32 get_leaps_before(u16 year)
{
    u32 days;
    year--;
    days = (year / 4 - year / 100 + year / 400);
    days -= ((TM_START_YEAR-1) / 4 - (TM_START_YEAR-1) / 100 + (TM_START_YEAR-1) / 400);
    return days;
}

//从TM_START_YEAR到指定日期共有多少天
AT(.text.rtc_time)
u32 get_totaldays(u16 year, u8 month, u8 day)
{
    u32 days, m;
    if (year < TM_START_YEAR) {
        return 0;
    }
    if (month == 0 || month > 12) {
        return 0;
    }
    if (day == 0 || day > 31) {
        return 0;
    }
    days = (year - TM_START_YEAR) * 365 + get_leaps_before(year);
    m = month - 1;
    days += m * 29 + tbl_month_delta[m];
    if (month > 2) {
        days += leap_year_day(year);
    }
    days += day - 1;
    return days;
}

//计算星期几
AT(.text.rtc_time)
u8 get_weekday(u16 year, u8 month, u8 day)
{
    u32 days = get_totaldays(year, month, day);
    u8 weekday = (days + TM_START_WEEKDAY) % 7;
    return weekday;
}

//time to tm
AT(.text.rtc_time)
tm_t time_to_tm(u32 time)
{
    tm_t tm;
    int days, y, m;
    int flag_leap_229;

    days = time / 86400;
    time -= days * 86400;

    tm.weekday = (days + TM_START_WEEKDAY) % 7;     //计算星期
    y = days / 365;
    tm.year = TM_START_YEAR + y;
    days -= y * 365 + get_leaps_before(tm.year);
    flag_leap_229 = 0;
    if (days < 0) {
        tm.year--;
        days += 365;                                //一般减下来都是大于2月,刚好抵下闰2月的减1
    } else if (days >= 31 + 29) {
        days -= leap_year_day(tm.year);             //大于2月的闰年减1天
    } else if (days == 31 + 29 - 1) {
        flag_leap_229 = leap_year_day(tm.year);
    }
    if (flag_leap_229 == 0) {
        m = days / 29;
        days -= m * 29 + tbl_month_delta[m];
        if (days < 0) {
            m--;
            days += tbl_month_days[m];
        }
        tm.mon = m + 1;
        tm.day = days + 1;
    } else {
        tm.mon = 2;
        tm.day = 29;
    }
    tm.hour = (u8)(time / 3600);
    time -= tm.hour * 3600;
    tm.min = (u8)(time / 60);
    tm.sec = (u8)(time - tm.min * 60);
    return tm;
}

//mktime, tm_t转time
AT(.text.rtc_time)
u32 tm_to_time(tm_t tm)
{
    u32 days = get_totaldays(tm.year, tm.mon, tm.day);
    u32 times = days * 86400 + tm.hour * 3600 + tm.min * 60 + tm.sec;
    return times;
}
