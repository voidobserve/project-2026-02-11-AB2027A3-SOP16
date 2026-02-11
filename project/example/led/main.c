#include "include.h"
#include "driver_gpio.h"

int main(void)
{
    WDT_DIS();

    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_set_bits(GPIOA_REG, GPIO_PIN_0);
    gpio_reset_bits(GPIOA_REG, GPIO_PIN_1);

    while(1){
        gpio_toggle_bits(GPIOA_REG, GPIO_PIN_0 | GPIO_PIN_1);
        delay_ms(500);
    }

    return 0;
}
