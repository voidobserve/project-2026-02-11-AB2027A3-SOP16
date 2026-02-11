#include "include.h"
#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "saradc_sampling.h"


AT(.com_rodata.saradc_test)
const char saradc_info[] = "[interrupt] voltage: %d\n";

#if INTERRUPT_EN
AT(.com_text.isr)
static void saradc_isr_handler(void)
{
    u32 adc_val;
    u32 adc_vbg;
    u32 voltage;

    if (saradc_get_flag(SARADC_IT_FINISHED)) {
        adc_val = saradc_get_data(ADC_CHANNEL);
        adc_vbg = saradc_get_data(ADC_CHANNEL_VRTC);

        voltage = adc_val * ASSIST_VOLTAGE / adc_vbg;
        printf(saradc_info, voltage);

        saradc_clear_flag(SARADC_IT_FINISHED);
        saradc_kick_start(ADC_CHANNEL | ADC_CHANNEL_VRTC);
    }
}
#endif // INTERRUPT_EN

void saradc_sampling_init(void)
{
    saradc_base_init_typedef saradc_base_init_struct;
    saradc_channel_init_typedef saradc_channel_init_struct;

    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_EN);

    saradc_base_init_struct.mode = SARADC_MODE_NORMAL;
    saradc_base_init_struct.baud = 50000;
    saradc_base_init_struct.auto_analog = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_base_init_struct.auto_analog_io = SARADC_AUTO_ENABLE_ANALOG_IO_EN;

    saradc_init(&saradc_base_init_struct);

#if INTERRUPT_EN
    saradc_pic_config(saradc_isr_handler, 0, SARADC_IT_FINISHED, ENABLE);
#endif // INTERRUPT_EN

    saradc_channel_init_struct.channel = ADC_CHANNEL | ADC_CHANNEL_VRTC;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
#if INTERRUPT_EN
    saradc_channel_init_struct.interrupt_en = ENABLE;
#else
    saradc_channel_init_struct.interrupt_en = DISABLE;
#endif // INTERRUPT_EN

    if (saradc_channel_init_struct.channel & ADC_CHANNEL_VUSB) {    
        clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);        // use for VUSB channel
    }

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_cmd(ENABLE);
    saradc_kick_start(ADC_CHANNEL | ADC_CHANNEL_VRTC);
}

void saradc_sampling_run(void)
{
    static u32 delay_cnt;

#if !INTERRUPT_EN
    u32 adc_val;
    u32 adc_vbg;
    u32 voltage;

    if (tick_check_expire(delay_cnt, 500)) {
        if (saradc_get_channel_flag(ADC_CHANNEL | ADC_CHANNEL_VRTC)) {
            adc_val = saradc_get_data(ADC_CHANNEL);
            adc_vbg = saradc_get_data(ADC_CHANNEL_VRTC);

            voltage = adc_val * ASSIST_VOLTAGE / adc_vbg;
            printf("voltage = %d\n", voltage);

            saradc_kick_start(ADC_CHANNEL | ADC_CHANNEL_VRTC);
        }

        delay_cnt = tick_get();
    }
#endif // !INTERRUPT_EN
}
