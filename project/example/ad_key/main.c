#include "include.h"
#include "ad_key.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    bsp_uart_debug_init();

    printf("--->ad_key example\n");
    ad_key_init();

    while (1) {
        ad_key_example();
    }

    return 0;
}
