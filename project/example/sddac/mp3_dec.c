#include "include.h"
#include "driver_sddac.h"
#include "res.h"
#include "api_codec.h"
#include "driver_gpio.h"

// 配置区
//SYS CLK 48M 以上
#define MP3_DEC_OBUF_SAMPLE_SIZE      576          // mp3解码固定输出样点大小
#define MP3_DEC_DAC_IBUF_SAMPLE_SIZE  (576*2)      // dac样点大小
uint8_t music_mp3_long_buffer = 1;                 // 大于192
uint8_t spi_audio_test_en = 0;                     // 测试模式使能
uint16_t mp3_fade_in_time = 36;                    // 淡入时间，36的倍数
uint16_t mp3_fade_out_time = 36;                   // 淡出时间，36的倍数

static uint8_t sys_clk_rec;

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

#if RES_USERBIN_EN
void spi_read_function_register(void (* read_func)(uint8_t *buf, u32 addr, u32 len));
#endif

void tone_play_volume_set(int8_t vol);
uint16_t tone_play_volume_get(void);
void tone_play_volume_proc(int16_t *dest, int16_t *src, uint16_t len);

static uint8_t mp3_decode_spr = 2;  //0:48K, 1:24K, 2:16K, 3:12K, 4:8K， 默认16K
static int16_t mp3_decode_obuf[MP3_DEC_OBUF_SAMPLE_SIZE] AT(.codec.mp3.buf);
static int16_t mp3_decode_dac_ibuf[MP3_DEC_DAC_IBUF_SAMPLE_SIZE] AT(.codec.mp3.buf);

AT(.com_text.dac)
void mp3_dec_end_call_back(int16_t *ptr, uint32_t samples)
{
    //samples: 576
    tone_play_volume_proc((int16_t*)&mp3_decode_obuf, ptr, samples);
}

void mp3_dec_spr_set_call_back(uint8_t spr)
{
    mp3_decode_spr = spr;
}

//功放mute脚控制，用户补充代码
void mp3_audio_output_mute(bool mute)
{
    if (mute) {
        // gpio_set_bits(GPIOB_REG, GPIO_PIN_2);
    } else {
        // gpio_reset_bits(GPIOB_REG, GPIO_PIN_2);
    }
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
    mp3_audio_output_mute(1);

    gpio_init_typedef gpio_init_structure;
     //dac init, dac first frame silence
    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    FUNO_PA2SEL(FO_AUDIO);                                    //Map dac output PA2
    
    mp3_music_dec_sta_set(MUSIC_STOP);
    mp3_audio_output_mute(0);
}

void mp3_res_deinit(void)
{
    mp3_music_dec_sta_set(MUSIC_STOP);
    mp3_audio_output_mute(1);
    gpio_deinit(GPIOA_REG, GPIO_PIN_2);
    sddac_cmd(DISABLE);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_DIS);
}

void mp3_res_play(u32 addr, u32 len)
{
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);

    if (len == 0) {
        return;
    }

#if (SYS_SLEEP_TIME)
    lowpwr_sleep_auto_dis();
#endif //SYS_SLEEP_TIME

#if RES_USERBIN_EN
    spi_read_function_register(os_spiflash_read);
#endif

    if (mp3_music_dec_sta_get() != MUSIC_STOP) {
        mp3_music_dec_sta_set(MUSIC_STOP);
    }

    sys_clk_rec = sys_clk_get();

    if (sys_clk_rec < SYS_120M) {
        sys_clk_set(SYS_120M);
    }

    mp3_dec_buf_init();

    //mp3 dec init
    mp3_res_play_kick(addr, len);

    //mp3 dec first frame, get sampling rate
    mp3_dec_frame();
    mp3_music_pcm_kick();

    mp3_audio_output_mute(1);
    GPIOADE &= ~BIT(2);

    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_EN);

    sddac_init_typedef sddac_config;
    sddac_config.dma_buf = (uint8_t*)mp3_decode_dac_ibuf;
    sddac_config.dma_size = MP3_DEC_DAC_IBUF_SAMPLE_SIZE;  // HalfWord
    sddac_config.sample_rate = mp3_decode_spr;
    sddac_init(&sddac_config);

    sddac_cmd(ENABLE);
    delay_5ms(10);
    GPIOADE |= BIT(2);
    mp3_audio_output_mute(0);
}

void tone_play_mp3_test(void)
{
#if RES_USERBIN_EN
    fot_ubin_head_t ubin_head;
    os_spiflash_read(&ubin_head, USERBIN_START_ADDR, sizeof(ubin_head));
    printf("ubin_head.down_area_start:%x, ubin_head.data_size:%x\n",ubin_head.down_area_start,ubin_head.data_size);
    mp3_res_play(ubin_head.down_area_start + sizeof(ubin_head), ubin_head.data_size);
#else
    mp3_res_play(RES_BUF_RINGTONE_MP3_MP3, RES_LEN_RINGTONE_MP3_MP3);
#endif
}

void mp3_res_play_proc(void)
{
    if (mp3_music_dec_sta_get() != MUSIC_STOP) {
        if (sddac_get_flag(SDDAC_IT_ALL_DONE)) {
            sddac_clear_flag(SDDAC_IT_ALL_DONE);
            if (mp3_dec_frame()) {
                mp3_music_pcm_kick();
                memcpy((uint8_t*)&mp3_decode_dac_ibuf[MP3_DEC_DAC_IBUF_SAMPLE_SIZE/2], (uint8_t*)mp3_decode_obuf, MP3_DEC_OBUF_SAMPLE_SIZE*2);
            } else {
                if (sys_clk_rec < SYS_160M) {
                    sys_clk_set(sys_clk_rec);  //恢复时钟
                }
                #if (SYS_SLEEP_TIME)
                    lowpwr_sleep_auto_en();
                #endif //SYS_SLEEP_TIME
                mp3_res_play_exit();
                GPIOADE &= ~BIT(2);
                sddac_cmd(DISABLE);
                printf("mp3 play end.\n");
                return;
            }
        }

        if (sddac_get_flag(SDDAC_IT_HALF_DONE)) {
            sddac_clear_flag(SDDAC_IT_HALF_DONE);
            if (mp3_dec_frame()) {
                mp3_music_pcm_kick();
                memcpy((uint8_t*)&mp3_decode_dac_ibuf[0], (uint8_t*)mp3_decode_obuf, MP3_DEC_OBUF_SAMPLE_SIZE*2);
            } else {
            }
        }
    }
}