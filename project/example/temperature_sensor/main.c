#include "include.h"
#include "bsp_uart_debug.h"
#include "t_sen.h"


/* 
  BIT[7:1]: Reserve.
  BIT[0]:   Buck enable bit. it's buck mode when it's 1, or it will be ldo mode.
 */
#define PMU_TYPE        0x01    // 0: LDO
                                // 1: BUCK


int main(void)
{
    WDT_DIS();
    bsp_uart_debug_init();

    printf("--->temperature example\n");
    /* Initialize PMU, then you will get a more accurate adc sampling value */
    pmu_init(PMU_TYPE);
    temperature_sensor_init();

    while (1) {
        temperature_sensor_example();
    }

    return 0;
}
