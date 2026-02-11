#include "include.h"
#include "spi1_flash.h"
#include "spi1_master.h"
#include "spi1_slave.h"
#include "bsp_uart_debug.h"

#define SPI1_2WIRE_TEST_MASTER      0
#define SPI1_2WIRE_TEST_SLAVE       1
#define SPI1_3WIRE_TEST_MASTER      2
#define SPI1_TEST_MODE              SPI1_2WIRE_TEST_SLAVE

#define TO_STRING(X)                #X

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

    #if (SPI1_TEST_MODE == SPI1_3WIRE_TEST_MASTER)
    printf("--->spi flash example--->%s\n", TO_STRING(SPI1_3WIRE_TEST_MASTER));
    W25Qxx_init(500000);      // SPI clk select 24M speed.
    W25Qxx_example();
    #elif (SPI1_TEST_MODE == SPI1_2WIRE_TEST_MASTER)
    printf("--->spi flash example--->%s\n", TO_STRING(SPI1_2WIRE_TEST_MASTER));
    spi1_master_init(500000);
    spi1_master_example();
    #elif (SPI1_TEST_MODE == SPI1_2WIRE_TEST_SLAVE)
    printf("--->spi flash example--->%s\n", TO_STRING(SPI1_2WIRE_TEST_SLAVE));
    spi1_slave_init(500000);
    spi1_slave_example();
    #endif
    
    return 0;
}
