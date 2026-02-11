#include "include.h"
#include "driver_gpio.h"

#if WS2812_LIB_EN

#include "ws2812_bsp.h" 
#include "ws2812_my_effect.h"
#include "driver_ledc.h"

static unsigned long tick_ms;

u8 ws2815_byte_reversed(u8 data)
{
	data = ((data & 0xaa) >> 1) | ((data & 0x55) << 1);
	data = ((data & 0xcc) >> 2) | ((data & 0x33) << 2);
	data = (data >> 4) | (data << 4);
    return data;
}

void ws281x_show(unsigned char *pixels_pattern, unsigned short pattern_size)
{
    // print_r(pixels_pattern, pattern_size);

    // 用三路PWM驱动RGB灯
// #if WS2812_NEO_TYPE == NEO_RGB
//     adv_timer_rgbcw_update_rgb(pixels_pattern[0], pixels_pattern[1], pixels_pattern[2]);
// #elif WS2812_NEO_TYPE == NEO_RBG
//     adv_timer_rgbcw_update_rgb(pixels_pattern[0], pixels_pattern[2], pixels_pattern[1]);
// #elif WS2812_NEO_TYPE == NEO_GRB
//     adv_timer_rgbcw_update_rgb(pixels_pattern[1], pixels_pattern[0], pixels_pattern[2]);
// #elif WS2812_NEO_TYPE == NEO_GBR
//     adv_timer_rgbcw_update_rgb(pixels_pattern[1], pixels_pattern[2], pixels_pattern[0]);
// #elif WS2812_NEO_TYPE == NEO_BRG
//     adv_timer_rgbcw_update_rgb(pixels_pattern[2], pixels_pattern[0], pixels_pattern[1]);
// #elif WS2812_NEO_TYPE == NEO_BGR
//     adv_timer_rgbcw_update_rgb(pixels_pattern[2], pixels_pattern[1], pixels_pattern[0]);
// #else
//     #error "plz set current WS2812_NEO_TYPE func api!!!"
// #endif

#if WS2812_JUST_DIRECT_RGB == 0
    //幻彩灯驱动函数
    for (u16 i = 0; i < pattern_size; i++)
        pixels_pattern[i] = ws2815_byte_reversed(pixels_pattern[i]);  // 将数据高低位反转
    ledc_dma_kick((u32)pixels_pattern, pattern_size);
    ledc_kick();
#endif
}

void ws281x_none(void)
{
    
}

// 周期10ms
unsigned long HAL_GetTick(void)
{
    return tick_ms;
}

// 每10ms调用一次
AT(.com_text.timer)
void run_tick_per_10ms(void)
{
    tick_ms += 10;
}

// 每5ms调用一次
AT(.com_text.timer)
void run_tick_per_5ms(void)
{
    tick_ms += 5;
}

// 每1ms调用一次
AT(.com_text.timer)
void run_tick_per_1ms(void)
{
    tick_ms++;
}

// 每nms调用一次
AT(.com_text.timer)
void run_tick_per_nms(u32 nmsec)
{
    tick_ms += nmsec;
}

#if WS2812_JUST_DIRECT_RGB == 0
AT(.com_text.isr)
void ledc_irq_handler(void)
{
    if (ledc_get_flag(LEDC_FLAG_DONE))
    {
        ledc_clear_flag(LEDC_FLAG_DONE);
    }
    
    if (ledc_get_flag(LEDC_FLAG_DMA))
    {
        ledc_clear_flag(LEDC_FLAG_DMA);
        WS2812FX_trigger();
    }
}
#endif

void ws281x_init()
{
#if WS2812_JUST_DIRECT_RGB == 0
    gpio_init_typedef gpio_init_struct;
    ledc_init_typedef ledc_init_struct;
    ledc_timing_init_typedef ledc_timing_init_struct;

    /* Config and enable clock */
    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_EN);
    clk_ledc_clk_set(CLK_LEDC_CLK_X24M_CLKDIV2);

    /* GPIO Init */
    gpio_init_struct.gpio_pin = WS2812_LEDC_PIN;
    gpio_init_struct.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen = GPIO_FEN_PER;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_struct.gpio_drv = GPIO_DRV_6MA;
    gpio_init(WS2812_LEDC_PORT, &gpio_init_struct);

    /* Config GPIO Mapping */
    gpio_func_mapping_config(WS2812_LEDC_PORT, WS2812_LEDC_PIN, GPIO_CROSSBAR_OUT_LEDCDAT);

    /* LEDC Base Ctrl Init */
    ledc_init_struct.output_invert = LEDC_OUT_INV_DIS;      // 电平反转
    ledc_init_struct.format = LEDC_IN_FORMAT_RGB;           // RGB/WRGB
    ledc_init_struct.idle_level = LEDC_IDLE_LEVEL_LOW;      // idle时电平
    ledc_init_struct.valid_data_mode = LEDC_VALID_DATA_DIR; // 有效数据直接/反转输出
    ledc_init_struct.byte_inv_number = LEDC_BYTE_INV_NO;    // 数据反转字节数:不翻转/1B/2B/4B
    ledc_init(&ledc_init_struct);

    /* LEDC Timing Init */
    ledc_timing_init_struct.baud = 15 - 1;      // period = 1 / (clk_12M / 15) = 1.25us
    ledc_timing_init_struct.delay = 0;
    ledc_timing_init_struct.code_0_high = 4;    // 1.25us / 15 * 4 = 333ns
    ledc_timing_init_struct.code_1_high = 11;   // 1.25us / 15 * 11 = 917ns
    ledc_timing_init_struct.reset_high = 20;
    ledc_timing_init_struct.reset_low = 300;
    ledc_timing_init(&ledc_timing_init_struct);

    /* Register interrupt, this interrupt source is not used by the module alone */
    /* Adjust the interrupt processing logic of related modules properly */
    // ledc_pic_config(ledc_irq_handler, 0, LEDC_FLAG_DONE, ENABLE);
    ledc_pic_config(ledc_irq_handler, 0, LEDC_FLAG_DMA, ENABLE);

    /* Initialize procedure parameters */
    ledc_set_frame_length(WS2812_LED_NUM_MAX);
    ledc_set_loop_number(0);

    /* Startup module, make sure to enable the module before starting DMA */
    ledc_cmd(ENABLE);
#endif

}
#endif
