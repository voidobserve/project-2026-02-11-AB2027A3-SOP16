#include "include.h"
#include "hsuart_transfer.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    sys_clk_set(SYS_60M);   // When selecting clock PLLDIV4, ensure that the sys_clk is SYS_60M or higher.
    bsp_uart_debug_init();  // Ensure uart init after sys_clk set.

    printf("--->hsuart transfer example\n");
    hsuart_transfer_init(115200);

    while (1) {
        hsuart_transfer_example();
    }

    return 0;
}
