#include "include.h"
#include "key_scan.h"
#include "bsp_uart_debug.h"


int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("--->key scan example\n");
    key_scan_init();

    while (1) {
        key_scan_example();
    }

    return 0;
}
