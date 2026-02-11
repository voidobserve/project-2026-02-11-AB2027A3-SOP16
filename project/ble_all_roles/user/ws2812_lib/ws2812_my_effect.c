#include "include.h"

#if WS2812_LIB_EN

#include "ws2812fx.H"
#include "ws2812_Adafruit_NeoPixel.H"
#include "ws2812fx_effect.h"

// WS2812FX_getSpeed_seg(n)
// WS2812FX_setSpeed_seg(n, speed)

extern Segment_runtime *WS2812FX_getSegmentRuntime_seg(uint8_t seg);

static uint32_t uws2812_colors[MAX_NUM_COLORS];
static uint8_t uws2812_speed = (UWS_SPEED_MAX - DEFAULT_SPEED) * 100 / (UWS_SPEED_MAX - UWS_SPEED_MIN);


/**
 * @brief 设置上电默认样式
*/
void uws2812_style_powerup_default(void)
{
    // uws2812_style_set_color_index(0, RED);
    // uws2812_style_static_segment0();

    uint32_t colors[MAX_NUM_COLORS];
    memset(colors, 0x00, sizeof(colors));
    colors[0] = RED;
    // colors[0] = GREEN;
    // colors[0] = BLUE;


    WS2812FX_set_coloQty(0, 1);
    WS2812FX_setSegment_colorsOptions(
        0,                          // 第0段
        0,                          // 起始位置
        WS2812_LED_NUM_MAX - 1,     // 结束位置
        &WS2812FX_mode_static,      // 效果
        colors,                     // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(0),   // 速度
        SIZE_SMALL                  // 选项，这里像素点大小：1
    );

    // uint32_t colors[MAX_NUM_COLORS];
    // memset(colors, 0x00, sizeof(colors));
    // colors[0] = YELLOW;
    // colors[1] = BLACK;
    // WS2812FX_set_coloQty(0, 2);
    // WS2812FX_setSegment_colorsOptions(
    //     0,                                  // 第0段
    //     0,                                  // 起始位置
    //     WS2812_LED_NUM_MAX - 1,             // 结束位置
    //     &WS2812FX_mode_music_move_down,     // 效果
    //     colors,                             // 颜色，WS2812FX_setColors设置
    //     WS2812FX_getSpeed_seg(0),           // 速度
    //     SIZE_SMALL                          // 选项，这里像素点大小：1
    // );
}


/**
 * @brief 清空并重新填充uws2812_colors[]
 * @param colors 颜色list地址
 * @param number 颜色数量
*/
void uws2812_style_reset_colors(u32 *colors, u8 number)
{
    if (number > MAX_NUM_COLORS) number = MAX_NUM_COLORS;

    memset(uws2812_colors, 0x00, sizeof(uws2812_colors));
    memcpy(uws2812_colors, colors, number * sizeof(uws2812_colors[0]));
}

/**
 * @brief 清空uws2812_colors[]
*/
void uws2812_style_clear_colors(void)
{
    memset(uws2812_colors, 0x00, sizeof(uws2812_colors));
}

/**
 * @brief 填充uws2812_colors[index] = color
 * @param index
 * @param color
*/
void uws2812_style_set_color_index(u8 index, u32 color)
{
    if (index >= MAX_NUM_COLORS) return;
    uws2812_colors[index] = color;
}

/**
 * @brief 设置速度值
 * @param speed [0:100]
*/
void uws2812_set_speed(u8 speed)
{
    if (speed > 100) speed = 100;
    uws2812_speed = speed;

    speed = UWS_SPEED_MAX - speed * 5;
    if (speed < UWS_SPEED_MIN) speed = UWS_SPEED_MIN;
    WS2812FX_setSpeed_seg(0, speed);
}

/**
 * @brief 获取速度值
 * @return [0:100]
*/
uint8_t uws2812_get_speed(void)
{
    return uws2812_speed;
}


/**
 * @brief color1->color0->color1 <delay> [repeat]
 * @param segment 段序号
 * @param start 起始灯序号[0:max-1]
 * @param end 结尾灯序号[0:max-1]
*/
void uws2812_style_breath_options(u8 segment, u16 start, u16 end)
{
    WS2812FX_set_coloQty(segment, 2);
    WS2812FX_setSegment_colorsOptions(
        segment,                        // 第n段
        start,                          // 起始位置
        end,                            // 结束位置
        &WS2812FX_mode_breath_speed,    // 效果
        uws2812_colors,                 // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(segment), // 速度
        SIZE_SMALL                      // 选项，这里像素点大小：1
    );
    WS2812FX_start();
}

/**
 * @brief color1->color0->color1 <delay> [repeat]
 * @param segment 段序号
*/
void uws2812_style_breath_segment(u8 segment)
{
    uws2812_style_breath_options(segment, 0, WS2812_LED_NUM_MAX - 1);
}

