#include "include.h"
#include "utils_ring_buffer.h"
#include "driver_sdadc.h"
#include "bsp_sdadc_voice.h"
#include "driver_gpio.h"
#if (AINS3_EN)
#include "ains3.h"
#elif (AINS4_EN)
#include "ains4.h"
#endif

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
//MSBC
#define SDADC_OUTPUT_SIZE   240  //DMA样点数 HALF DONE + ALL DONE
#define ENC_INPUT_SIZE      120  //压缩前样点数
#define ENC_OUTPUT_SIZE     57   //压缩后byte数
#define SDADC_SPR           SPR_16000

#elif (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
/*********************************************************************************************************
* IMA ADPCM编码需要多一个样点，该样点赋值到adpcm音频帧的前两byte，也就是presample。
* 如下例程，实际是512byte压缩成128byte(4:1)，presample不做压缩。
* 同理，如果用我们的IMA ADPCM解码，那么输出buf为(512+2)byte、输入buf(4+128)byte、解码样点数(256+1)
**********************************************************************************************************/
#define SDADC_OUTPUT_SIZE   (512)         //DMA样点数 HALF DONE + ALL DONE
#define ENC_INPUT_SIZE      (256+1)       //压缩前样点数
#define ENC_OUTPUT_SIZE     (4+128)       //压缩后byte数，2byte presample + 1byte index + 1byte reserve + 128byte 偏差值
#define SDADC_SPR           SPR_8000
#endif

#if (AINS3_EN || AINS4_EN)  //ains算法需要丢弃mic刚启动时的不稳定数据
#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
#define SDADC_SKIP_FRAME    20  //150ms
#else
#define SDADC_SKIP_FRAME    5
#endif //(LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
#else
#define SDADC_SKIP_FRAME    0
#endif //(AINS3_EN || AINS4_EN)

#define SDADC_BUF_CNT       4    //pcm缓存包数

static bool sdadc_start_flag = 0;
static uint8_t sdadc_skip_frame = SDADC_SKIP_FRAME;

extern void user_enc_buf_process(uint8_t *buffer, uint32_t size);

uint16_t sdadc_output_buf[SDADC_OUTPUT_SIZE] AT(.ble_voice.sdadc.obuf);
uint8_t enc_output_buf[ENC_OUTPUT_SIZE] AT(.ble_voice.enc_obuf);

ring_buf_t sdadc_pcm_ring_buf AT(.ble_voice.ring_buf);
uint8_t sdadc_pcm_buf[ENC_INPUT_SIZE * 2 * SDADC_BUF_CNT] AT(.ble_voice.ring_buf);

AT(.com_text.sdadc.proc)
void bsp_sdadc_voice_process(SDADC_DONE_TYPEDEF type)
{
    uint8_t *enc_iptr;
    int16_t *dac_optr;

    if (sdadc_skip_frame) {
        sdadc_skip_frame--;
        return;
    }

    if (type == SDADC_ALL_DONE) {
        dac_optr = (int16_t *)&sdadc_output_buf[SDADC_OUTPUT_SIZE/2];
    } else {
        dac_optr = (int16_t *)&sdadc_output_buf[0];
    }

#if (AINS3_EN)
    ains3_process(dac_optr);
    ains3_process(dac_optr+AINS3_PART_LEN);  //msbc (AINS3_PART_LEN is 60), adpcm (AINS3_PART_LEN is 64)
    #if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
        ains3_process(dac_optr+AINS3_PART_LEN+AINS3_PART_LEN);
        ains3_process(dac_optr+AINS3_PART_LEN+AINS3_PART_LEN+AINS3_PART_LEN);  //ADPCM 256 points
    #endif
#elif (AINS4_EN)
    ains4_process(dac_optr);
    ains4_process(dac_optr+AINS4_PART_LEN);  //msbc (AINS4_PART_LEN is 60), adpcm (AINS4_PART_LEN is 64)
    #if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
        ains4_process(dac_optr+AINS4_PART_LEN+AINS4_PART_LEN);
        ains4_process(dac_optr+AINS4_PART_LEN+AINS4_PART_LEN+AINS4_PART_LEN);  //ADPCM 256 points
    #endif
#endif

    utils_ring_buf_write(&sdadc_pcm_ring_buf, (u8*)dac_optr, sizeof(sdadc_output_buf)/2);

    if (utils_ring_buf_data_len_get(&sdadc_pcm_ring_buf) < ENC_INPUT_SIZE*2) {
        return;
    } else {
        enc_iptr = utils_ring_buf_get(&sdadc_pcm_ring_buf, ENC_INPUT_SIZE*2);
    }

    //print_r(enc_iptr, ENC_INPUT_SIZE*2);  //建议使用6M以上的串口波特率，避免数据被覆盖问题

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
    msbc_encode_frame(enc_iptr, ENC_INPUT_SIZE*2, (uint8_t *)enc_output_buf, ENC_OUTPUT_SIZE);
#elif (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
    adpcm_encode_block_big((uint8_t *)enc_output_buf, enc_iptr, ENC_INPUT_SIZE);
#endif

    user_enc_buf_process((uint8_t *)enc_output_buf, ENC_OUTPUT_SIZE);
}

