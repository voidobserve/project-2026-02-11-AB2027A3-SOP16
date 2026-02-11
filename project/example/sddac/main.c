#include "include.h"
#include "driver_sddac.h"
#include "driver_gpio.h"
#include "bsp_uart_debug.h"
#include "res.h"

#define DIGITAL_VOLUME_DEFAULT   7
#define DIGITAL_VOLUME_LEVEL     8  //level:0~7

/*****************************************************/
//Only one of these functions can be enabled at a time
#define SDDAC_SIN_TEST      0
#define SDDAC_WAV_TEST      0
#define SDDAC_MP3_TEST      1

static uint8_t dig_vol_level = 0;
AT(.com_rodata.tone_play)
const uint16_t dig_vol_level_tbl[DIGITAL_VOLUME_LEVEL] = {32767, 50, 30, 20, 10, 5, 3, 1};  //level:0~7

void sin_res_init(void);
void wav_res_init(void);
void mp3_res_init(void);

void sin_res_deinit(void);
void wav_res_deinit(void);
void mp3_res_deinit(void);

void tone_play_sin_test(void);
bool tone_play_wav_test(void);
void tone_play_mp3_test(void);

void sin_res_play_proc(void);
void wav_res_play_proc(void);
void mp3_res_play_proc(void);

///level:0~7
void tone_play_volume_set(int8_t vol)
{
    vol = (vol > DIGITAL_VOLUME_LEVEL-1) ? (DIGITAL_VOLUME_LEVEL-1) : vol;
    vol = (vol < 0) ? 0 : vol;

    dig_vol_level = vol;
}

uint16_t tone_play_volume_get(void)
{
    return dig_vol_level_tbl[dig_vol_level];
}

void tone_play_volume_proc(int16_t *dest, int16_t *src, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        dest[i] = src[i] / dig_vol_level_tbl[dig_vol_level];
    }
}

void tone_play_init(void)
{
#if SDDAC_SIN_TEST
    sin_res_init();
#elif SDDAC_WAV_TEST
    wav_res_init();
#elif SDDAC_MP3_TEST
    mp3_res_init();
#endif
}

void tone_play_denit(void)
{
#if SDDAC_SIN_TEST
    sin_res_deinit();
#elif SDDAC_WAV_TEST
    wav_res_deinit();
#elif SDDAC_MP3_TEST
    mp3_res_deinit();
#endif
}

void tone_play_test(void)
{
#if SDDAC_SIN_TEST
    tone_play_sin_test();
#elif SDDAC_WAV_TEST
    tone_play_wav_test();
#elif SDDAC_MP3_TEST
    tone_play_mp3_test();
#endif
}

void tone_play_proc(void)
{
#if SDDAC_SIN_TEST
    sin_res_play_proc();
#elif SDDAC_WAV_TEST
    wav_res_play_proc();
#elif SDDAC_MP3_TEST
    mp3_res_play_proc();
#endif  //SDDAC_MP3_TEST
}

//sellect sddac clk --> configure sddac samplerate --> configure DMA addr and size --> map dac to PA2 --> enable DMA_EN and DAC_EN
int main(void)
{
    uint8_t cnt = 0;
    uint32_t tick = -1;

    WDT_DIS();

    sys_clk_set(SYS_120M);

    bsp_uart_debug_init();

    printf("--->sddac example\n");

    tone_play_volume_set(DIGITAL_VOLUME_DEFAULT);

    tone_play_init();  //上电或者睡眠唤醒后

    printf("tone play volume:%d\n", dig_vol_level);

    while (cnt <= 3) {

        //3s触发一次提示音，测试
        if (tick_check_expire(tick, 3000)) {
            cnt++;
            tick = tick_get();
            if (cnt <= 3) {
                printf("tone play\n");
                tone_play_test();
            }
        }
        tone_play_proc();  //非阻塞处理，放func_process()这类函数中
    }

    tone_play_denit();  //关机或者睡眠前
}
