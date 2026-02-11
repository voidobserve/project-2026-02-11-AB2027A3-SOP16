#ifndef __BLE_VOICE_SERVICE_H
#define __BLE_VOICE_SERVICE_H
#include "include.h"
#include "ble_service.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)

//按下语音按键
//66 Notify(Report)
//[ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//52 Notify(HID Report)
//[ 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00
//63 Write Request
//[ 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//开始发送语音
//70(60byte)
//74(60byte)
//78(60byte)
//70(60byte)
//74(60byte)
//78(60byte)
//松开按键
//66 Notify(Report)
//[ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//52 Notify(HID Report)
//[ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//63 Write Request
//[ 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#define BLE_VOICE_CMD_BUF_LEN            4
#define BLE_VOICE_CMD_BUF_MASK           (BLE_VOICE_CMD_BUF_LEN - 1)
#define BLE_VOICE_RX_BUF_LEN             20

#define BLE_VOICE_NOTIFY_DATA_BUF_LEN        60 //发语音
#define BLE_VOICE_NOTIFY_CTL_BUF_LEN         20 //发语音使能控制

typedef enum {
    VOICE_STATUS_STOP          = 0x00,
    VOICE_STATUS_START         = 0x01,
} VOICE_STATUS_TYPEDEF;

typedef enum {
    BLE_VOICE_DISABLE_CMD      = 0x00,
    BLE_VOICE_ENABLE_CMD       = 0x01,
} VOICE_CMD_TYPEDEF;

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_VOICE_RX_BUF_LEN];
    uint16_t handle;
}ble_voice_rx_cmd_t;

typedef struct{
    ble_voice_rx_cmd_t cmd[BLE_VOICE_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}ble_voice_rx_cb_t;

//TX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_VOICE_NOTIFY_DATA_BUF_LEN];
    uint16_t handle;
}ble_voice_tx_t;

typedef struct{
    uint8_t voice_status;
    uint8_t exit_clk;
}ble_voice_var_t;

void ble_voice_service_init(void);
void ble_voice_service_proc(void);
#endif // LE_REOMTE_PRODUCT_ID

#endif