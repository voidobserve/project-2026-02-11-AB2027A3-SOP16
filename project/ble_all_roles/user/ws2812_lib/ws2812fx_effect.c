#include "include.h"

#if WS2812_LIB_EN

// 利用ws2812fx库，创建效果
#include "ws2812fx.h"
#include "ws2812fx_tool.h"
#include "ws2812_Adafruit_NeoPixel.h"
// #include "led_strand_effect.h"
// #include "system/includes.h"

extern  Segment *_seg;
extern  uint16_t _seg_len;
extern Segment_runtime *_seg_rt;

/* ****************项目专属效果******************************* */

#ifdef fc_effect
// 调整RGB顺序效果，以红绿蓝跳变
uint16_t WS2812FX_adj_rgb_sequence(void)
{
    u32 c = BLUE;
    switch (_seg_rt->counter_mode_step)
    {
    case 0:
        c = RED;
        break;
    case 1:
        c = GREEN;
        break;
    case 2:
        c = BLUE;
        break;
    }

    Adafruit_NeoPixel_fill(c, _seg->start, _seg_len);

    if (_seg_rt->counter_mode_step < 3)
    {
        _seg_rt->counter_mode_step++;
    }
    return 1000;
}

// 单色灯,做流星效果
uint16_t WS2812FX_mode_comet_1(void)
{
    extern u8 get_effect_p(void);
    // 计时中，模式循环完成

    // printf("\n mode_cycle=%d",fc_effect.mode_cycle);
    // printf("\n fc_effect.period_cnt=%d",fc_effect.period_cnt);
    if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1))
    {
        return (_seg->speed);
    }
    WS2812FX_fade_out();
    extern u8 get_custom_index(void);
    u8 offset;
    // if(get_custom_index() < 5)
    // {
    //   offset = get_custom_index()*3+2;
    // }
    // else
    // {
    //   offset = (get_custom_index() - 4)*3;
    // }
    offset = 13;
    if (IS_REVERSE)
    {
        if (_seg_rt->aux_param == 0)
        {
            // _seg_rt->counter_mode_step = _seg->stop;
            _seg_rt->aux_param = 1;
        }
        if ((_seg->stop - _seg->start) >= _seg_rt->counter_mode_step)
        {
            WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
            // printf("\n _seg_rt->counter_mode_step=%d",_seg_rt->counter_mode_step);
        }
    }
    else
    {
        if (_seg_rt->counter_mode_step < _seg->stop + 1)
            WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);

    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1;
        // printf("\n fc_effect.mode_cycle=%d",fc_effect.mode_cycle);
    }

    return (_seg->speed);
}

// 两段流星灯，向中心靠拢
uint16_t WS2812FX_mode_comet_2(void)
{
    extern u8 get_effect_p(void);  //当计数器大于0时，返回1
    // 计时中，模式循环完成
    // printf("\n fc_effect.mode_cycle=%d",fc_effect.mode_cycle);
    // printf("\n fc_effect.period_cnt=%d",fc_effect.period_cnt);

    if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1))  //计时ms,运行时的计数器    //1:模式完成一个循环。0：正在跑，和meteor_period搭配用
    {
        return (_seg->speed); //步数，进度
    }
    WS2812FX_fade_out();   //颜色弹出，类似渐变，效果工具
    extern u8 get_custom_index(void);
    u8 offset;
    offset = 6;
    if (_seg_rt->counter_mode_step < 5)
        WS2812FX_setPixelColor(_seg->start + 5 + _seg_rt->counter_mode_step, _seg->colors[0]); //哪灯珠，填充颜色      _seg_rt->counter_mode_step 从0开始

    if (_seg_rt->counter_mode_step < 5)
        WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);   //_seg->start 是从1开始，表示从段的
    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);  //_seg_len 段的总长度 这条语句控制

    //判断是否完成一段的循环
    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1;  //完成一个循环
    }
    return (_seg->speed);  //返回计数器结果
}


uint16_t WS2812FX_mode_comet_3(void)
{
    extern u8 get_effect_p(void);
    // 计时中，模式循环完成

    if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1))
    {
        return (_seg->speed);
    }
    WS2812FX_fade_out();
    extern u8 get_custom_index(void);
    u8 offset;

    offset = 7;

    if (_seg_rt->counter_mode_step < 6)
    {
        WS2812FX_setPixelColor(11 - _seg_rt->counter_mode_step, _seg->colors[0]);
        // printf("\n _seg_rt->counter_mode_step=%d",_seg_rt->counter_mode_step);
    }

    if (_seg_rt->counter_mode_step < 6)
        WS2812FX_setPixelColor(5 - _seg_rt->counter_mode_step, _seg->colors[0]);

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
    if (_seg_rt->counter_mode_step == 0)
    {
        SET_CYCLE;
        fc_effect.mode_cycle = 1;
        // printf("\n fc_effect.mode_cycle=%d",fc_effect.mode_cycle);
    }

    return (_seg->speed);
}







// #include "led_strip_drive.h"
uint16_t music_mode1(void)
{
    static u8 trg_cnt = 0;
    static u8 no_trg_cnt = 0;
    extern u8 get_sound_result(void);
    // const u8 rate[12] = {0,1,1,2,2,3,3,4,5,5,6,6};
    const u8 rate[12] = { 253,250,240,230,220,200,180,130,100,75,50,0 };

    if (get_sound_result())
    {
        uint32_t color = _seg->colors[0];
        int w1 = (color >> 24) & 0xff;
        int r1 = (color >> 16) & 0xff;
        int g1 = (color >> 8) & 0xff;
        int b1 = color & 0xff;


        WS2812FX_setPixelColor_rgbw(_seg->start + trg_cnt, r1 - rate[trg_cnt], g1 - rate[trg_cnt], b1 - rate[trg_cnt], w1 - rate[trg_cnt]);

        // WS2812FX_setPixelColor(_seg->start+trg_cnt , _seg->colors[0] );
        if (trg_cnt < _seg_len)
        {
            trg_cnt++;
            no_trg_cnt = 0;
        }
        else
        {
            // trg_cnt = 0;
        }
    }
    else
    {
        WS2812FX_setPixelColor(_seg->start + trg_cnt, BLACK);
        no_trg_cnt++;
        if (no_trg_cnt >= 3)
        {
            no_trg_cnt = 0;
            if (trg_cnt > 0)
            {
                trg_cnt--;
            }
        }
    }
    return 30;
}


#define MAX_RATE 8
// 流星发射，声音触发，不支持连续发射，等上个流星发射完成再发射第二个
uint16_t meteor(void)
{

    static uint8_t i = 0, trg;
    uint32_t r1, g1, b1, w1;
    const uint8_t rate[MAX_RATE] = { 100,88,75,55,30,10,0,0 };
    int w = (_seg->colors[0] >> 24) & 0xff;
    int r = (_seg->colors[0] >> 16) & 0xff;
    int g = (_seg->colors[0] >> 8) & 0xff;
    int b = _seg->colors[0] & 0xff;
    extern u8 get_sound_result(void);
    if (get_sound_result())
    {
        trg = 1;
    }

    if (trg)
    {
        WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1);

        r1 = r * rate[i] / 100;
        g1 = g * rate[i] / 100;
        b1 = b * rate[i] / 100;
        w1 = w * rate[i] / 100;

        WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);
        if (i < MAX_RATE - 1)
            i++;

        _seg_rt->counter_mode_step++;
        if (_seg_rt->counter_mode_step >= _seg_len + 8)
        {
            trg = 0;
            _seg_rt->counter_mode_step = 0;
            i = 0;
        }
    }


    // if(i>0)
    // i--;
    // else
    // i = MAX_RATE-1;

    return (_seg->speed);
}

