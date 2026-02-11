#ifndef _BSP_RGBCW_HALFPOWER_H_
#define _BSP_RGBCW_HALFPOWER_H_

#include "driver_adv_timer.h"
#include "driver_gpio.h"

void adv_timer_rgbcw_init(void);
void adv_timer_rgbcw_set_duty(u8 index, u16 duty);
void adv_timer_rgbcw_process(void);
void adv_timer_rgbcw_update_rgb(u8 r, u8 g, u8 b);

#endif
