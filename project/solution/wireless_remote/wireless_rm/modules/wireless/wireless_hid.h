#ifndef __WL_HID_H
#define __WL_HID_H
#include "include.h"

#define WL_HID_CMD_PAYLOAD_LEN       (8)      
#define WL_HID_CMD_HEAD_LEN          (2)   //1byte wireless packet type + 1byte cmd len
#define WL_HID_CMD_LEN               (WL_HID_CMD_HEAD_LEN + WL_HID_CMD_PAYLOAD_LEN)

#define WL_HID_RELEASE_CMD         0x00
#define WL_HID_UP_CMD              0x52
#define WL_HID_DOWN_CMD            0x51
#define WL_HID_LEFT_CMD            0x50
#define WL_HID_RIGHT_CMD           0x4F
#define WL_HID_MEUN_CMD            0x65
#define WL_HID_HOME_CMD            0x4A
#define WL_HID_OK_CMD              0x28
#define WL_HID_BACK_CMD            0xF1
#define WL_HID_VOL_UP_CMD          0x80
#define WL_HID_VOL_DOWN_CMD        0x81
#define WL_HID_PWR_CMD             0x28
#define WL_HID_VOICE_EN_CMD        0x3E

#define WL_HID_NONE_CMD            0xFF

int16_t wl_hid_msg_notify(KEY_MSG_TYPEDEF key_msg);
void wl_hid_proc(void);

#endif  //__WL_HID_H
