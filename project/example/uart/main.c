#include "include.h"
#include "uart_transfer.h"

int main(void)
{
    WDT_DIS();

#if (UART_UDET_EXAMPLE_SEL == UART_EXAMPLE)
    uart_transfer_init(115200);

    while (1) {
        uart_transfer_example();
    }
#elif (UART_UDET_EXAMPLE_SEL == UDET_EXAMPLE)
    udet_transfer_init(115200);

    while (1) {
        udet_transfer_example();
    }
#elif (UART_UDET_EXAMPLE_SEL == UART_UDET_EXAMPLE)
    uart_transfer_init(115200);
    udet_transfer_init(115200);
    
    while (1) {
        uart_transfer_example();
        udet_transfer_example();
    }
#endif

    return 0;
}
