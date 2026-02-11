#include "include.h"
#include "bsp_tone.h"
#include "driver_gpio.h"
#include "driver_sddac.h"
#include "api_sddac.h"

#if (TONE_PLAY_EN)

#define DIGITAL_VOLUME_DEFAULT       7
#define DIGITAL_VOLUME_LEVEL_MAX     8  //level:0~7

AT(.codec.tone_play)
tone_paly_cb_t tone_play_cb;

AT(.codec.tone_play)
static int16_t tone_play_dac_temp_buf[32] = {0};

AT(.com_rodata.tone_play)
const uint16_t dig_vol_tbl[DIGITAL_VOLUME_LEVEL_MAX] = {32767, 50, 30, 20, 10, 5, 3, 1};  //level:0~7

void bsp_tone_play_proc_do(SDDAC_IT_TYPEDEF done_type);

//功放mute脚控制，用户补充代码
void bsp_tone_play_output_mute(bool mute)
{
    if (mute) {
        // gpio_set_bits(GPIOB_REG, GPIO_PIN_2);
    } else {
        // gpio_reset_bits(GPIOB_REG, GPIO_PIN_2);
    }
}

uint8_t bsp_tone_play_sta_get(void)
{
    return tone_play_cb.sta;
}

///level:0~7
void bsp_tone_play_volume_set(int8_t vol)
{
    vol = (vol > DIGITAL_VOLUME_LEVEL_MAX-1) ? (DIGITAL_VOLUME_LEVEL_MAX-1) : vol;
    vol = (vol < 0) ? 0 : vol;

    tone_play_cb.volume = vol;
}

uint16_t bsp_tone_play_volume_get(void)
{
    return dig_vol_tbl[tone_play_cb.volume];
}

void bsp_tone_play_volume_proc(int16_t *dest, int16_t *src, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        dest[i] = src[i] / dig_vol_tbl[tone_play_cb.volume];
    }
}

#if (TONE_PLAY_THREAD_EN)
AT(.com_text.sdadc.isr)
void bsp_tone_play_dac_isr(void)
{
    if (sddac_get_flag(SDDAC_IT_ALL_DONE) != RESET) {
        sddac_clear_flag(SDDAC_IT_ALL_DONE);
        sddac_done_proc_kick(SDDAC_IT_ALL_DONE);
    } else if (sddac_get_flag(SDDAC_IT_HALF_DONE) != RESET) {
        sddac_clear_flag(SDDAC_IT_HALF_DONE);
        sddac_done_proc_kick(SDDAC_IT_HALF_DONE);
    }
}
#endif  //(TONE_PLAY_THREAD_EN)

void bsp_tone_play_init(void)
{
    memset(&tone_play_cb, 0, sizeof(tone_paly_cb_t));

    bsp_tone_play_volume_set(DIGITAL_VOLUME_DEFAULT);

#if (TONE_PLAY_EXTERN_FLASH_EN)
    //uint extern_spiflash_init(void);  //用户自己添加
    extern_spiflash_init(void);
#endif  //(TONE_PLAY_EXTERN_FLASH_EN)

    gpio_init_typedef gpio_init_structure;
     //dac init, dac first frame silence
    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_drv = GPIO_DRV_24MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    FUNO_PA2SEL(FO_AUDIO);  //Map dac output PA2

#if (TONE_PLAY_THREAD_EN)
    sddac_done_callback_set(bsp_tone_play_proc_do);
#endif  //(TONE_PLAY_THREAD_EN)

    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_EN);

    sddac_init_typedef sddac_config;
    sddac_config.dma_buf = (uint8_t*)tone_play_dac_temp_buf;
    sddac_config.dma_size = 32;  // HalfWord
    sddac_config.sample_rate = SDDAC_NORMAL_SPR_16k;
    sddac_init(&sddac_config);
#if (TONE_PLAY_THREAD_EN)
    sddac_pic_config(bsp_tone_play_dac_isr, 0, SDDAC_IT_ALL_DONE | SDDAC_IT_HALF_DONE, ENABLE);
