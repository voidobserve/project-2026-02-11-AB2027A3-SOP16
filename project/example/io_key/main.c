#include "include.h"
#include "io_key.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("--->io key example\n");
    io_key_init();

    while (1) {
        io_key_example();
    }

    return 0;
}
