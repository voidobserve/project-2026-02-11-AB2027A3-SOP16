#include "include.h"
#include "driver_lowpwr.h"

#include "driver_gpio.h" // 测试时使用

int main(void)
{
#if 1
    sys_cb.rst_reason = sys_rst_init(WK0_10S_RESET);
    printf("Hello AB202X: %08x\n", sys_cb.rst_reason);
    sys_rst_dump(sys_cb.rst_reason);

    sys_cb.wakeup_reason  = lowpwr_get_wakeup_source();

    printf("SW_VERSION: %s\n",SW_VERSION);

    sys_ram_info_dump();

    bsp_sys_init();

    prod_test_init();

    func_run();
#endif


    // gpio_init_typedef gpio_init_structure;

    // gpio_init_structure.gpio_pin = GPIO_PIN_3;
    // gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    // gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    // gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    // gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    // gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    // gpio_init(GPIOB_REG, &gpio_init_structure);

    // // gpio_set_bits(GPIOA_REG, GPIO_PIN_0);
    // gpio_reset_bits(GPIOB_REG, GPIO_PIN_3);

    // while (1)
    // {
    //     WDT_CLR();
    //     gpio_toggle_bits(GPIOB_REG, GPIO_PIN_3);
    //     delay_ms(500);
    // }

    return 0;
}
