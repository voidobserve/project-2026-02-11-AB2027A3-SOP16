#ifndef __WIRELESS_ADV_H
#define __WIRELESS_ADV_H
#include "include.h"

typedef enum {
    WL_RM_ADV_PKT_DEFAULT = 0,
    WL_RM_ADV_PKT_POWERON,
    WL_RM_ADV_PKT_PAIR,
} WL_RM_ADV_PKT;

uint8_t wl_get_adv_data(u8 *adv_buf, WL_RM_ADV_PKT adv_pkt);
void wl_adv_init(WL_RM_ADV_PKT adv_pkt);
void wl_adv_ctl_set(uint8_t key_msg);
void wl_adv_proc(void);
#endif  //__WIRELESS_ADV_H
