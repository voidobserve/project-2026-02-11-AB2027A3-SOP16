#ifndef __COLORFUL_LIGHT_CTL_H__
#define __COLORFUL_LIGHT_CTL_H__

#include "include.h"
#include "user_config.h"
#include "driver_gpio.h"

#define COLORFUL_LIGHT_POWER_CTL_PORT GPIOB_REG // 幻彩灯的电源控制引脚所在的端口
#define COLORFUL_LIGHT_POWER_CTL_PIN GPIO_PIN_9 // 幻彩灯的电源控制引脚
#define COLORFUL_LIGHT_POWER_CTL_PIN_SET() \
    gpio_set_bits(COLORFUL_LIGHT_POWER_CTL_PORT, COLORFUL_LIGHT_POWER_CTL_PIN)
#define COLORFUL_LIGHT_POWER_CTL_PIN_RESET() \
    gpio_reset_bits(COLORFUL_LIGHT_POWER_CTL_PORT, COLORFUL_LIGHT_POWER_CTL_PIN)

typedef struct
{
    u8 left_light_enable;  // 左边幻彩灯使能标志
    u8 right_light_enable; // 右边幻彩灯使能标志

} colorful_light_ctl_t;
extern volatile colorful_light_ctl_t colorful_light_ctl;

void colorful_light_power_ctl_io_init(void);
void colorful_light_set_static_color(u32 color);

#endif