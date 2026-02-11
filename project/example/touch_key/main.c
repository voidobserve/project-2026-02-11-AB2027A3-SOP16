#include "include.h"
#include "touch_key_example.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();
    
    printf("Hello AB202X, This Is Touch Key Example\n");

    touch_key_init_example();

    while (1) {
        touch_key_scan_example();
    }

    return 0;
}
