#include "include.h"
#include "sdadc.h"
#include "sddac.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("bsp sdadc example\n");
    sdadc_example_init();
    sddac_example_init();

    while (1) {
        sdadc_example_process();
    }

    return 0;
}
