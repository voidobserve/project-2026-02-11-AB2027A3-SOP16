#ifndef __BLE_INIT_H
#define __BLE_INIT_H

typedef struct _ble_cb_t {
    bool pending;
    ble_con_handle_t con_handle;
} ble_cb_t;

extern ble_cb_t ble_cb;

void bsp_ble_init(void);
void ble_set_work_mode(BT_WORK_MODE mode);
BT_WORK_MODE ble_get_work_mode(void);

#endif