// 流星发射，声音触发，可以连续发射
uint16_t meteor1(void)
{

    static uint8_t i = 0, trg;
    uint32_t r1, g1, b1, w1;
    const uint8_t rate[MAX_RATE] = { 100,75,50,25,10,0,0,0 };
    int w = (_seg->colors[0] >> 24) & 0xff;
    int r = (_seg->colors[0] >> 16) & 0xff;
    int g = (_seg->colors[0] >> 8) & 0xff;
    int b = _seg->colors[0] & 0xff;
    extern u8 get_sound_result(void);
    if (get_sound_result())
    {
        if (i == MAX_RATE - 1)
        {
            i = 0;
            printf("\n i=%d", i);
        }
    }

    WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1);
    r1 = r * rate[i] / 100;
    g1 = g * rate[i] / 100;
    b1 = b * rate[i] / 100;
    w1 = w * rate[i] / 100;
    WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);



    if (i < MAX_RATE - 1)
        i++;

    // if(i>0)
    // i--;
    // else
    // i = MAX_RATE-1;

    return (30);
}

// 正向移动某一段
// s起始地址
// e结束地址
// 条件：e>s
void WS2812FX_move_forward(u16 s, u16 e)
{
    u16 i;

    if (s > e) return;
    uint32_t c = Adafruit_NeoPixel_getOriginPixelColor(e);

    for (i = 0;i < e - s;i++)
    {
        WS2812FX_copyPixels(s + i + 1, s + i, 1);
    }

    // Adafruit_NeoPixel_setPixelColor_raw(s, c);
}

// 流星发射，声音触发，可以连续发射
uint16_t music_meteor3(void)
{

    static uint8_t i = 0, trg;
    uint32_t r1, g1, b1, w1;
    const uint8_t rate[MAX_RATE] = { 100,75,50,25,10,0,0,0 };
    int w = (_seg->colors[0] >> 24) & 0xff;
    int r = (_seg->colors[0] >> 16) & 0xff;
    int g = (_seg->colors[0] >> 8) & 0xff;
    int b = _seg->colors[0] & 0xff;
    extern u8 get_sound_result(void);
    if (get_sound_result())
    {
        if (i == MAX_RATE - 1)
        {
            i = 0;
        }
    }
    // WS2812FX_move_forward(_seg->start, _seg->stop);
    WS2812FX_copyPixels(_seg->start, _seg->start + 1, _seg_len - 1);
    r1 = r * rate[i] / 100;
    g1 = g * rate[i] / 100;
    b1 = b * rate[i] / 100;
    w1 = w * rate[i] / 100;
    WS2812FX_setPixelColor_rgbw(_seg->stop, r1, g1, b1, w1);



    if (i < MAX_RATE - 1)
        i++;

    // if(i>0)
    // i--;
    // else
    // i = MAX_RATE-1;

    return (30);
}

uint16_t music_mode2(void)
{
    static u8 b;

    Adafruit_NeoPixel_fill(WHITE, _seg->start, _seg_len);
    extern u8 get_sound_result(void);

    if (get_sound_result())
    {
        b = 255;
        WS2812FX_setBrightness(255);
    }
    else
    {
        // if(b>10)
        //   b-=10;
        // else
        b = 0;
        WS2812FX_setBrightness(b);
    }
    return 10;
}
#endif


// 开机效果
uint16_t power_on_effect(void)
{
    if (_seg_rt->counter_mode_step)
    {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
    }
    else
    {
        Adafruit_NeoPixel_fill(GREEN, _seg->start, _seg_len);
    }
    _seg_rt->counter_mode_step = !_seg_rt->counter_mode_step;
    _seg_rt->aux_param++;
    if (_seg_rt->aux_param > 6)
    {
        // extern void read_flash_device_status_init(void);
        // read_flash_device_status_init();
        // set_fc_effect();
    }
    return (500);
}

uint16_t power_off_effect(void)
{
    if (_seg_rt->counter_mode_step)
    {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
    }
    else
    {
        Adafruit_NeoPixel_fill(RED, _seg->start, _seg_len);
    }
    _seg_rt->counter_mode_step = !_seg_rt->counter_mode_step;
    _seg_rt->aux_param++;
    if (_seg_rt->aux_param > 4)
    {
        // 硬件关机
        // gpio_direction_output(IO_PORTA_08, 0);
    }
    return (500);
}

// 解绑效果
uint16_t unbind_effect(void)
{
    if (_seg_rt->counter_mode_step)
    {
        Adafruit_NeoPixel_fill(WHITE, _seg->start, _seg_len);
    }
    else
    {
        Adafruit_NeoPixel_fill(GRAY, _seg->start, _seg_len);
    }
    _seg_rt->counter_mode_step = !_seg_rt->counter_mode_step;

    return (500);
}

/*
 * Color wipe function，多种颜色流水效果
 * LEDs are turned on (color1) in sequence, then turned off (color2) in sequence.
 * if (uint8_t rev == true) then LEDs are turned off in reverse order
 * 颜色擦除功能，用color1依次点亮LED,再用color2依次擦除
 * rev = 0:两个颜色同向， =1color2反方向擦除
 * IS_REVERSE:流水方向
 */
uint16_t WS2812FX_multiColor_wipe(uint8_t is_reverse, uint8_t rev)
{
    static uint32_t color[2];

    if (_seg_rt->counter_mode_step == _seg_len)
    {
        // 一个循环后更换颜色
        color[_seg_rt->aux_param] = _seg->colors[_seg_rt->aux_param3];
        _seg_rt->aux_param++;
        _seg_rt->aux_param %= 2;
        _seg_rt->aux_param3++;
        _seg_rt->aux_param3 %= _seg->c_n;
    }
    if (_seg_rt->counter_mode_step == 0)
    {

        color[_seg_rt->aux_param] = _seg->colors[_seg_rt->aux_param3];
        _seg_rt->aux_param++;
        _seg_rt->aux_param %= 2;
        _seg_rt->aux_param3++;
        _seg_rt->aux_param3 %= _seg->c_n;
        SET_CYCLE;
    }

    if (_seg_rt->counter_mode_step < _seg_len)
    {
        uint32_t led_offset = _seg_rt->counter_mode_step;
        if (is_reverse)
        {
            WS2812FX_setPixelColor(_seg->stop - led_offset, color[0]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + led_offset, color[0]);
        }
    }
    else
    {
        uint32_t led_offset = _seg_rt->counter_mode_step - _seg_len;
        if ((is_reverse && !rev) || (!is_reverse && rev))
        {
            WS2812FX_setPixelColor(_seg->stop - led_offset, color[1]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + led_offset, color[1]);
        }
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len * 2);

    return (_seg->speed);
}


// 多种颜色流水效果
// 正向流水，颜色同向
uint16_t WS2812FX_mode_multi_forward_same(void)
{
    return WS2812FX_multiColor_wipe(0, 0);
}


// 多种颜色流水效果
// 反向流水，颜色同向
uint16_t WS2812FX_mode_multi_back_same(void)
{
    return WS2812FX_multiColor_wipe(1, 0);
}

/*
在每一个LED颜色随机。并且淡出，淡入
Cycle a rainbow on each LED
 */
uint16_t WS2812FX_mode_fade_each_led(void)
{
    if (_seg_rt->counter_mode_step == 0)
    {
        _seg_rt->counter_mode_step = 0x0f;
        for (uint16_t i = _seg->start; i <= _seg->stop; i++)
        {
            WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));

        }
    }
    else
    {
        WS2812FX_fade_out();
    }
    _seg_rt->counter_mode_step++;
    return (_seg->speed / 8);
}



/*
功能：颜色块跳变效果，多个颜色块组成背景,以块为单位步进做流水,
_seg->c_n:有效颜色数量
SIZE_OPTION：决定颜色块大小
IS_REVERSE:0 反向流水 ；1正向流水，WS2812FX_setOptions(REVERSE)来设置
 */
