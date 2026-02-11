#include "sddac.h"
#include "driver_gpio.h"
#include "driver_sddac.h"


extern uint16_t sdadc_buf[];

void sddac_example_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    FUNO_PA2SEL(FO_AUDIO);                                    //Map dac output PA2

    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_EN);

    sddac_init_typedef sddac_config;
    sddac_config.dma_buf = (uint8_t*)sdadc_buf;
    sddac_config.dma_size = 48;
    sddac_config.sample_rate = SDDAC_NORMAL_SPR_16k;
    sddac_init(&sddac_config);

    sddac_cmd(ENABLE);
}
