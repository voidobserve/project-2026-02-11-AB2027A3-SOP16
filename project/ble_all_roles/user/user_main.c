#include "user_config.h"

// 定义幻彩灯效果控制结构体
typedef struct 
{   
    /* data */
} fantastic_color_effect_t; 
 


volatile fantastic_color_effect_t fc_effect;




// 初始化函数，在 bsp_sys.c -> bsp_sys_init() 中调用
void user_init(void)
{
    

#if WS2812_LIB_EN
    WS2812FX_init(WS2812_LED_NUM_MAX, WS2812_NEO_TYPE);
    WS2812FX_setBrightness(255);

    // 设置上电默认样式
    uws2812_style_powerup_default();

#if UWS_POWERUP_DEFAULT == 0
    WS2812FX_show_cover_ptr(ws281x_none);
    // Adafruit_NeoPixel_clear();
    ws281x_show(Adafruit_NeoPixel_getPixels(), Adafruit_NeoPixel_getNumBytes()); // 即时填充发送灯全灭数据
#else
    WS2812FX_trigger();
    WS2812FX_start();
#endif
#endif // WS2812_LIB_EN
}


// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
void user_ws2812_service(void)
{
#if WS2812_LIB_EN
    /*更新ws2812系统时钟计数*/
    static u32 last_tick = 0;
    u32 new_tick = tick_get();
    if (last_tick < new_tick)
    {
        run_tick_per_nms(new_tick - last_tick);
    }
    last_tick = new_tick;

    /*ws2812系统主任务*/
    WS2812FX_service();
#endif
}
