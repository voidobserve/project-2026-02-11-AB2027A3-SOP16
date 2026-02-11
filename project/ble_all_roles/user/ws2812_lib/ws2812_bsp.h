#ifndef __ws2812_bsp_h
#define __ws2812_bsp_h


// #include "adafruit_typedef.h"
#include "include.h"

#if WS2812_LIB_EN
#include <stdint.h>

void ws281x_init(void);

void ws281x_show(unsigned char *pixels_pattern, uint16_t pattern_size);
void ws281x_none(void);

unsigned long HAL_GetTick(void);

// 每10ms调用一次
void run_tick_per_10ms(void);
void run_tick_per_5ms(void);
void run_tick_per_1ms(void);
void run_tick_per_nms(u32 nmsec);

#endif

#endif
