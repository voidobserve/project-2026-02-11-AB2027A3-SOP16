#include "include.h"
#include "driver_sddac.h"
#include "res.h"
#include "api_codec.h"
#include "driver_gpio.h"

#if (TONE_PLAY_MP3_DEC_EN)

// 配置区
//SYS CLK 48M 以上
#define MP3_DEC_OBUF_SAMPLE_SIZE      576          // mp3解码固定输出样点大小
#define MP3_DEC_DAC_IBUF_SAMPLE_SIZE  (576*2)      // dac样点大小
uint8_t music_mp3_long_buffer = 1;                 // 大于192
uint8_t spi_audio_test_en = 0;                     // 测试模式使能
uint16_t mp3_fade_in_time = 36;                    // 淡入时间，36的倍数
uint16_t mp3_fade_out_time = 36;                   // 淡出时间，36的倍数

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

static uint8_t mp3_decode_spr = 2;  //0:48K, 1:24K, 2:16K, 3:12K, 4:8K， 默认16K
static int16_t mp3_decode_obuf[MP3_DEC_OBUF_SAMPLE_SIZE] AT(.codec.mp3.buf);
static int16_t mp3_decode_dac_ibuf[MP3_DEC_DAC_IBUF_SAMPLE_SIZE] AT(.codec.mp3.buf);

void spi_read_function_register(void (* read_func)(uint8_t *buf, uint32_t addr, uint32_t len));

AT(.com_text.dac)
void mp3_dec_end_call_back(int16_t *ptr, uint32_t samples)
{
    //samples: 576
    bsp_tone_play_volume_proc((int16_t*)&mp3_decode_obuf, ptr, samples);
}

void mp3_dec_spr_set_call_back(uint8_t spr)
{
    mp3_decode_spr = spr;
}

void mp3_dec_buf_init(void)
{
    uint16_t sp_len = MP3_DEC_DAC_IBUF_SAMPLE_SIZE;
    for (int16_t i = 0; i < sp_len; i++) {
        if (i < MP3_DEC_OBUF_SAMPLE_SIZE) {
            mp3_decode_obuf[0] = 0;
        }
        mp3_decode_dac_ibuf[i] = 0;
    }
}

void mp3_res_init(void)
{
#if TONE_PLAY_EXTERN_FLASH_EN
    //uint extern_spiflash_read(void *buf, u32 addr, uint len);  //用户自己添加
    spi_read_function_register((void *)extern_spiflash_read);    //mp3注册外挂flash读接口
#else
    spi_read_function_register(NULL);
#endif
}

void mp3_res_deinit(void)
{
    mp3_res_play_exit();
}

bool mp3_res_play(uint32_t addr, uint32_t len)
{
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);

    if (len == 0) {
        return false;
    }

    if (mp3_music_dec_sta_get() != MUSIC_STOP) {
        mp3_music_dec_sta_set(MUSIC_STOP);
    }

    mp3_dec_buf_init();

    //mp3 dec init
    mp3_res_play_kick(addr, len);

    //mp3 dec first frame, get sampling rate
    mp3_dec_frame();
    mp3_music_pcm_kick();

    if (mp3_decode_spr != sddac_spr_get()) {
        // bsp_tone_play_output_mute(1);   //解决播放不同采样率的提示音出现杂音问题
    }

    GPIOADE &= ~BIT(2);
    sddac_spr_set(mp3_decode_spr);

    // bsp_tone_play_output_mute(0);   //解决播放不同采样率的提示音出现杂音问题

    sddac_dma_kick_start((uint8_t*)mp3_decode_dac_ibuf, MP3_DEC_DAC_IBUF_SAMPLE_SIZE);
    GPIOADE |= BIT(2);
    return true;
}

uint8_t mp3_res_play_proc(SDDAC_IT_TYPEDEF done_type)
{
    if (mp3_music_dec_sta_get() != MUSIC_STOP) {
        if (done_type == SDDAC_IT_ALL_DONE) {
            if (mp3_dec_frame()) {
                mp3_music_pcm_kick();
                memcpy((uint8_t*)&mp3_decode_dac_ibuf[MP3_DEC_DAC_IBUF_SAMPLE_SIZE/2], (uint8_t*)mp3_decode_obuf, MP3_DEC_OBUF_SAMPLE_SIZE*2);
            } else {
                #if (SYS_SLEEP_TIME)
                    lowpwr_sleep_auto_en();
                #endif //SYS_SLEEP_TIME
                mp3_res_play_exit();
                printf("mp3 play end.\n");
                return 1;
            }
        }

        if (done_type == SDDAC_IT_HALF_DONE) {
            if (mp3_dec_frame()) {
                mp3_music_pcm_kick();
                memcpy((uint8_t*)&mp3_decode_dac_ibuf[0], (uint8_t*)mp3_decode_obuf, MP3_DEC_OBUF_SAMPLE_SIZE*2);
            } else {
            }
        }
    }

    return 0;
}

#endif  //(TONE_PLAY_MP3_DEC_EN)
