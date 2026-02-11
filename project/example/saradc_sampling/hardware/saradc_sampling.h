#ifndef _SARADC_SAMPLING_H
#define _SARADC_SAMPLING_H


#define ADC_CHANNEL         ADC_CHANNEL_PB0
#define ASSIST_VOLTAGE      pmu_get_vrtc24_mv()

#define INTERRUPT_EN        0

void saradc_sampling_init(void);
void saradc_sampling_run(void);

#endif // _SARADC_SAMPLING_H