uint16_t WS2812FX_mode_single_block_scan(void)
{
    uint8_t size = (SIZE_OPTION << 1) + 1;
    uint8_t j;
    uint32_t c;

    if (size > (_seg->stop - _seg->start))
        return 0;
    // size = 5; //debug用，最后删除
    // _seg->c_n = 3;//debug用，最后删除
    _seg_rt->counter_mode_step = 0;
    while (_seg_rt->counter_mode_step < _seg->stop)
    {
        for (j = 0; j < size; j++)
        {
            if (IS_REVERSE == 0) //反向流水
            {
                WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
            }
            else
            {
                WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
            }
            _seg_rt->counter_mode_step++;
            if (_seg_rt->counter_mode_step > _seg->stop)
            {
                break;
            }
        }
        _seg_rt->aux_param++;
        _seg_rt->aux_param %= _seg->c_n;
    }

    c = _seg->colors[0];
    // 重新开始，对颜色转盘
    for (j = 1; j < _seg->c_n; j++)
    {
        // 把后面的颜色提前
        _seg->colors[j - 1] = _seg->colors[j];
    }
    _seg->colors[j - 1] = c;

    return _seg->speed * size;
}

/*
多段颜色同时流水效果
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
*/

/*
WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,59,                  //起始位置，结束位置
        &WS2812FX_mode_multi_block_scan,               //效果
        0,                                      //颜色，WS2812FX_setColors设置
        1000,            //速度
        SIZE_MEDIUM | REVERSE);
*/
uint16_t WS2812FX_mode_multi_block_scan(void)
{
    uint8_t size = (SIZE_OPTION << 1) + 1;
    uint8_t j;
    uint16_t i;
    if (size > (_seg->stop - _seg->start))
        return 0;

    /* 构建背景颜色 */
    if (_seg_rt->counter_mode_step == 0)
    {
        while (_seg_rt->counter_mode_step <= _seg->stop)
        {
            for (j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
            {
                WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
                _seg_rt->counter_mode_step++;
            }
            _seg_rt->aux_param++;
            _seg_rt->aux_param %= _seg->c_n;
            // _seg_rt->aux_param %= MAX_NUM_COLORS;
        }
    }
    else
    {
        if (IS_REVERSE) //反向流水
        {
            /* 获取原始颜色，没有进行亮度调整的颜色 */

            uint32_t c = Adafruit_NeoPixel_getOriginPixelColor(_seg->start);
            /* 颜色平移一个像素,把后面像素复制到前面 */
            WS2812FX_copyPixels(_seg->start, _seg->start + 1, _seg->stop - _seg->start);

            /* 把第一个颜色，补到最后一个位置 */
            Adafruit_NeoPixel_setPixelColor_raw(_seg->stop, c);
        }
        else
        {
            uint32_t c = Adafruit_NeoPixel_getOriginPixelColor(_seg->stop);

            for (i = 0;i < _seg->stop - _seg->start;i++)
            {
                WS2812FX_copyPixels(_seg->stop - i, _seg->stop - 1 - i, 1);
            }

            Adafruit_NeoPixel_setPixelColor_raw(_seg->start, c);

        }


    }

    return _seg->speed;
}




/*
 * Fades the LEDs between mutil colors
 */
#if 0
uint16_t WS2812FX_mode_mutil_fade(void)
{
    uint8_t size = 1 << SIZE_OPTION;
    uint16_t j;
    uint32_t color, color1, color0;
    int lum = _seg_rt->counter_mode_step;

    size = 5; //调试用
    _seg->c_n = 3; //调试用
    if (size > (_seg->stop - _seg->start))
        return 0;

    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0
    _seg_rt->aux_param = 0;
    _seg_rt->aux_param2 = 0;
    // if(_seg_rt->aux_param2 == 0)
    {
        while (_seg_rt->aux_param2 < _seg->stop)
        {
            color0 = _seg->colors[_seg_rt->aux_param];
            _seg_rt->aux_param++;
            _seg_rt->aux_param %= _seg->c_n;
            color1 = _seg->colors[_seg_rt->aux_param];
            _seg_rt->aux_param++;
            _seg_rt->aux_param %= _seg->c_n;
            color = WS2812FX_color_blend(color1, color0, lum);
            for (j = 0; j < size; j++)
            {
                WS2812FX_setPixelColor(_seg->start + _seg_rt->aux_param2, \
                    color);
                _seg_rt->aux_param2++;
                if (_seg_rt->aux_param2 > _seg->stop)
                {
                    break;
                }
            }
        }
    }




    // uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    // Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    // 此时颜色停留在color1,把color0颜色变换
    if (_seg_rt->counter_mode_step == 0)
    {

    }

    _seg_rt->counter_mode_step++;

    // 此时颜色停留在color1
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }
    // 此时颜色停留再color0
    if (_seg_rt->counter_mode_step == 256)
    {

    }
    return (_seg->speed / 32);
}


#endif

// 多段颜色，同时在做渐变效果，每段效果把_seg->colors轮转
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量
uint16_t WS2812FX_mode_mutil_fade(void)
{
    uint8_t size = (SIZE_OPTION << 1) + 1;
    uint16_t j;
    uint8_t cnt0 = 0, cnt1 = 1;
    uint32_t color, color1, color0;
    static uint32_t c1[MAX_NUM_COLORS];
    int lum = _seg_rt->counter_mode_step;


    if (size > (_seg->stop - _seg->start) && size == 0)
        return 0;

    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0
    _seg_rt->aux_param = 0;
    _seg_rt->aux_param2 = 0;

    if (_seg_rt->aux_param3 == 0)
    {
        _seg_rt->aux_param3 = 1;
        memcpy(c1, _seg->colors, MAX_NUM_COLORS * 4);

    }

    while (_seg_rt->aux_param2 < _seg->stop)
    {
        color0 = _seg->colors[cnt0];
        cnt0++;
        cnt0 %= _seg->c_n;
        color1 = c1[cnt1];
        cnt1++;
        cnt1 %= _seg->c_n;
        color = WS2812FX_color_blend(color1, color0, lum);
        for (j = 0; j < size; j++)
        {
            WS2812FX_setPixelColor(_seg->start + _seg_rt->aux_param2, \
                color);
            _seg_rt->aux_param2++;
            if (_seg_rt->aux_param2 > _seg->stop)
            {
                break;
            }
        }
    }

    _seg_rt->counter_mode_step += 4;

    // 此时颜色停留在color1
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;
        // color0的颜色池左移1

        SET_CYCLE;
    }

    // 此时颜色停留在color1,把color0颜色变换,color0向左转盘
    if (_seg_rt->counter_mode_step == 0)
    {
        uint32_t c_tmp;
        c_tmp = _seg->colors[0];

        memmove(_seg->colors, _seg->colors + 1, (_seg->c_n - 1) * 4);

        _seg->colors[_seg->c_n - 1] = c_tmp;
    }

    // 此时颜色停留再color0
    if (_seg_rt->counter_mode_step == 256)
    {
        // color1的颜色池左移1
        uint32_t c_tmp;
        c_tmp = c1[0];
        memmove(&c1[0], &c1[1], (_seg->c_n - 1) * 4);


        c1[_seg->c_n - 1] = c_tmp;
    }
    return (_seg->speed / 32);
}



// 多段颜色构成背景色，做呼吸效果
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量

uint16_t WS2812FX_mode_mutil_breath(void)
{
    uint8_t size = (SIZE_OPTION << 1) + 1;
    uint8_t j;
    uint16_t lum = _seg_rt->aux_param3;
    uint32_t color;

    if (lum > 255)
    {
        lum = 511 - lum;
    }

    if (size > (_seg->stop - _seg->start))
        return 0;
    _seg_rt->counter_mode_step = 0;
    _seg_rt->aux_param = 0;
    while (_seg_rt->counter_mode_step <= _seg->stop)
    {
        for (j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
        {
            color = WS2812FX_color_blend(_seg->colors[_seg_rt->aux_param], 0, lum);
            WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, color);
            _seg_rt->counter_mode_step++;
        }
        _seg_rt->aux_param++;
        _seg_rt->aux_param %= _seg->c_n;
    }

    _seg_rt->aux_param3 += 4;
    _seg_rt->aux_param3 %= 511;

    return _seg->speed / 4;
}


