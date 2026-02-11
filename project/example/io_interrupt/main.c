#include "include.h"
#include "io_interrupt.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("--->io interrupt example\n");

    io_interrupt_init();

    return 0;
}