/**
 * @brief color1->color0->color1 <delay> [repeat]
*/
void uws2812_style_breath_segment0(void)
{
    uws2812_style_breath_options(0, 0, WS2812_LED_NUM_MAX - 1);
}


/**
 * @brief color [repeat]
 * @param segment 段序号
 * @param start 起始灯序号[0:max-1]
 * @param end 结尾灯序号[0:max-1]
*/
void uws2812_style_static_options(u8 segment, u16 start, u16 end)
{
    WS2812FX_set_coloQty(segment, 1);
    WS2812FX_setSegment_colorsOptions(
        segment,                        // 第n段
        start,                          // 起始位置
        end,                            // 结束位置
        &WS2812FX_mode_static,          // 效果
        uws2812_colors,                 // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(segment), // 速度
        SIZE_SMALL                      // 选项，这里像素点大小：1
    );
    WS2812FX_start();
}

/**
 * @brief color [repeat]
 * @param segment 段序号
*/
void uws2812_style_static_segment(u8 segment)
{
    uws2812_style_static_options(segment, 0, WS2812_LED_NUM_MAX - 1);
}

/**
 * @brief color [repeat]
*/
void uws2812_style_static_segment0(void)
{
    uws2812_style_static_options(0, 0, WS2812_LED_NUM_MAX - 1);
}


/**
 * @brief color[n]->black [repeat n++ n=[0:num-1]]
 * @param segment 段序号
 * @param start 起始灯序号[0:max-1]
 * @param end 结尾灯序号[0:max-1]
*/
void uws2812_style_strobe_options(u8 segment, u16 start, u16 end)
{
    WS2812FX_set_coloQty(segment, 1);
    WS2812FX_setSegment_colorsOptions(
        segment,                        // 第n段
        start,                          // 起始位置
        end,                            // 结束位置
        &WS2812FX_mutil_strobe,         // 效果
        uws2812_colors,                 // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(segment), // 速度
        SIZE_SMALL                      // 选项，这里像素点大小：1
    );
    WS2812FX_start();
}

/**
 * @brief color[n]->black [repeat n++ n=[0:num-1]]
 * @param segment 段序号
*/
void uws2812_style_strobe_segment(u8 segment)
{
    uws2812_style_strobe_options(segment, 0, WS2812_LED_NUM_MAX - 1);
}

/**
 * @brief color[n]->black [repeat n++ n=[0:num-1]]
*/
void uws2812_style_strobe_segment0(void)
{
    uws2812_style_strobe_options(0, 0, WS2812_LED_NUM_MAX - 1);
}


/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param segment 段序号
 * @param start 起始灯序号[0:max-1]
 * @param end 结尾灯序号[0:max-1]
 * @param color_num 颜色数量
*/
void uws2812_style_fade_options(u8 segment, u16 start, u16 end, u8 color_num)
{
    WS2812FX_set_coloQty(segment, color_num);
    WS2812FX_setSegment_colorsOptions(
        segment,                        // 第n段
        start,                          // 起始位置
        end,                            // 结束位置
        &WS2812FX_mode_mutli_fade,      // 效果
        uws2812_colors,                 // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(segment), // 速度
        SIZE_SMALL                      // 选项，这里像素点大小：1
    );
    WS2812FX_start();
}

/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param segment 段序号
 * @param color_num 颜色数量
*/
void uws2812_style_fade_segment(u8 segment, u8 color_num)
{
    uws2812_style_fade_options(segment, 0, WS2812_LED_NUM_MAX - 1, color_num);
}

/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param color_num 颜色数量
*/
void uws2812_style_fade_segment0(u8 color_num)
{
    uws2812_style_fade_options(0, 0, WS2812_LED_NUM_MAX - 1, color_num);
}


/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param segment 段序号
 * @param start 起始灯序号[0:max-1]
 * @param end 结尾灯序号[0:max-1]
 * @param color_num 颜色数量
*/
void uws2812_style_jump_options(u8 segment, u16 start, u16 end, u8 color_num)
{
    WS2812FX_set_coloQty(segment, color_num);
    WS2812FX_setSegment_colorsOptions(
        segment,                        // 第n段
        start,                          // 起始位置
        end,                            // 结束位置
        &WS2812FX_mutil_c_jump,         // 效果
        uws2812_colors,                 // 颜色，WS2812FX_setColors设置
        WS2812FX_getSpeed_seg(segment), // 速度
        SIZE_SMALL                      // 选项，这里像素点大小：1
    );
    WS2812FX_start();
}

/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param segment 段序号
 * @param color_num 颜色数量
*/
void uws2812_style_jump_segment(u8 segment, u8 color_num)
{
    uws2812_style_jump_options(segment, 0, WS2812_LED_NUM_MAX - 1, color_num);
}

/**
 * @brief color[n] [repeat n++ n=[0:num-1]]
 * @param color_num 颜色数量
*/
void uws2812_style_jump_segment0(u8 color_num)
{
    uws2812_style_jump_options(0, 0, WS2812_LED_NUM_MAX - 1, color_num);
}

#endif
