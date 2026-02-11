#include "include.h"
#include "lpwm_led.h"
#include "bsp_uart_debug.h"


int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("--->LPWM led example\n");
    lpwm_led_init();

    while (1) {
        lpwm_led_example();
    }

    return 0;
}
