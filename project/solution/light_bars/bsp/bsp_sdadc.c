#include "include.h"
#include "driver_sdadc.h"
#include "thr_track_flash.h"

#if (BSP_MIC_EN)

#define SDADC_DMA_SIZE  256

AT(.ble_buf.sdadc)
uint16_t sdadc_buf[SDADC_DMA_SIZE];
static volatile s32 mic_power;
static volatile bool drumbeat_flag = 0;  //鼓点标志

#if THR_TRACK_FLASH_EN    //输出鼓点标志（新算法，效果较好）
bool drumbeat_flag_get(void)
{
    return drumbeat_flag;
}

//用户一直获取鼓点标志，flag为1则执行相应闪灯功能，默认100ms刷新一次flag
void sdadc_mic_ch_drumbeat_proc(void)
{
    if (drumbeat_flag) {
        printf("drumbeat\n");
        drumbeat_flag = 0;
    }
}

AT(.com_text.mic.power)
void sdadc_done_mic_cb(SDADC_DONE_TYPEDEF type)
{
    if(type == SDADC_ALL_DONE){
        void *mic_power_ptr = &sdadc_buf[SDADC_DMA_SIZE / 2];
        drumbeat_flag = thr_track_flash_process(mic_power_ptr);
    }

    if(type == SDADC_HALF_DONE){
        void *mic_power_ptr = sdadc_buf;
        drumbeat_flag = thr_track_flash_process(mic_power_ptr);
    }
}

#else    //输出mic power
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
void sdadc_done_mic_cb(SDADC_DONE_TYPEDEF type)
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
#endif  //THR_TRACK_FLASH_EN

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

void bsp_sdadc_mic_init(void)
{
    sdadc_init_typedef sdadc_cfg_init;

    sdadc_done_callback_set(sdadc_done_mic_cb);
    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);
    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);

    sdadc_cfg_init.sample_rate = SPR_8000;
    sdadc_cfg_init.mic_analog_cfg = MIC_ANALOG_CFG_NORMAL;
    sdadc_cfg_init.subtract_soft_dc_en = ENABLE;
    sdadc_cfg_init.get_dc_en = ENABLE;
    sdadc_cfg_init.remove_dc_en = ENABLE;
    sdadc_cfg_init.dig_gain_en = ENABLE;
    sdadc_init(&sdadc_cfg_init);

#if THR_TRACK_FLASH_EN
    thr_track_flash_init();
#endif  //THR_TRACK_FLASH_EN

    sdadc_pic_config(sdadc_isr, 0, SDADC_FLAG_DMA_HALF_DONE | SDADC_FLAG_DMA_ALL_DONE, ENABLE);
    sdadc_dma_cmd((u32)sdadc_buf, SDADC_DMA_SIZE - 1, ENABLE);
    sdadc_cmd(ENABLE);

    sdadc_digital_gain_set(DIG_GAIN_3200);
    mic_analog_gain_set(5);
    mic_bias_level_set(7);
}

#endif //#if (BSP_MIC_EN)