// 多段颜色构成背景色，做闪烁
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量

uint16_t WS2812FX_mode_mutil_twihkle(void)
{
    uint8_t size = (SIZE_OPTION << 1) + 1;
    uint8_t j;
    size = 5;
    _seg->c_n = 3;
    if (size > (_seg->stop - _seg->start))
        return 0;

    _seg_rt->counter_mode_step = 0;
    _seg_rt->aux_param = 0;
    if (_seg_rt->aux_param3)
    {
        while (_seg_rt->counter_mode_step <= _seg->stop)
        {
            for (j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
            {
                WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
                _seg_rt->counter_mode_step++;
            }
            _seg_rt->aux_param++;
            _seg_rt->aux_param %= _seg->c_n;
        }
    }
    else
    {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);

    }

    _seg_rt->aux_param3 = !_seg_rt->aux_param3;


    return _seg->speed;
}






/* -------------------------------ws2812fx自带效果----------------------------------- */

/* #define	FX_MODE_STATIC			0		//静态-无闪烁。只是普通的老式静电灯。
#define	FX_MODE_BLINK			1		//眨眼-正常眨眼。50%的开/关时间。
#define	FX_MODE_BREATH			2		//呼吸-进行众所周知的i-设备的“备用呼吸”。固定速度。
#define	FX_MODE_COLOR_WIPE			3		//颜色擦除-在每个LED灯亮起后点亮所有LED灯。然后按顺序关闭它们。重复
#define	FX_MODE_COLOR_WIPE_INV			4		//颜色擦除反转-与颜色擦除相同，只是交换开/关颜色。
#define	FX_MODE_COLOR_WIPE_REV			5		//颜色擦除反转-在每个LED点亮后点亮所有LED。然后按相反的顺序关闭它们。重复
#define	FX_MODE_COLOR_WIPE_REV_INV			6		//颜色擦除反转-与颜色擦除反转相同，除了交换开/关颜色。
#define	FX_MODE_COLOR_WIPE_RANDOM			7		//颜色擦除随机-将所有LED依次切换为随机颜色。然后用另一种颜色重新开始。
#define	FX_MODE_RANDOM_COLOR			8		//随机颜色-以一种随机颜色点亮所有LED。然后将它们切换到下一个随机颜色。
#define	FX_MODE_SINGLE_DYNAMIC			9		//单一动态-以随机颜色点亮每个LED。将一个随机LED逐个更改为随机颜色。
#define	FX_MODE_MULTI_DYNAMIC			10		//多动态-以随机颜色点亮每个LED。将所有LED同时更改为新的随机颜色。
#define	FX_MODE_RAINBOW			11		//彩虹-通过彩虹同时循环所有LED。
#define	FX_MODE_RAINBOW_CYCLE			12		//彩虹循环-在整个LED串上循环彩虹。
#define	FX_MODE_SCAN			13		//扫描-来回运行单个像素。
#define	FX_MODE_DUAL_SCAN			14		//双扫描-在相反方向来回运行两个像素。
#define	FX_MODE_FADE			15		//淡入淡出-使LED灯再次淡入淡出。
#define	FX_MODE_THEATER_CHASE			16		//剧场追逐——剧场风格的爬行灯。灵感来自Adafruit的例子。
#define	FX_MODE_THEATER_CHASE_RAINBOW			17		//剧院追逐彩虹-剧院风格的彩虹效果爬行灯。灵感来自Adafruit的例子。
#define	FX_MODE_RUNNING_LIGHTS			18		//运行灯光-运行灯光效果与平滑正弦过渡。
#define	FX_MODE_TWINKLE			19		//闪烁-闪烁几个LED灯，重置，重复。
#define	FX_MODE_TWINKLE_RANDOM			20		//闪烁随机-以随机颜色闪烁多个LED，打开、重置、重复。
#define	FX_MODE_TWINKLE_FADE			21		//闪烁淡出-闪烁几个LED，淡出。
#define	FX_MODE_TWINKLE_FADE_RANDOM			22		//闪烁淡出随机-以随机颜色闪烁多个LED，然后淡出。
#define	FX_MODE_SPARKLE			23		//闪烁-每次闪烁一个LED。
#define	FX_MODE_FLASH_SPARKLE			24		//闪烁-以选定颜色点亮所有LED。随机闪烁单个白色像素。
#define	FX_MODE_HYPER_SPARKLE			25		//超级火花-像闪光火花。用更多的闪光。
#define	FX_MODE_STROBE			26		//频闪-经典的频闪效果。
#define	FX_MODE_STROBE_RAINBOW			27		//频闪彩虹-经典的频闪效果。骑自行车穿越彩虹。
#define	FX_MODE_MULTI_STROBE			28		//多重选通-具有不同选通计数和暂停的选通效果，由速度设置控制。
#define	FX_MODE_BLINK_RAINBOW			29		//闪烁彩虹-经典的闪烁效果。骑自行车穿越彩虹。
#define	FX_MODE_CHASE_WHITE			30		//追逐白色——白色上的颜色。
#define	FX_MODE_CHASE_COLOR			31		//追逐颜色-白色在颜色上奔跑。
#define	FX_MODE_CHASE_RANDOM			32		//Chase Random-白色跑步，然后是随机颜色。
#define	FX_MODE_CHASE_RAINBOW			33		//追逐彩虹——白色在彩虹上奔跑。
#define	FX_MODE_CHASE_FLASH			34		//Chase Flash（追逐闪光）-在彩色屏幕上运行的白色闪光。
#define	FX_MODE_CHASE_FLASH_RANDOM			35		//Chase Flash Random（随机闪烁）：白色闪烁，然后是随机颜色。
#define	FX_MODE_CHASE_RAINBOW_WHITE			36		//追逐彩虹白色-彩虹在白色上奔跑。
#define	FX_MODE_CHASE_BLACKOUT			37		//Chase Blackout-黑色在颜色上运行。
#define	FX_MODE_CHASE_BLACKOUT_RAINBOW			38		//追逐遮光彩虹-黑色在彩虹上奔跑。
#define	FX_MODE_COLOR_SWEEP_RANDOM			39		//颜色扫描随机-从条带开始和结束交替引入的随机颜色。
#define	FX_MODE_RUNNING_COLOR			40		//运行颜色-交替运行颜色/白色像素。
#define	FX_MODE_RUNNING_RED_BLUE			41		//运行红蓝-交替运行红/蓝像素。
#define	FX_MODE_RUNNING_RANDOM			42		//随机运行-随机彩色像素运行。
#define	FX_MODE_LARSON_SCANNER			43		//拉森扫描仪-K.I.T.T。
#define	FX_MODE_COMET			44		//彗星——从一端发射彗星。
#define	FX_MODE_FIREWORKS			45		//烟花-烟花火花。
#define	FX_MODE_FIREWORKS_RANDOM			46		//烟花随机-随机彩色烟花火花。
#define	FX_MODE_MERRY_CHRISTMAS			47		//圣诞快乐-绿色/红色像素交替运行。
#define	FX_MODE_FIRE_FLICKER			48		//火焰闪烁-火焰闪烁效果。就像在刺骨的风中。
#define	FX_MODE_FIRE_FLICKER_SOFT			49		//火焰闪烁（软）-火焰闪烁效果。跑得更慢/更柔和。
#define	FX_MODE_FIRE_FLICKER_INTENSE			50		//火焰闪烁（强烈）-火焰闪烁效果。更多颜色范围。
#define	FX_MODE_CIRCUS_COMBUSTUS			51		//Circus Combustitus-交替运行白/红/黑像素。
#define	FX_MODE_HALLOWEEN			52		//万圣节-交替运行橙色/紫色像素。
#define	FX_MODE_BICOLOR_CHASE			53		//双色追逐-背景色上运行的两个LED。
#define	FX_MODE_TRICOLOR_CHASE			54		//三色追逐-交替运行三色像素。
#define	FX_MODE_CUSTOM			55  // keep this for backward compatiblity		//闪烁福克斯-灯光随机淡入淡出。
#define	FX_MODE_CUSTOM_0			55  // custom modes need to go at the end		//通过63。自定义-最多八个用户创建的自定义效果。 */
/*
 * Random flickering.
 */
