#ifndef __BLE_ADV_H
#define __BLE_ADV_H
#include "include.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)

bool ble_set_poweron_adv_data(void);
u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size);
u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size, LE_REMOTE_ADV_PKT adv_pkt);
void ble_adv_ctl_set(uint8_t key_event);
void ble_adv_param_init(LE_REMOTE_ADV_PKT adv_pkt);
void ble_adv_proc(void);
#endif  //(LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
#endif  //__BLE_ADV_H