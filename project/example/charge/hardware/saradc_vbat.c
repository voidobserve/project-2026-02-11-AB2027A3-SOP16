#include "driver_saradc.h"
#include "saradc_vbat.h"

// 电池电量相关参数
vbat_param_struct vbat_param = {
    .vbat_max_level = 100,
    .vbat_min_power = 2500,
    .vbat_max_power = 4200,
};

void vbat_detect_init(void)
{
    saradc_base_init_typedef saradc_base_init_struct;
    saradc_channel_init_typedef saradc_channel_init_struct;

    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_EN);

    saradc_base_init_struct.mode = SARADC_MODE_NORMAL;
    saradc_base_init_struct.baud = 200000;
    saradc_base_init_struct.auto_analog = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_base_init_struct.auto_analog_io = SARADC_AUTO_ENABLE_ANALOG_IO_EN;

    saradc_init(&saradc_base_init_struct);

    saradc_channel_init_struct.channel = ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
    saradc_channel_init_struct.interrupt_en = DISABLE;

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_cmd(ENABLE);
    saradc_kick_start(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL);
}

// 获取电池电量： 1：电量adc值(mA)  0：电量水平
uint16_t get_vbat_level(u8 mode)
{
    u8 i = 0, loop = 5;
    u32 adc_val, adc_val_total = 0;
    u32 adc_vbg, adc_vbg_total = 0;
    u32 voltage;
    u8 current_power;

    while (i < loop) {
        saradc_kick_start(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL);
        while (!saradc_get_channel_flag(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL));
        adc_val_total += saradc_get_data(ADC_CHANNEL_VBAT);
        adc_vbg_total += saradc_get_data(VBAT_DETECT_ASSIST_CHANNEL);

        i++;
    }
    adc_val = adc_val_total / loop;
    adc_vbg = adc_vbg_total / loop;
    voltage = adc_val * VBAT_DETECT_ASSIST_VOLTAGE / adc_vbg;

    if (mode == 0) {
        if (vbat_param.vbat_min_power > voltage) {
            return 0;
        }
        current_power = vbat_param.vbat_max_level * \
        (voltage - vbat_param.vbat_min_power) / (vbat_param.vbat_max_power - vbat_param.vbat_min_power);
        return (current_power > vbat_param.vbat_max_level? vbat_param.vbat_max_level: current_power);
    } else {
        return voltage;
    }
}