uint16_t WS2812FX_mode_fire_flicker(void)
{
    return WS2812FX_fire_flicker(3);
}

/*
* Random flickering, less intensity.
*/
uint16_t WS2812FX_mode_fire_flicker_soft(void)
{
    return WS2812FX_fire_flicker(6);
}

/*
* Random flickering, more intensity.
*/
uint16_t WS2812FX_mode_fire_flicker_intense(void)
{
    return WS2812FX_fire_flicker(1);
}


/*
 * Random colored firework sparks.
 */
uint16_t WS2812FX_mode_fireworks_random(void)
{
    return WS2812FX_fireworks(WS2812FX_color_wheel(WS2812FX_random8()));
}


/*
 * Firework sparks.
 */
uint16_t WS2812FX_mode_fireworks(void)
{
    uint32_t color = BLACK;
    do
    { // randomly choose a non-BLACK color from the colors array
        color = _seg->colors[WS2812FX_random8_lim(MAX_NUM_COLORS)];
    } while (color == BLACK);
    return WS2812FX_fireworks(color);
}

/*
 * Firing comets from one end.
 */
uint16_t WS2812FX_mode_comet(void)
{
    WS2812FX_fade_out();

    if (IS_REVERSE)
    {
        WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
    }
    else
    {
        WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % _seg_len;
    if (_seg_rt->counter_mode_step == 0) SET_CYCLE;

    return (_seg->speed / _seg_len);
}


/*
 * K.I.T.T.
 */
uint16_t WS2812FX_mode_larson_scanner(void)
{
    WS2812FX_fade_out();

    if (_seg_rt->counter_mode_step < _seg_len)
    {
        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
        }
    }
    else
    {
        uint16_t index = (_seg_len * 2) - _seg_rt->counter_mode_step - 2;
        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->stop - index, _seg->colors[0]);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->start + index, _seg->colors[0]);
        }
    }

    _seg_rt->counter_mode_step++;
    if (_seg_rt->counter_mode_step >= (uint16_t)((_seg_len * 2) - 2))
    {
        _seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }

    return (_seg->speed / (_seg_len * 2));
}


/*
 * Random colored pixels running.
 */
uint16_t WS2812FX_mode_running_random(void)
{
    uint8_t size = 2 << SIZE_OPTION;
    if ((_seg_rt->counter_mode_step) % size == 0)
    {
        _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
    }

    uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);

    return WS2812FX_running(color, color);
}


/*
 * Alternating color/white pixels running.
 */
uint16_t WS2812FX_mode_running_color(void)
{
    return WS2812FX_running(_seg->colors[0], _seg->colors[1]);
}


/*
 * Alternating red/blue pixels running.
 */
uint16_t WS2812FX_mode_running_red_blue(void)
{
    return WS2812FX_running(RED, BLUE);
}


/*
 * Alternating red/green pixels running.
 */
uint16_t WS2812FX_mode_merry_christmas(void)
{
    return WS2812FX_running(RED, GREEN);
}

/*
 * Alternating orange/purple pixels running.
 */
uint16_t WS2812FX_mode_halloween(void)
{
    return WS2812FX_running(PURPLE, ORANGE);
}

/*
 * White flashes running on _color.
 */
uint16_t WS2812FX_mode_chase_flash(void)
{
    return WS2812FX_chase_flash(_seg->colors[0], WHITE);
}


/*
 * White flashes running, followed by random color.
 */
uint16_t WS2812FX_mode_chase_flash_random(void)
{
    return WS2812FX_chase_flash(WS2812FX_color_wheel(_seg_rt->aux_param), WHITE);
}



/*
 * White running on rainbow.
 白色点流水效果，背景为彩虹色，才会说也跟随白色点变换花样
 */