AT(.com_text.sdadc.isr)
void bsp_sdadc_voice_isr(void)
{
    if (sdadc_get_flag(SDADC_FLAG_DMA_ALL_DONE) != RESET) {
        sdadc_done_proc_kick(SDADC_ALL_DONE);
        sdadc_clear_flag(SDADC_FLAG_DMA_ALL_DONE);
    } else if (sdadc_get_flag(SDADC_FLAG_DMA_HALF_DONE) != RESET) {
        sdadc_done_proc_kick(SDADC_HALF_DONE);
        sdadc_clear_flag(SDADC_FLAG_DMA_HALF_DONE);
    }
}

void bsp_sdadc_voice_init(void)
{
    sdadc_init_typedef sdadc_cfg_init;

    utils_ring_buf_init(&sdadc_pcm_ring_buf, sdadc_pcm_buf, sizeof(sdadc_pcm_buf));

    sdadc_done_callback_set(bsp_sdadc_voice_process);
    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);
    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    sdadc_cfg_init.sample_rate = SDADC_SPR;
    sdadc_cfg_init.mic_analog_cfg = MIC_ANALOG_CFG_NORMAL;
    sdadc_cfg_init.subtract_soft_dc_en = ENABLE;
    sdadc_cfg_init.get_dc_en = ENABLE;
    sdadc_cfg_init.remove_dc_en = ENABLE;
    sdadc_cfg_init.dig_gain_en = ENABLE;
    sdadc_init(&sdadc_cfg_init);
    sdadc_pic_config(bsp_sdadc_voice_isr, 0, SDADC_FLAG_DMA_HALF_DONE | SDADC_FLAG_DMA_ALL_DONE, ENABLE);
    sdadc_digital_gain_set(DIG_GAIN_3200); //输入增益
    mic_analog_gain_set(MIC_ANA_GAIN);
}

void bsp_sdadc_voice_start(void)
{
    sdadc_start_flag = 1;
    sdadc_skip_frame = SDADC_SKIP_FRAME;  //丢弃mic刚启动时的不稳定数据

#if AINS3_EN
    ains3_init();
#elif AINS4_EN
    ains4_init();
#endif

    bsp_sdadc_voice_init();

#if MIC_BIAS_EN
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = GPIO_PIN_1;  //only
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(GPIOB_REG, &gpio_init_structure);
    mic_bias_open(MIC_BIAS_LEVEL, MIC_BIAS_RES);
#endif  //MIC_BIAS_EN
    sdadc_dma_cmd((u32)sdadc_output_buf, SDADC_OUTPUT_SIZE - 1, ENABLE);
    sdadc_cmd(ENABLE);
}

void bsp_sdadc_voice_stop(void)
{
    if (sdadc_start_flag == 0) {
        return;
    } else {
        sdadc_start_flag  = 0;
    }

#if AINS3_EN
    ains3_exit();
#elif AINS4_EN
    ains4_exit();
#endif

#if MIC_BIAS_EN
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = GPIO_PIN_1;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(GPIOB_REG, &gpio_init_structure);
    mic_bias_close();
#endif  //MIC_BIAS_EN
    sdadc_dma_cmd(0, 0, DISABLE);
    sdadc_cmd(DISABLE);

    sdadc_deinit();
}
