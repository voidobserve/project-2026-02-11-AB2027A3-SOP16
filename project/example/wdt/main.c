#include "include.h"
#include "wdt.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();
    printf("--->wdt example\n");
    wdt_run();

    return 0;
}
