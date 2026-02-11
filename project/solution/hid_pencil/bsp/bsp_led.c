#include "include.h"
#include "driver_gpio.h"
#include "bsp_led.h"

#if BSP_LED_EN

void bsp_led_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init(GPIOA_REG, &gpio_init_structure);
}

void bsp_led_process(void)
{
    static u32 delay_cnt;
    u8 battery_level;

    if (tick_check_expire(delay_cnt, 500)) {
        battery_level = bsp_get_vbat_level(0);
        if (battery_level > 70) {
            gpio_set_bits(GPIOA_REG, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
        } else if (battery_level > 35) {
            gpio_set_bits(GPIOA_REG, GPIO_PIN_2 | GPIO_PIN_3);
            gpio_reset_bits(GPIOA_REG, GPIO_PIN_4);
        } else {
            gpio_set_bits(GPIOA_REG, GPIO_PIN_2);
            gpio_reset_bits(GPIOA_REG, GPIO_PIN_3 | GPIO_PIN_4);
        }
    }
}

#endif
