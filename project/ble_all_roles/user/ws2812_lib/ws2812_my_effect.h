#ifndef my_effect_h
#define my_effect_h

#include "include.h"

#if WS2812_LIB_EN

#include <stdint.h>

void uws2812_style_powerup_default(void);

void uws2812_style_reset_colors(u32 *colors, u8 number);
void uws2812_style_clear_colors(void);
void uws2812_style_set_color_index(u8 index, u32 color);

void uws2812_set_speed(u8 speed);
uint8_t uws2812_get_speed(void);

void uws2812_style_breath_options(u8 segment, u16 start, u16 end);
void uws2812_style_breath_segment(u8 segment);
void uws2812_style_breath_segment0(void);

void uws2812_style_breath_options(u8 segment, u16 start, u16 end);
void uws2812_style_static_segment(u8 segment);
void uws2812_style_static_segment0(void);

void uws2812_style_strobe_options(u8 segment, u16 start, u16 end);
void uws2812_style_strobe_segment(u8 segment);
void uws2812_style_strobe_segment0(void);

void uws2812_style_fade_options(u8 segment, u16 start, u16 end, u8 color_num);
void uws2812_style_fade_segment(u8 segment, u8 color_num);
void uws2812_style_fade_segment0(u8 color_num);

void uws2812_style_jump_options(u8 segment, u16 start, u16 end, u8 color_num);
void uws2812_style_jump_segment(u8 segment, u8 color_num);
void uws2812_style_jump_segment0(u8 color_num);

#endif

#endif

