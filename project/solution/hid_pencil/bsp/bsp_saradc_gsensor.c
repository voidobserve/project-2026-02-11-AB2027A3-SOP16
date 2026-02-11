#include "include.h"
#include "driver_saradc.h"
#include "bsp_saradc_gsensor.h"

#if BSP_GSENSOR_DETECT_EN

void bsp_adc_gsensor_init(void)
{
    saradc_channel_init_typedef saradc_channel_init_struct;

    saradc_channel_init_struct.channel = GSENSOR_ADC_CHANNEL;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
    saradc_channel_init_struct.interrupt_en = DISABLE;

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_kick_start(GSENSOR_ADC_CHANNEL);
}

void bsp_adc_gsensor_process(void)
{
    static u32 delay_cnt;
    u32 adc_val;

    if (tick_check_expire(delay_cnt, 100)) {
        if (saradc_get_channel_flag(GSENSOR_ADC_CHANNEL)) {
            adc_val = saradc_get_data(GSENSOR_ADC_CHANNEL);

            printf("voltage = %d\n", adc_val);

            saradc_kick_start(GSENSOR_ADC_CHANNEL);
        }

        delay_cnt = tick_get();
    }
}

#endif // BSP_GSENSOR_DETECT_EN
