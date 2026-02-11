#ifndef _BSP_SARADC_GSENSOR_H
#define _BSP_SARADC_GSENSOR_H


#define GSENSOR_ADC_CHANNEL      ADC_CHANNEL_PA5

void bsp_adc_gsensor_init(void);
void bsp_adc_gsensor_process(void);


#endif // _BSP_SARADC_GSENSOR_H