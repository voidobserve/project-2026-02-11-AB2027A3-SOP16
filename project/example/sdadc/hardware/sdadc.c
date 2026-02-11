#include "include.h"
#include "driver_sdadc.h"
#include "driver_gpio.h"

//测试选项
#define SDADC_TEST_MIC_POWER            1           //sdadc采集计算能量
#define SDADC_TEST_DATA_DUMP            0           //sdadc采集后通过串口打印pcm数据

#define SDADC_GAIN_ADD_PORT             GPIOA_REG
#define SDADC_GAIN_ADD_PIN              GPIO_PIN_7
#define SDADC_GAIN_DEC_PORT             GPIOA_REG
#define SDADC_GAIN_DEC_PIN              GPIO_PIN_6

#define SDADC_DMA_SIZE  48
#define GAIN_LIST_LEN   7

uint16_t sdadc_buf[SDADC_DMA_SIZE] AT(.com_text.sdadc_buf);
static s32 mic_power;

u16 gain_list[GAIN_LIST_LEN] = {
    DIG_GAIN_400,
    DIG_GAIN_800,
    DIG_GAIN_1600,
    DIG_GAIN_3200,
    DIG_GAIN_4800,
    DIG_GAIN_6400,
    DIG_GAIN_8000,
};

AT(.com_text.sdadc.isr)
void sdadc_isr(void)
{
    if (sdadc_get_flag(SDADC_FLAG_DMA_ALL_DONE) != RESET) {
        sdadc_done_proc_kick(SDADC_ALL_DONE);
        sdadc_clear_flag(SDADC_FLAG_DMA_ALL_DONE);
    }

    if (sdadc_get_flag(SDADC_FLAG_DMA_HALF_DONE) != RESET) {
        sdadc_done_proc_kick(SDADC_HALF_DONE);
        sdadc_clear_flag(SDADC_FLAG_DMA_HALF_DONE);
    }
}

#if SDADC_TEST_MIC_POWER
AT(.com_text.mic.power)
u16 mic_power_maxpow(void *ptr, u16 len)
{
    s16 *temp = (s16*)ptr;
    s16 max = temp[0], min = temp[0];
    len = len / 2;
    for(u16 i=0;i<len;i++){
        if(temp[i] > max){
            max = temp[i];
        }
        if(temp[i] < min){
            min = temp[i];
        }
    }
    return(max - min);
}

s32 mic_power_get(void)
{
    return mic_power;
}

AT(.com_text.mic.power)
void sdadc_mic_power(SDADC_DONE_TYPEDEF type)
{
    if(type == SDADC_ALL_DONE){
        void *mic_power_ptr = &sdadc_buf[SDADC_DMA_SIZE / 2];
        u16 sdadc_buf_len = sizeof(sdadc_buf)/2;
        mic_power = mic_power_maxpow(mic_power_ptr,sdadc_buf_len);
    }

    if(type == SDADC_HALF_DONE){
        void *mic_power_ptr = sdadc_buf;
        u16 sdadc_buf_len = sizeof(sdadc_buf)/2;
        mic_power = mic_power_maxpow(mic_power_ptr,sdadc_buf_len);
    }
}
#else
AT(.com_text.mic.power)
void sdadc_done_callback_func(SDADC_DONE_TYPEDEF type)
{
    u8 *ptr;
    u16 sdadc_buf_len = sizeof(sdadc_buf)/2;

    if(type == SDADC_ALL_DONE){
        ptr = (u8*)&sdadc_buf[SDADC_DMA_SIZE / 2];
    }

    if(type == SDADC_HALF_DONE){
        ptr = (u8*)sdadc_buf;
    }

#if SDADC_TEST_DATA_DUMP
    for(uint16_t i=0; i<sdadc_buf_len; i++){
        uart_putchar(*(ptr+i));
    }
#endif
}
#endif

void sdadc_example_init(void)
{
    sdadc_init_typedef sdadc_cfg_init;
    gpio_init_typedef gpio_init_structure;

#if SDADC_TEST_MIC_POWER
    sdadc_done_callback_set(sdadc_mic_power);
#else
    sdadc_done_callback_set(sdadc_done_callback_func);
#endif

    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);
    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);

    /* KEY[gain+] */
    gpio_init_structure.gpio_pin = SDADC_GAIN_ADD_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(SDADC_GAIN_ADD_PORT, &gpio_init_structure);

    /* KEY[gain-] */
    gpio_init_structure.gpio_pin = SDADC_GAIN_DEC_PIN;
    gpio_init(SDADC_GAIN_DEC_PORT, &gpio_init_structure);

    sdadc_cfg_init.sample_rate = SPR_16000;
    sdadc_cfg_init.mic_analog_cfg = MIC_ANALOG_CFG_NORMAL;
    sdadc_cfg_init.subtract_soft_dc_en = ENABLE;
    sdadc_cfg_init.get_dc_en = ENABLE;
    sdadc_cfg_init.remove_dc_en = ENABLE;
    sdadc_cfg_init.dig_gain_en = ENABLE;
    sdadc_init(&sdadc_cfg_init);

    sdadc_pic_config(sdadc_isr, 0, SDADC_FLAG_DMA_HALF_DONE | SDADC_FLAG_DMA_ALL_DONE, ENABLE);
    sdadc_dma_cmd((u32)sdadc_buf, SDADC_DMA_SIZE - 1, ENABLE);
    sdadc_cmd(ENABLE);

    sdadc_digital_gain_set(DIG_GAIN_3200);
    mic_analog_gain_set(5);
    mic_bias_level_set(7);
}

void sdadc_example_process(void)
{
    static u32 delay_cnt;
    static u8 key_down_gain_dec = 0;
    static u8 key_down_gain_add = 0;
    static u8 gain_idx = 0;

#if SDADC_TEST_MIC_POWER
    if (tick_check_expire(delay_cnt, 200)) {
        delay_cnt = tick_get();
        /*
         * The number of correlations of mic volume,
         * the lounder the volume, the lager the value.
         */
        printf("mic_power: %d\n", mic_power_get());
    }
#endif

    if (gpio_read_bit(SDADC_GAIN_DEC_PORT, SDADC_GAIN_DEC_PIN) == RESET) {
        if (key_down_gain_dec == 0) {
            key_down_gain_dec = 1;
            gain_idx--;
            if (gain_idx >= GAIN_LIST_LEN) {
                gain_idx = GAIN_LIST_LEN - 1;
            }
            sdadc_digital_gain_set(gain_list[gain_idx]);
            printf("set gain idx: %d\n", gain_idx);
        }
    } else {
        key_down_gain_dec = 0;
    }

    if (gpio_read_bit(SDADC_GAIN_ADD_PORT, SDADC_GAIN_ADD_PIN) == RESET) {
        if (key_down_gain_add == 0) {
            key_down_gain_add = 1;
            gain_idx = (gain_idx + 1) % GAIN_LIST_LEN;
            sdadc_digital_gain_set(gain_list[gain_idx]);
            printf("set gain idx: %d\n", gain_idx);
        }
    } else {
        key_down_gain_add = 0;
    }
}