uint16_t WS2812FX_mode_chase_rainbow(void)
{
    uint8_t color_sep = 256 / _seg_len;
    uint8_t color_index = _seg_rt->counter_mode_call & 0xFF;
    uint32_t color = WS2812FX_color_wheel(((_seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

    return WS2812FX_chase(color, WHITE, WHITE);
}


/*
 * Black running on rainbow.
 */
uint16_t WS2812FX_mode_chase_blackout_rainbow(void)
{
    uint8_t color_sep = 256 / _seg_len;
    uint8_t color_index = _seg_rt->counter_mode_call & 0xFF;
    uint32_t color = WS2812FX_color_wheel(((_seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

    return WS2812FX_chase(color, BLACK, BLACK);
}


/*
 * White running followed by random color.
 */
uint16_t WS2812FX_mode_chase_random(void)
{
    if (_seg_rt->counter_mode_step == 0)
    {
        _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
    }
    return WS2812FX_chase(WS2812FX_color_wheel(_seg_rt->aux_param), WHITE, WHITE);
}


/*
 * Rainbow running on white.
 */
uint16_t WS2812FX_mode_chase_rainbow_white(void)
{
    uint16_t n = _seg_rt->counter_mode_step;
    uint16_t m = (_seg_rt->counter_mode_step + 1) % _seg_len;
    uint32_t color2 = WS2812FX_color_wheel(((n * 256 / _seg_len) + (_seg_rt->counter_mode_call & 0xFF)) & 0xFF);
    uint32_t color3 = WS2812FX_color_wheel(((m * 256 / _seg_len) + (_seg_rt->counter_mode_call & 0xFF)) & 0xFF);

    return WS2812FX_chase(WHITE, color2, color3);
}

/*
 * Bicolor chase mode
 */
uint16_t WS2812FX_mode_bicolor_chase(void)
{
    return WS2812FX_chase(_seg->colors[0], _seg->colors[1], _seg->colors[2]);
}


/*
 * White running on _color.
 */
uint16_t WS2812FX_mode_chase_color(void)
{
    return WS2812FX_chase(_seg->colors[0], WHITE, WHITE);
}


/*
 * Black running on _color.
 */
uint16_t WS2812FX_mode_chase_blackout(void)
{
    return WS2812FX_chase(_seg->colors[0], BLACK, BLACK);
}


/*
 * _color running on white.
 */
uint16_t WS2812FX_mode_chase_white(void)
{
    return WS2812FX_chase(WHITE, _seg->colors[0], _seg->colors[0]);
}


/*
 * Strobe effect with different strobe count and pause, controlled by speed.
  颜色爆闪
 */
uint16_t WS2812FX_mode_multi_strobe(void)
{
    Adafruit_NeoPixel_fill(_seg->colors[1], _seg->start, _seg_len);

    uint16_t delay = 200 + ((9 - (_seg->speed % 10)) * 100);
    uint16_t count = 2 * ((_seg->speed / 100) + 1);
    if (_seg_rt->counter_mode_step < count)
    {
        if ((_seg_rt->counter_mode_step & 1) == 0)
        {
            Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg_len);
            delay = 20;
        }
        else
        {
            delay = 50;
        }
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (count + 1);
    if (_seg_rt->counter_mode_step == 0) SET_CYCLE;
    return delay;
}

/*
 * Blinks one LED at a time.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_sparkle(void)
{
    return WS2812FX_sparkle(_seg->colors[1], _seg->colors[0]);
}


/*
 * Lights all LEDs in the color. Flashes white pixels randomly.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_flash_sparkle(void)
{
    return WS2812FX_sparkle(_seg->colors[0], WHITE);
}


/*
 * Like flash sparkle. With more flash.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_hyper_sparkle(void)
{
    Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg_len);

    uint8_t size = 1 << SIZE_OPTION;
    for (uint8_t i = 0; i < 8; i++)
    {
        Adafruit_NeoPixel_fill(WHITE, _seg->start + WS2812FX_random16_lim(_seg_len - size), size);
    }

    SET_CYCLE;
    return (_seg->speed / 32);
}


/*
 * Blink several LEDs on, fading out.
 */
uint16_t WS2812FX_mode_twinkle_fade(void)
{
    return WS2812FX_twinkle_fade(_seg->colors[0]);
}


/*
 * Blink several LEDs in random colors on, fading out.
 */
uint16_t WS2812FX_mode_twinkle_fade_random(void)
{
    return WS2812FX_twinkle_fade(WS2812FX_color_wheel(WS2812FX_random8()));
}


// 所有LED当前颜色淡出，弹出最终颜色为_seg->colors[1]
uint16_t WS2812FX_mode_fade_single(void)
{

    WS2812FX_fade_out();
    return (_seg->speed / 8);
}

/*
 * Blink several LEDs on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle(void)
{
    return WS2812FX_twinkle(_seg->colors[0], _seg->colors[1]);
}

/*
 * Blink several LEDs in random colors on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle_random(void)
{
    return WS2812FX_twinkle(WS2812FX_color_wheel(WS2812FX_random8()), _seg->colors[1]);
}


/*
 * Theatre-style crawling lights with rainbow effect.
 * Inspired by the Adafruit examples.
 */
uint16_t WS2812FX_mode_theater_chase_rainbow(void)
{
    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
    uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);
    return WS2812FX_tricolor_chase(color, _seg->colors[1], _seg->colors[1]);
}


/*
 * Running lights effect with smooth sine transition.
 */
uint16_t WS2812FX_mode_running_lights(void)
{
    uint8_t size = 1 << SIZE_OPTION;
    uint8_t sineIncr = max(1, (256 / _seg_len) * size);
    for (uint16_t i = 0; i < _seg_len; i++)
    {
        int lum = (int)Adafruit_NeoPixel_sine8(((i + _seg_rt->counter_mode_step) * sineIncr));
        uint32_t color = WS2812FX_color_blend(_seg->colors[0], _seg->colors[1], lum);
        if (IS_REVERSE)
        {
            WS2812FX_setPixelColor(_seg->start + i, color);
        }
        else
        {
            WS2812FX_setPixelColor(_seg->stop - i, color);
        }
    }
    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % 256;
    if (_seg_rt->counter_mode_step == 0) SET_CYCLE;
    return (_seg->speed / _seg_len);
}



/*
 * Tricolor chase mode
 */
uint16_t WS2812FX_mode_tricolor_chase(void)
{
    return WS2812FX_tricolor_chase(_seg->colors[0], _seg->colors[1], _seg->colors[2]);
}


/*
 * Alternating white/red/black pixels running.
 */
uint16_t WS2812FX_mode_circus_combustus(void)
{
    return WS2812FX_tricolor_chase(RED, WHITE, BLACK);
}


/*
 * Theatre-style crawling lights.
 * Inspired by the Adafruit examples.
 */
uint16_t WS2812FX_mode_theater_chase(void)
{
    return WS2812FX_tricolor_chase(_seg->colors[0], _seg->colors[1], _seg->colors[1]);
}

/*
 * Cycles a rainbow over the entire string of LEDs.
 彩虹颜色流水效果
 */
uint16_t WS2812FX_mode_rainbow_cycle(void)
{
    for (uint16_t i = 0; i < _seg_len; i++)
    {
        uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
        WS2812FX_setPixelColor(_seg->stop - i, color);
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

    if (_seg_rt->counter_mode_step == 0) SET_CYCLE;

    return (_seg->speed / 256);
}

/*
 * Cycles all LEDs at once through a rainbow.
 */
uint16_t WS2812FX_mode_rainbow(void)
{
    uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

    if (_seg_rt->counter_mode_step == 0)  SET_CYCLE;

    return (_seg->speed / 256);
}

/*
 * Runs a block of pixels back and forth.
 来回运动像素块
 */
uint16_t WS2812FX_mode_scan(void)
{
    return WS2812FX_scan(_seg->colors[0], _seg->colors[1], false);
}


/*
 * Runs two blocks of pixels back and forth in opposite directions.
 */
uint16_t WS2812FX_mode_dual_scan(void)
{
    return WS2812FX_scan(_seg->colors[0], _seg->colors[1], true);
}


/*
 * Fades the LEDs between two colors
 */
uint16_t WS2812FX_mode_fade(void)
{
    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step += 4;
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }
    return (_seg->speed / 128);
}

/*
 * Fades the LEDs between two colors
 */
uint16_t WS2812FX_mode_mutli_fade(void)
{
    int lum = _seg_rt->counter_mode_step % 256;
    int base_index = _seg_rt->counter_mode_step / 256;
    int tar_index = base_index + 1;
    if (tar_index >= _seg->c_n) tar_index = 0;

    uint32_t color = WS2812FX_color_blend(_seg->colors[base_index], _seg->colors[tar_index], lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step += 1;
    if (_seg_rt->counter_mode_step >= 256 * _seg->c_n)
    {
        _seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }
    return (_seg->speed / 16);
}

/*
 * Does the "standby-breathing" of well known i-Devices. Fixed Speed.
 * Use mode "fade" if you like to have something similar with a different speed.
 * _seg->colors[1]，和_seg->colors[0]渐变，若_seg->colors[1]为很色就是呼吸功能
 * lum最小值决定两种颜色混合最小比例。典型值15，若为红色呼吸，LED最暗到15
 */
uint16_t WS2812FX_mode_breath(void)
{
    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 15 -> 255 -> 15

    uint16_t delay;
    if (lum == 15) delay = 970; // 970 pause before each breath
    else if (lum <= 25) delay = 38; // 19
    else if (lum <= 50) delay = 36; // 18
    else if (lum <= 75) delay = 28; // 14
    else if (lum <= 100) delay = 20; // 10
    else if (lum <= 125) delay = 14; // 7
    else if (lum <= 150) delay = 11; // 5
    else delay = 10; // 4

    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step += 2;
    if (_seg_rt->counter_mode_step > (512 - 15))
    {
        _seg_rt->counter_mode_step = 15;
        SET_CYCLE;
    }
    return delay;
}

/*
 * Does the "standby-breathing" of well known i-Devices. Fixed Speed.
 * Use mode "fade" if you like to have something similar with a different speed.
 * _seg->colors[1]，和_seg->colors[0]渐变，若_seg->colors[1]为很色就是呼吸功能
 * lum最小值决定两种颜色混合最小比例。典型值15，若为红色呼吸，LED最暗到15
 */
uint16_t WS2812FX_mode_breath_speed(void)
{
    int min_lum = 1;
    u16 each_breath_delay = 470;

    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = min_lum -> 255 -> min_lum

    uint16_t delay;
    if (lum == min_lum) delay = each_breath_delay; // <each_breath_delay> pause before each breath
    else if (lum <= 25) delay = 38; // 19
    else if (lum <= 50) delay = 36; // 18
    else if (lum <= 75) delay = 28; // 14
    else if (lum <= 100) delay = 20; // 10
    else if (lum <= 125) delay = 14; // 7
    else if (lum <= 150) delay = 11; // 5
    else delay = 10; // 4

    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step += 2;
    if (_seg_rt->counter_mode_step > (512 - min_lum))
    {
        _seg_rt->counter_mode_step = min_lum;
        SET_CYCLE;
    }
    
    return delay * _seg->speed / UWS_SPEED_MAX;
}


/*
 * Lights every LED in a random color. Changes all LED at the same time
 * to new random colors.
 * 每次以随机颜色变换所有LED
 */
uint16_t WS2812FX_mode_multi_dynamic(void)
{
    for (uint16_t i = _seg->start; i <= _seg->stop; i++)
    {
        WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));
    }
    SET_CYCLE;
    return _seg->speed;
}



/*
 * Lights all LEDs in one random color up. Then switches them
 * to the next random color.
 * 彩虹跳变
 */
uint16_t WS2812FX_mode_random_color(void)
{
    _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param); // aux_param will store our random color wheel index
    uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
    SET_CYCLE;
    return _seg->speed;
}


/*
 * Lights every LED in a random color. Changes one random LED after the other
 * to another random color.
 * 以随机颜色点亮每个LED。依次更改一个随机LED
 * 另一种随机颜色。
 */
uint16_t WS2812FX_mode_single_dynamic(void)
{
    if (_seg_rt->counter_mode_call == 0)
    {
        for (uint16_t i = _seg->start; i <= _seg->stop; i++)
        {
            WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));
        }
    }

    WS2812FX_setPixelColor(_seg->start + WS2812FX_random16_lim(_seg_len), WS2812FX_color_wheel(WS2812FX_random8()));
    SET_CYCLE;
    return _seg->speed;
}


/*
 * Turns all LEDs after each other to a random color.
 * Then starts over with another color.
 * 彩虹随机颜色依次流水
 */
uint16_t WS2812FX_mode_color_wipe_random(void)
{
    if (_seg_rt->counter_mode_step % _seg_len == 0)
    { // aux_param will store our random color wheel index
        _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
    }
    uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
    return WS2812FX_color_wipe(color, color, false) * 2;
}


/*
 * Random color introduced alternating from start and end of strip.
 彩虹颜色往返流水，每次到达起点/终点，变换颜色
 */
uint16_t WS2812FX_mode_color_sweep_random(void)
{
    if (_seg_rt->counter_mode_step % _seg_len == 0)
    { // aux_param will store our random color wheel index
        _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
    }
    uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
    return WS2812FX_color_wipe(color, color, true) * 2;
}


/*
 * Lights all LEDs one after another.
 */
uint16_t WS2812FX_mode_color_wipe(void)
{
    return WS2812FX_color_wipe(_seg->colors[0], _seg->colors[1], false);
}

uint16_t WS2812FX_mode_color_wipe_inv(void)
{
    return WS2812FX_color_wipe(_seg->colors[1], _seg->colors[0], false);
}

uint16_t WS2812FX_mode_color_wipe_rev(void)
{
    return WS2812FX_color_wipe(_seg->colors[0], _seg->colors[1], true);
}

uint16_t WS2812FX_mode_color_wipe_rev_inv(void)
{
    return WS2812FX_color_wipe(_seg->colors[1], _seg->colors[0], true);
}


/*
 * Normal blinking. 50% on/off time.
 */
uint16_t WS2812FX_mode_blink(void)
{
    return WS2812FX_blink(_seg->colors[0], _seg->colors[1], false);
}


/*
 * Classic Blink effect. Cycling through the rainbow.
 彩虹颜色和_seg->colors[1]交替闪烁，彩虹颜色一直在变换
 */
uint16_t WS2812FX_mode_blink_rainbow(void)
{
    return WS2812FX_blink(WS2812FX_color_wheel(_seg_rt->counter_mode_call & 0xFF), _seg->colors[1], false);
}


/*
 * Classic Strobe effect.
 两个颜色爆闪，_seg->colors[1]下突然爆闪一下_seg->colors[0]，
_seg->colors[0]时间很多眼睛都没察觉
 */
uint16_t WS2812FX_mode_strobe(void)
{
    return WS2812FX_blink(_seg->colors[0], _seg->colors[1], true);
}


/*
 * Classic Strobe effect. Cycling through the rainbow.
 彩虹色爆闪
 */
uint16_t WS2812FX_mode_strobe_rainbow(void)
{
    return WS2812FX_blink(WS2812FX_color_wheel(_seg_rt->counter_mode_call & 0xFF), _seg->colors[1], true);
}

/*
 * No blinking. Just plain old static light.
 */
uint16_t  WS2812FX_mode_static(void)
{
    Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg_len);
    SET_CYCLE;
    return _seg->speed;
}


