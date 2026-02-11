#include "include.h"
#include "wireless.h"

// typedef struct PACKED {
//     struct {
//         uint8_t     left_ctrl   : 1;
//         uint8_t     left_shift  : 1;
//         uint8_t     left_alt    : 1;
//         uint8_t     left_gui    : 1;
//         uint8_t     rigth_ctrl  : 1;
//         uint8_t     rigth_shift : 1;
//         uint8_t     rigth_alt   : 1;
//         uint8_t     rigth_gui   : 1;
//     };
//     uint8_t         rsvd0;
//     uint8_t         key[6];
// } hid_rep_keyboard_t;

AT(.text.prod_test.wl_hid_ntf)
int16_t wl_hid_msg_notify(KEY_MSG_TYPEDEF key_msg)
{
    bool msg_valid = true;
    uint8_t hid_buf[WL_HID_CMD_LEN];
    int16_t ret = 0;

    memset(hid_buf, 0, WL_HID_CMD_LEN);

    switch (key_msg) {
        case KEY_MSG_RELEASE:
            hid_buf[4] = WL_HID_RELEASE_CMD;
            break;
        case KEY_MSG_UP:
            hid_buf[4] = WL_HID_UP_CMD;
            break;
        case KEY_MSG_DOWN:
            hid_buf[4] = WL_HID_DOWN_CMD;
            break;
        case KEY_MSG_LEFT:
            hid_buf[4] = WL_HID_LEFT_CMD;
            break;
        case KEY_MSG_RIGHT:
            hid_buf[4] = WL_HID_RIGHT_CMD;
            break;
        case KEY_MSG_MEUN:
            hid_buf[4] = WL_HID_MEUN_CMD;
            break;
        case KEY_MSG_HOME:
            hid_buf[4] = WL_HID_HOME_CMD;
            break;
        case KEY_MSG_OK:
            hid_buf[4] = WL_HID_OK_CMD;
            break;
        case KEY_MSG_BACK:
            hid_buf[4] = WL_HID_BACK_CMD;
            break;
        case KEY_MSG_VOL_UP:
            hid_buf[4] = WL_HID_VOL_UP_CMD;
            break;
        case KEY_MSG_VOL_DOWN:
            hid_buf[4] = WL_HID_VOL_DOWN_CMD;
            break;
        case KEY_MSG_POWER:
            hid_buf[4] = WL_HID_PWR_CMD;
            break;
        case KEY_MSG_VOICE_ON:
            hid_buf[4] = WL_HID_VOICE_EN_CMD;
            break;
        case KEY_MSG_VOICE_OFF:
            hid_buf[4] = WL_HID_RELEASE_CMD;
            break;

        default:
            msg_valid = false;
            break;
    }

    if (msg_valid && wireless_con_handle_get()) {
        hid_buf[0] = WIRELESS_PACKET_TYPE_HID;
        hid_buf[1] = WL_HID_CMD_PAYLOAD_LEN;
        ret = wireless_send_for_con(wireless_con_handle_get(), hid_buf, sizeof(hid_buf));
    }

    return ret;
}

AT(.text.app.proc.rm_hid)
void wl_hid_proc(void)
{

}
