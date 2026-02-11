#ifndef _SARADC_VBAT_H
#define _SARADC_VBAT_H

#include "include.h"

#define VBAT_DETECT_ASSIST_CHANNEL      ADC_CHANNEL_VRTC
#define VBAT_DETECT_ASSIST_VOLTAGE      pmu_get_vrtc24_mv()

typedef struct {
    u8  vbat_max_level;
    u16 vbat_min_power;
    u16 vbat_max_power;
} vbat_param_struct;

void vbat_detect_init(void);
uint16_t get_vbat_level(u8 mode);

#endif // _SARADC_VBAT_H
