#include "include.h"
#include "rtc_calendar.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("--->rtc calendar example\n");
    rtc_calendar_init();

    while (1) {
        rtc_calendar_example();
    }

    return 0;
}
