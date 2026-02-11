#include "include.h"
#include "i2c_eeprom.h"
#include "i2c_slave.h"
#include "soft_iic.h"
#include "bsp_uart_debug.h"


#define I2C_SOFT_MASTER             0
#define I2C_HARD_SLAVE              1
#define I2C_EEPROM_EXAMPLE          2
#define I2C_TEST_MODE               I2C_EEPROM_EXAMPLE
#define TO_STRING(X)                #X

int main(void)
{
    WDT_DIS();


    bsp_uart_debug_init();

#if (I2C_TEST_MODE == I2C_SOFT_MASTER)
    printf("--->IIC example--->%s\n", TO_STRING(I2C_SOFT_MASTER));
    bsp_iicsw_init();
	bsp_iicsw_master_example();
#elif (I2C_TEST_MODE == I2C_HARD_SLAVE)
    printf("--->IIC example--->%s\n", TO_STRING(I2C_HARD_SLAVE));
    i2c_slave_init();
    i2c_slave_example();
#elif (I2C_TEST_MODE == I2C_EEPROM_EXAMPLE)
    printf("--->IIC EEPROM example\n");
    i2c_eeprom_init();
    while (1) {
        i2c_eeprom_example();
    }
#endif

    return 0;
}