#endif  //(TONE_PLAY_THREAD_EN)
    sddac_cmd(ENABLE);
    sddac_dma_stop();
}

void bsp_tone_play_stop(void)
{
    tone_play_cb.sta = 0;
    bsp_tone_play_output_mute(1);
    GPIOADE &= ~BIT(2);
    sddac_dma_stop();
}

void bsp_tone_play(uint8_t tone_type, uint32_t addr, uint32_t len)
{
    bool res = 0;

    tone_play_cb.type = tone_type;
    tone_play_cb.addr = addr;
    tone_play_cb.len  = len;

    WDT_CLR();

#if (SYS_SLEEP_TIME)
    lowpwr_sleep_auto_dis();
#endif //SYS_SLEEP_TIME

    bsp_tone_play_output_mute(1);

    if (tone_play_cb.sta == 1) {
        sddac_dma_stop();
    }

    tone_play_cb.sta = 1;

    printf("tone play volume:%d\n", tone_play_cb.volume);

    switch (tone_type)
    {
        case TONE_TYPE_SIN: {
                #if (TONE_PLAY_SIN_DEC_EN)
                    sin_res_init();
                    res = sin_res_play((int16_t *)addr, len);
                #endif  //(TONE_PLAY_SIN_DEC_EN)
            } break;
        case TONE_TYPE_MP3: {
                #if (TONE_PLAY_MP3_DEC_EN)
                    tone_play_cb.sys_clk_rec = sys_clk_get();
                    if (tone_play_cb.sys_clk_rec < SYS_120M) {
                        sys_clk_set(SYS_120M);
                    }
                    mp3_res_init();
                    res = mp3_res_play(addr, len);
                #endif  //(TONE_PLAY_MP3_DEC_EN)
            } break;
        case TONE_TYPE_WAV: {
                #if (TONE_PLAY_WAV_DEC_EN)
                    wav_res_init();
                    res = wav_res_play(addr, len);
                #endif  //(TONE_PLAY_WAV_DEC_EN)
            } break;

        default:
            break;
    }

    bsp_tone_play_output_mute(0);

    if (res == 0) {
        printf("!!tone play err!!\n");
    }
}

void bsp_tone_play_proc_do(SDDAC_IT_TYPEDEF done_type)
{
    uint8_t finish = 0;
    WDT_CLR();
    switch (tone_play_cb.type)
    {
        case TONE_TYPE_SIN: {
                #if (TONE_PLAY_SIN_DEC_EN)
                    finish = sin_res_play_proc(done_type);
                #endif  //(TONE_PLAY_SIN_DEC_EN)
            } break;
        case TONE_TYPE_MP3: {
                #if (TONE_PLAY_MP3_DEC_EN)
                    finish = mp3_res_play_proc(done_type);
                #endif  //(TONE_PLAY_MP3_DEC_EN)
            } break;
        case TONE_TYPE_WAV: {
                #if (TONE_PLAY_WAV_DEC_EN)
                    finish = wav_res_play_proc(done_type);
                #endif  //(TONE_PLAY_WAV_DEC_EN)
            } break;

        default:
            break;
    }

    if (finish == 1) {
        printf("tone play end!\n");
        bsp_tone_play_stop();
        if (tone_play_cb.sys_clk_rec != 0) {
            sys_clk_set(tone_play_cb.sys_clk_rec);  //恢复时钟
        }
        #if (SYS_SLEEP_TIME)
            lowpwr_sleep_auto_en();
        #endif //SYS_SLEEP_TIME
    }
}

void bsp_tone_play_process(void)
{
    if (sddac_get_flag(SDDAC_IT_ALL_DONE) != RESET) {
        sddac_clear_flag(SDDAC_IT_ALL_DONE);
        bsp_tone_play_proc_do(SDDAC_IT_ALL_DONE);
    } else if (sddac_get_flag(SDDAC_IT_HALF_DONE) != RESET) {
        sddac_clear_flag(SDDAC_IT_HALF_DONE);
        bsp_tone_play_proc_do(SDDAC_IT_HALF_DONE);
    }
}

#endif  //(TONE_PLAY_EN)