// 多种颜色跳变
uint16_t WS2812FX_mutil_c_jump(void)
{
    Adafruit_NeoPixel_fill(_seg->colors[_seg_rt->counter_mode_step], _seg->start, _seg_len);
    _seg_rt->counter_mode_step++;
    _seg_rt->counter_mode_step %= _seg->c_n;
    return _seg->speed * 8;
}


#ifdef fc_effect
// 整条灯带渐变，支持多种颜色之间切换
// 颜色池：fc_effect.dream_scene.rgb[]
// 颜色数量fc_effect.dream_scene.c_n
uint16_t WS2812FX_mutil_c_gradual(void)
{
    static u8 index;
    u32 rgb;
    static u8 pp = 0;

    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    // _seg->colors[1]:目标颜色
    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);

    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    if (_seg_rt->counter_mode_step == 256)
    {
        pp = 2;
        index++;
        index %= fc_effect.dream_scene.c_n;

        rgb = ((u32)fc_effect.dream_scene.rgb[index].r << 16) |
            ((u32)fc_effect.dream_scene.rgb[index].g << 8) |
            ((u32)fc_effect.dream_scene.rgb[index].b);

        _seg->colors[0] = color;
        _seg->colors[1] = rgb;

    }

    _seg_rt->counter_mode_step++;
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;

        SET_CYCLE;
    }
    return (_seg->speed / 5);
}

// w通道呼吸
uint16_t breath_w(void)
{
    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    fc_effect.cw = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    Adafruit_NeoPixel_fill(0, _seg->start, _seg_len);
    _seg_rt->counter_mode_step += 1;
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;

        return 3000 + _seg->speed;
    }
    else if (_seg_rt->counter_mode_step == 255)
    {
        return _seg->speed;
    }

    else
    {
        return 10;
    }
}
#endif


// 支持多颜色频闪
uint16_t WS2812FX_mutil_strobe(void)
{
    if (_seg_rt->aux_param == 0)
    {
        Adafruit_NeoPixel_fill(_seg->colors[_seg_rt->counter_mode_step], _seg->start, _seg_len);
        _seg_rt->counter_mode_step++;
        _seg_rt->counter_mode_step %= _seg->c_n;
    }
    else
    {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
    }
    _seg_rt->aux_param = !_seg_rt->aux_param;

    return _seg->speed / 2;
}

// rgb通道呼吸
// 按照用户需求，亮5秒，灭3秒。这个时间为最小时间单位，进行调节
// 整体呼吸，亮-》灭-》亮
uint16_t breath_rgb(void)
{
    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    _seg_rt->counter_mode_step += 1;
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;

        return 3000 + _seg->speed;
    }
    else if (_seg_rt->counter_mode_step == 255)
    {
        return _seg->speed;
    }

    else
    {
        return 10;
    }
}



// ----------------------------------------------------------------------------全彩音乐效果

#ifdef fc_effect
// 渐变，触发变色
uint16_t fc_music_gradual(void)
{
    extern u8 music_trigger;
    uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);

    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    if (get_sound_result())
    {
        music_trigger = 0;
        _seg_rt->counter_mode_step += 20;
    }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

    if (_seg_rt->counter_mode_step == 0)  SET_CYCLE;
    // printf("fc_music_gradual(");
    return (100);
}


