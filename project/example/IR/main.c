#include "include.h"
#include "ir_example.h"
#include "driver_ir.h"
#include "bsp_uart_debug.h"
#include "sw_irtx.h"

#define IRRX_EXAMPLE                0
#define IRTX_EXAMPLE                1
#define IRTX_TIMER                  2
#define IR_EXAMPLE_SEL              IRTX_EXAMPLE

#if (IR_EXAMPLE_SEL == IRRX_EXAMPLE)
#define IRRX_EXAMPLE_DECODE         1
#define IRRX_EXAMPLE_WAKEUP         2
#define IRRX_EXAMPLE_SEL            IRRX_EXAMPLE_DECODE

#elif (IR_EXAMPLE_SEL == IRTX_EXAMPLE||IR_EXAMPLE_SEL == IRTX_TIMER)
#define IRTX_EXAMPLE_SEND           0
#define IRTX_EXAMPLE_LEARN          1
#define IRTX_EXAMPLE_D7C6           2
#define IRTX_EXAMPLE_TIMER          3
#define IRTX_EXAMPLE_SEL            IRTX_EXAMPLE_SEND
#endif


int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    printf("Hello AB202X, This is IR Example\n");

    #if (IR_EXAMPLE_SEL == IRRX_EXAMPLE)
    irrx_example_init(IRRX_EXAMPLE_SEL);
    #elif (IR_EXAMPLE_SEL == IRTX_EXAMPLE)
    irtx_example_init(IRTX_EXAMPLE_SEL);
    #elif (IR_EXAMPLE_SEL == IRTX_TIMER)
    sw_ir_tx_data_init();
    #endif

    while (1) {
    #if (IR_EXAMPLE_SEL == IRRX_EXAMPLE)
        irrx_handler_example();
    #elif (IR_EXAMPLE_SEL == IRTX_EXAMPLE||IR_EXAMPLE_SEL == IRTX_TIMER)

        #if (IRTX_EXAMPLE_SEL == IRTX_EXAMPLE_SEND)
        irtx_send_handler_example();
        #elif (IRTX_EXAMPLE_SEL == IRTX_EXAMPLE_LEARN)
        irtx_learn_handler_example();
        #elif (IRTX_EXAMPLE_SEL == IRTX_EXAMPLE_D7C6)
        irtx_extension_d7c6_handler_example();
        #elif (IRTX_EXAMPLE_SEL == IRTX_EXAMPLE_TIMER)
        sw_ir_tx_data_example();
        #endif

    #endif
    }

    return 0;
}
