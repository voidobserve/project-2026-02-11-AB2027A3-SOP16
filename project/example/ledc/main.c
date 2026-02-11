#include "include.h"
#include "WS2815.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    bsp_uart_debug_init();

    printf("--->ledc ws2815 example\n");

    ws2815_example_init();
    ws2815_example_loop();

    return 0;
}
