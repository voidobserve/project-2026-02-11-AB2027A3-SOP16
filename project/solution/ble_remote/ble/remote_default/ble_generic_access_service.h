#ifndef __BLE_GENERIC_ACCESS_SERVICE_H
#define __BLE_GENERIC_ACCESS_SERVICE_H
#include "include.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
#define BLE_GENERIC_ACCESS_CMD_BUF_LEN            4
#define BLE_GENERIC_ACCESS_CMD_BUF_MASK           (BLE_GENERIC_ACCESS_CMD_BUF_LEN - 1)
#define BLE_GENERIC_ACCESS_RX_BUF_LEN             20

#define BLE_GENERIC_ACCESS_NOTIFY_DATA_BUF_LEN    8

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_GENERIC_ACCESS_RX_BUF_LEN];
    uint16_t handle;
}ble_generic_access_rx_cmd_t;

typedef struct{
    ble_generic_access_rx_cmd_t cmd[BLE_GENERIC_ACCESS_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}ble_generic_access_rx_cb_t;

//TX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_GENERIC_ACCESS_NOTIFY_DATA_BUF_LEN];
    uint16_t handle;
}ble_generic_access_tx_t;

typedef struct{
}ble_generic_access_var_t;

void ble_generic_access_service_init(void);
void ble_generic_access_service_proc(void);
#endif  //LE_REOMTE_PRODUCT_ID

#endif
