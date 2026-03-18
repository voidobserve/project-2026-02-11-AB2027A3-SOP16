#include "colorful_light_ctl.h"

volatile colorful_light_ctl_t colorful_light_ctl;

void colorful_light_power_ctl_io_init(void)
{
    // 幻彩灯电源控制，0：关闭，1：打开
    // 幻彩灯的电源控制引脚，控制左侧的灯
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = COLORFUL_LIGHT_LEFT_POWER_CTL_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(COLORFUL_LIGHT_LEFT_POWER_CTL_PORT, &gpio_init_structure);
    gpio_reset_bits(COLORFUL_LIGHT_LEFT_POWER_CTL_PORT, COLORFUL_LIGHT_LEFT_POWER_CTL_PIN);

    // 幻彩灯的电源控制引脚，控制右侧的灯
    gpio_init_structure.gpio_pin = COLORFUL_LIGHT_RIGHT_POWER_CTL_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(COLORFUL_LIGHT_RIGHT_POWER_CTL_PORT, &gpio_init_structure);
    gpio_reset_bits(COLORFUL_LIGHT_RIGHT_POWER_CTL_PORT, COLORFUL_LIGHT_RIGHT_POWER_CTL_PIN);
}

void colorful_light_set_static_color(u32 color)
{
    uint32_t colors[MAX_NUM_COLORS] = {0};
    // memset(colors, 0x00, sizeof(colors));
    colors[0] = color;

    WS2812FX_set_coloQty(0, 1);
    WS2812FX_setSegment_colorsOptions(
        0,                        // 第0段
        0,                        // 起始位置
        WS2812_LED_NUM_MAX - 1,   // 结束位置
        &WS2812FX_mode_static,    // 效果
        colors,                   // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(0), // 速度
        SIZE_SMALL                // 选项，这里像素点大小：1
    );
    WS2812FX_trigger();
    WS2812FX_start();
}