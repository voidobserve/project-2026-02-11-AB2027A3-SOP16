#include "include.h"
#include "driver_sdadc.h"
#include "bsp_sdadc.h"
#include "usb_audio.h"

#if USB_MIC_EN

#define SDADC_DMA_SIZE              16

uint16_t sdadc_buf[SDADC_DMA_SIZE] AT(.buf.sdadc_buf);


AT(.com_text.isr.sdadc)
void sdadc_isr(void)
{
    if (sdadc_get_flag(SDADC_FLAG_DMA_ALL_DONE) != RESET) {
        sdadc_done_proc_kick(SDADC_ALL_DONE);
        sdadc_clear_flag(SDADC_FLAG_DMA_ALL_DONE);
    }
}

AT(.com_text.thread.aupcm)
void sdadc_done_callback_func(SDADC_DONE_TYPEDEF type)
{
    if(type == SDADC_ALL_DONE){
        usb_audio_mic_stream(sdadc_buf, SDADC_DMA_SIZE);
    }
}

void bsp_sdadc_init(void)
{
    sdadc_init_typedef sdadc_cfg_init;

    sdadc_done_callback_set(sdadc_done_callback_func);

    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);
    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);

    sdadc_cfg_init.sample_rate = SPR_16000;
    sdadc_cfg_init.mic_analog_cfg = MIC_ANALOG_CFG_NORMAL;
    sdadc_cfg_init.subtract_soft_dc_en = ENABLE;
    sdadc_cfg_init.get_dc_en = ENABLE;
    sdadc_cfg_init.remove_dc_en = ENABLE;
    sdadc_cfg_init.dig_gain_en = ENABLE;
    sdadc_init(&sdadc_cfg_init);

    sdadc_pic_config(sdadc_isr, 0, SDADC_FLAG_DMA_ALL_DONE, ENABLE);
    sdadc_dma_cmd((u32)sdadc_buf, SDADC_DMA_SIZE - 1, ENABLE);

    // sdadc_cmd(ENABLE);

    sdadc_digital_gain_set(DIG_GAIN_8000);
    mic_analog_gain_set(5);
    mic_bias_level_set(7);
}
#endif
