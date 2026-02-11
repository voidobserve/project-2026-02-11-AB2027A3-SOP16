#ifndef _BSP_SARADC_TSENSOR_H
#define _BSP_SARADC_TSENSOR_H


#define REFERENCE_VOLTAGE_CHANNEL       ADC_CHANNEL_VRTC
#define REFERENCE_VOLTAGE_VALUE         pmu_get_vrtc24_mv()

void bsp_tsensor_init(void);
void bsp_tsensor_process(void);

#endif // _BSP_SARADC_TSENSOR_H
