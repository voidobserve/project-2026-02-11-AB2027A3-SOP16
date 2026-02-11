#ifndef __BLE_PROC_H
#define __BLE_PROC_H

typedef enum {
    BLE_KEY_MSG_RELEASE             = 0x00,  //按键抬起
    BLE_KEY_MSG_UP                  = 0x01,
    BLE_KEY_MSG_DOWN                = 0x02,
    BLE_KEY_MSG_LEFT                = 0x03,
    BLE_KEY_MSG_RIGHT               = 0x04,
    BLE_KEY_MSG_MEUN                = 0x05,
    BLE_KEY_MSG_HOME                = 0x06,
    BLE_KEY_MSG_OK                  = 0x07,
    BLE_KEY_MSG_BACK                = 0x08,
    BLE_KEY_MSG_VOL_UP              = 0x09,
    BLE_KEY_MSG_VOL_DOWN            = 0x0a,
    BLE_KEY_MSG_POWER               = 0x0b,
    BLE_KEY_MSG_VOICE_ON            = 0x0c,  //开始语音
    BLE_KEY_MSG_VOICE_OFF           = 0x0d,  //结束语音
    BLE_KEY_MSG_PAIR                = 0x0e,
    
    BLE_KEY_MSG_NONE                = 0xff,
} BLE_KEY_MSG_TYPEDEF;  //范围：0x00~0xff

void ble_latency_en_dly_tick_init(void);
void ble_user_set_service_pending(void);
void ble_app_proc(void);
void ble_enter_sleep_process(void);
void ble_exit_sleep_process(void);
void ble_set_key_msg(uint8_t msg);
uint8_t ble_get_key_msg(void);
bool ble_proc_pending(void);

#endif
