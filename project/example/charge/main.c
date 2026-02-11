#include "include.h"
#include "charge.h"
#include "bsp_uart_debug.h"

#define PMU_TYPE        1       // 0: LDO
                                // 1: BUCK
#define POWER_TYPE      0       // 0: NORMAL
                                // 1: NO BATTERRY

u8 pmu_type = (POWER_TYPE << 7) | PMU_TYPE;

int main(void)
{
    WDT_DIS();
    bsp_uart_debug_init();

    printf("--->charge example\n");

    pmu_init(pmu_type);

    if (pmu_type & BIT(7)) {
        while (1) {
            printf("system is running.\n");
            delay_ms(1000);
        }
    } else {
        my_charge_init();
        while (1) {
            my_charge_example();
        }
    }

    return 0;
}
