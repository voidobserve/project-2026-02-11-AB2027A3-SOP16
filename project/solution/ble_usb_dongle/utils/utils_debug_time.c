#include "include.h"
#include "utils_debug_time.h"
#include "driver_rtc.h"


extern u32 __comm_vma, __comm_ram_size;

isr_t utils_debug_time_callback = NULL;

AT(COM_TEXT_UTILS_DEBUG.string)
static char utils_debug_info[] = "callback need in comm area\n";


AT(COM_TEXT_UTILS_DEBUG)
static void utils_debug_time_isr(void)
{
    if (rtc_get_flag(RTC_IT_ALM)) {
        if (((u32)utils_debug_time_callback >= (u32)&__comm_vma) && \
            ((u32)utils_debug_time_callback <= (((u32)&__comm_vma) + ((u32)&__comm_ram_size  ))))
        {
            utils_debug_time_callback();
        } else {
            printf(utils_debug_info);
        }
        rtc_clear_flag(RTC_IT_ALM);
    }
}

// Unit: 32us  Fclk: XOSC_DIV12
AT(COM_TEXT_UTILS_DEBUG)
void utils_debug_time_init(void)
{
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_X24MDIV12_RTC);
    rtc_prescale_set(0);
    rtc_set_counter(0);
}

// Unit: 32us  Fclk: XOSC_DIV12
AT(COM_TEXT_UTILS_DEBUG)
uint32_t utils_debug_time_get(void)
{
    return rtc_get_counter();
}

AT(COM_TEXT_UTILS_DEBUG)
void utils_debug_time_alarm(uint32_t delay, isr_t callback)
{
    utils_debug_time_callback = callback;
    rtc_pic_config(utils_debug_time_isr, 0, RTC_IT_ALM, ENABLE);
    rtc_set_alarm(rtc_get_counter() + delay - 1);  //RTCALM = (real time) - 1.
}