// 呼吸，触发渐亮-》渐暗，最后黑，每次变色
uint16_t fc_music_breath(void)
{
    extern u8 music_trigger;
    static uint32_t color1;
    if (get_sound_result())
    {
        // if(_seg_rt->counter_mode_step == 0)
        {
            color1 = WS2812FX_color_wheel(_seg_rt->aux_param);
            _seg_rt->aux_param += 5;
        }
        // music_trigger = 0;
        _seg_rt->counter_mode_step = 1;
    }

    int lum = _seg_rt->counter_mode_step;
    if (lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    uint32_t color = WS2812FX_color_blend(0, color1, lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
    if (_seg_rt->counter_mode_step != 0)
    {
        _seg_rt->counter_mode_step += 8;
    }
    if (_seg_rt->counter_mode_step > 511)
    {
        _seg_rt->counter_mode_step = 0;
        // music_trigger = 0;
        // printf("\n fc_music_breath");
    }
    return 10;
}

// 定色，触发换颜色
uint16_t fc_music_static(void)
{
    extern u8 music_trigger;
    uint32_t color1;

    if (music_trigger)

        // if(get_sound_result())
    {
        //if(_seg_rt->counter_mode_step == 0)
        {
            color1 = WS2812FX_color_wheel(_seg_rt->aux_param);
            _seg_rt->aux_param += 20;
        }
        music_trigger = 0;
        Adafruit_NeoPixel_fill(color1, _seg->start, _seg_len);
    }
    //printf("music_static");
    return 0xffff;    //配合 WS2812FX_trigger();使用

}

// 定色，触发黑->爆闪一下，每次变色
uint16_t fc_music_twinkle(void)
{
    extern u8 music_trigger;
    uint32_t color1;
    if (get_sound_result())
    {
        // if(_seg_rt->counter_mode_step == 0)
        {
            color1 = WS2812FX_color_wheel(_seg_rt->aux_param);
            _seg_rt->aux_param += 20;
        }
        // music_trigger = 0;
        Adafruit_NeoPixel_fill(color1, _seg->start, _seg_len);
    }
    else
    {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
    }
    return 100;
}
#endif

#if 0 // USER_TO_DO 这里先屏蔽声控的效果
/*
 * 
 */
uint16_t WS2812FX_mode_music_full_bright(void)
{
    uint32_t color = (mic_power_for_ws2812 << 16) | (mic_power_for_ws2812 << 8);
    // _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param); // aux_param will store our random color wheel index
    // uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
    SET_CYCLE;
    return 10;
}

uint16_t WS2812FX_mode_music_move_up(void)
{
    static uint16_t num = 0;

    uint16_t current_num = mic_power_for_ws2812;
    if (current_num > num)
    {
        num += (current_num - num) / 2 + 1;
    }
    else if (current_num < num)
    {
        num -= (num - current_num) / 12 + 1;
    }

    uint16_t step = 255 / _seg_len;
    uint16_t led_num = num / step;
    uint16_t last_rgb = num % step;
    
    if (led_num > _seg_len) led_num = _seg_len;

    Adafruit_NeoPixel_fill(_seg->colors[1], _seg->start, _seg_len);
    if (led_num)
        Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, led_num);

    // printf("a=%d,", led_num);

    SET_CYCLE;
    return 10;
}

uint16_t WS2812FX_mode_music_move_down(void)
{
    static uint16_t num = 0;

    uint16_t current_num = mic_power_for_ws2812;
    if (current_num > num)
    {
        num += (current_num - num) / 2 + 1;
    }
    else if (current_num < num)
    {
        num -= (num - current_num) / 12 + 1;
    }

    uint16_t step = 255 / _seg_len;
    uint16_t led_num = num / step;
    uint16_t last_rgb = num % step;

    if (led_num > _seg_len) led_num = _seg_len;

    Adafruit_NeoPixel_fill(_seg->colors[1], _seg->start, _seg_len);
    if (led_num)
        Adafruit_NeoPixel_fill(_seg->colors[0], (_seg_len - 1) - (led_num - 1), led_num);

    SET_CYCLE;
    return 33;
}
#endif
#endif

#if 0
void teset(void)
{
    ls_set_color(0, RED);
    ls_set_color(1, GREEN);
    ls_set_color(2, BLUE);
    ls_set_color(3, YELLOW);
    ls_set_color(4, CYAN);
    ls_set_color(5, MAGENTA);
    ls_set_color(6, WHITE);
    fc_effect.dream_scene.c_n = 7;


    WS2812FX_setSegment_colorOptions(   //设置一段颜色的效果
        0,                              //第0段
        0,fc_effect.led_num-1,          //起始位置，结束位置
        &test_effect,                   //效果
        0,                              //颜色，WS2812FX_setColors设置
        100,                            //速度
        0);                             //选项，这里像素点大小：1
    // WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);  // 设置颜色数量  0：第0段   fc_effect.dream_scene.c_n  颜色数量，一个颜色包含（RGB）
    // ls_set_colors(1, &fc_effect.rgb);   //1:1个颜色    &fc_effect.rgb 这个颜色是什么色
    
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
}

uint16_t longmeteor_signle_c(void)
{
    static uint8_t flash_flag = 1;
    static uint8_t buff[24 * 4];  // 24个点，每个点4个字节，需要根据实际情况调整，大小=灯数量x（RGB：3 或者 RGBW：4）

    static uint8_t i = 0;
    uint32_t r1, g1, b1, w1;
    const uint8_t rate[50] = { 100,100,100,100,100,100,100,100,100,100,
                              100,100,100,100,100,100,100,100,100,100,
                              100,100,100,100,100,100,100,100,100,100,
                              100,100,100,100,100,100,100,100,100,100,
                              0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };

    int w = (_seg->colors[_seg_rt->aux_param] >> 24) & 0xff;
    int r = (_seg->colors[_seg_rt->aux_param] >> 16) & 0xff;
    int g = (_seg->colors[_seg_rt->aux_param] >> 8) & 0xff;
    int b = _seg->colors[_seg_rt->aux_param] & 0xff;

    if (flash_flag == 1)
    {   // 亮时照常推进1格
        if (IS_REVERSE) //反向
        {
            WS2812FX_copyPixels(_seg->start, _seg->start + 1, _seg_len - 1);
        }
        else
        {
            WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1);
        }
    }
    else
    {   // 灭时从buff中取数据再推进一格
        uint8_t *pixels = Adafruit_NeoPixel_getPixels();
        uint8_t bytesPerPixel = WS2812FX_getNumBytesPerPixel(); // 3=RGB, 4=RGBW

        if (IS_REVERSE) //反向
        {
            memcpy(pixels + (_seg->start * bytesPerPixel), &buff[1], (_seg_len - 1) * bytesPerPixel);
        }
        else
        {
            memcpy(pixels + ((_seg->start + 1) * bytesPerPixel), &buff[0], (_seg_len - 1) * bytesPerPixel);
        }
    }

    r1 = r * rate[i] / 100;
    g1 = g * rate[i] / 100;
    b1 = b * rate[i] / 100;
    w1 = w * rate[i] / 100;

    if (IS_REVERSE) //反向
    {
        WS2812FX_setPixelColor_rgbw(_seg->stop, r1, g1, b1, w1);
    }
    else
    {
        WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);
    }

    {   // 复制计算后的数据到buff
        // uint8_t *pixels = Adafruit_NeoPixel_getPixels();
        // uint8_t bytesPerPixel = WS2812FX_getNumBytesPerPixel(); // 3=RGB, 4=RGBW
        // memcpy(buff, pixels, _seg_len * bytesPerPixel);
        memcpy(buff, Adafruit_NeoPixel_getPixels(), _seg_len * WS2812FX_getNumBytesPerPixel());
    }

    // 计算亮灭时机
    static uint16_t flash_count = 0;
    flash_count++;
    if (flash_count == 1)  // 经过1次调用后，灭
    {
        flash_flag = 0;
    }
    else if (flash_count == 2) // 经过2次调用后，亮
    {
        flash_flag = 1;
        flash_count = 0;
    }

    if (flash_flag == 0)
    {
        for (uint16_t index = 0; index < _seg_len; index++)
        {
            WS2812FX_setPixelColor_rgbw(_seg->start + index, 0, 0, 0, 0);
        }
    }

    i++;
    i %= 50;
    if (i == 0)
    {
        _seg_rt->aux_param++;
        _seg_rt->aux_param %= _seg->c_n;
    }
    return (_seg->speed);
}
#endif
