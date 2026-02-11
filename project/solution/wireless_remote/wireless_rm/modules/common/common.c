#include "include.h"
#include "wireless.h"

void remote_hid_msg_send(KEY_MSG_TYPEDEF key_msg)
{
    if (ble_get_work_mode() == MODE_WIRELESS) {
        wl_hid_msg_notify(key_msg);
    } else if (ble_get_work_mode() == MODE_NORMAL) {
        // ble_hid_msg_notify();
    }
}

void remote_voice_no_msg_send(void)
{
    if (ble_get_work_mode() == MODE_WIRELESS) {
        wl_voice_start();
    } else if (ble_get_work_mode() == MODE_NORMAL) {
        // ble_voice_start();
    }
}

void remote_voice_off_msg_send(void)
{
    if (ble_get_work_mode() == MODE_WIRELESS) {
        wl_voice_stop();
    } else if (ble_get_work_mode() == MODE_NORMAL) {
        // ble_voice_stop();
    }
}
