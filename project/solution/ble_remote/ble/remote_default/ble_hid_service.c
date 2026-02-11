#include "include.h"
#include "ble_service.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)

#define HID_SVC_DEBUG_EN 1

#if HID_SVC_DEBUG_EN
#define HID_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define HID_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_hid_service;

ble_hid_tx_t ble_hid_tx AT(.ble_hid.tx);
ble_hid_var_t ble_hid_var AT(.ble_hid.var);
static uint16_t report_id1_client_config;
static uint8_t ble_enc_done;

const uint8_t ble_hid_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x05,        //   Report Count (5)
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x05,        //   Usage Maximum (Kana)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3)
    0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x28,        //   Logical Minimum (40)
    0x25, 0xFE,        //   Logical Maximum (-2)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x28,        //   Usage Minimum (0x28)
    0x29, 0xFE,        //   Usage Maximum (0xFE)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0C,        //   Usage Page (Consumer)
    0x09, 0x01,        //   Usage (Consumer Control)
    0xA1, 0x01,        //   Collection (Application)
    0x85, 0xF1,        //     Report ID (-15)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x18,        //     Report Count (24)
    0x09, 0xB5,        //     Usage (Scan Next Track)
    0x09, 0xB6,        //     Usage (Scan Previous Track)
    0x09, 0xB7,        //     Usage (Stop)
    0x09, 0xCD,        //     Usage (Play/Pause)
    0x09, 0xE2,        //     Usage (Mute)
    0x09, 0xE5,        //     Usage (Bass Boost)
    0x09, 0xE7,        //     Usage (Loudness)
    0x09, 0xE9,        //     Usage (Volume Increment)
    0x09, 0xEA,        //     Usage (Volume Decrement)
    0x0A, 0x52, 0x01,  //     Usage (Bass Increment)
    0x0A, 0x53, 0x01,  //     Usage (Bass Decrement)
    0x0A, 0x54, 0x01,  //     Usage (Treble Increment)
    0x0A, 0x55, 0x01,  //     Usage (Treble Decrement)
    0x0A, 0x83, 0x01,  //     Usage (AL Consumer Control Configuration)
    0x0A, 0x8A, 0x01,  //     Usage (AL Email Reader)
    0x0A, 0x92, 0x01,  //     Usage (AL Calculator)
    0x0A, 0x94, 0x01,  //     Usage (AL Local Machine Browser)
    0x0A, 0x21, 0x02,  //     Usage (AC Search)
    0x0A, 0x23, 0x02,  //     Usage (AC Home)
    0x0A, 0x24, 0x02,  //     Usage (AC Back)
    0x0A, 0x25, 0x02,  //     Usage (AC Forward)
    0x0A, 0x26, 0x02,  //     Usage (AC Stop)
    0x0A, 0x27, 0x02,  //     Usage (AC Refresh)
    0x0A, 0x2A, 0x02,  //     Usage (AC Bookmarks)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

const uint8_t ble_hid_information[] = {
    0x00, 0x00,     //Hid Version
    0x00,           //Country Code
    0x01,           //feature
};

static void ble_hid_var_init(void)
{
    memset(&ble_hid_var, 0, sizeof(ble_hid_var_t));
}

static void ble_hid_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    bool event_valid = true;
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            uint8_t status = param[10];
            if (status) {
                printf("%s, connection failed\n", __func__);
                return;
            }

            memcpy(&ble_cb.con_handle, &param[7], 2);
            ble_disable_latency(ble_cb.con_handle);
            ble_latency_en_dly_tick_init();
            HID_SVC_DEBUG("-->BLE_HID_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cb.con_handle, &param[0], 2);
            HID_SVC_DEBUG("-->BLE_HID_EVENT_DISCONNECTED:%x\n",ble_cb.con_handle);
            ble_cb.con_handle = 0;
            report_id1_client_config = CCCD_DFT;

            #if LE_DIR_ADV_EN
            if (ble_get_bonding_cnt()) {
                #if LE_RPA_EN
                ble_rpa_param_init();
                ble_rpa_add_dev_to_list(0);  //bonding info index 0
                #endif //LE_RPA_EN
                ble_dir_adv_param_init(0);  //bonding info index 0
            }
            #endif
        } break;

        case BLE_EVT_PAIRING_START:{
            HID_SVC_DEBUG("-->BLE_EVT_PAIRING_START\n");
        } break;

        case BLE_EVT_PAIRING_DONE:{
            HID_SVC_DEBUG("-->BLE_EVT_PAIRING_DONE status:0x%x\n", param[9]);  //status 0x00 is pairing success
        } break;
        case BLE_EVT_ENCRYPTION_CHANGE:
            ble_enc_done = 1;
            HID_SVC_DEBUG("-->BLE_EVT_ENCRYPTION_CHANGE\n");
            break;
        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
}

static uint16_t ble_hid_service_read_cb_proc(uint16_t data_size, const uint8_t *data, uint16_t offset, uint8_t *read_buf, uint16_t read_buf_size)
{
    uint16_t read_size = data_size;
    read_size = read_size - offset;
    read_size = (read_size > read_buf_size) ? read_buf_size : read_size;
    memcpy(read_buf, data+offset, read_size);
    return read_size;
}

static uint16_t ble_hid_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t read_len = 0;
    HID_SVC_DEBUG("hid read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch(attribute_handle){
        case ATT_CHARACTERISTIC_2A4A_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_information);
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len= ble_hid_service_read_cb_proc(read_len, ble_hid_information, offset, buffer, buffer_size);
        } break;
        case ATT_CHARACTERISTIC_2A4B_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_descriptor);
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len = ble_hid_service_read_cb_proc(read_len, ble_hid_descriptor, offset, buffer, buffer_size);
        } break;
        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
    // HID_SVC_DEBUG("len:%d\n", read_len);
    // print_r(buffer, read_len);
	return read_len;
}

static int ble_hid_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
	bool event_valid = true;
    HID_SVC_DEBUG("hid write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A4D_01_CLIENT_CONFIGURATION_HANDLE:
            report_id1_client_config = GET_LE16(&buffer[0]);
            break;
        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
	return ATT_ERR_NO_ERR;
}

void ble_hid_service_notify_for_handle(uint16_t att_handle, uint8_t cmd)
{
    if(report_id1_client_config && !txpkt_is_full(&ble_tx)){
        extern bool ble_sm_pairing_support(void);
        /*If SM is supported, we should make sure encryption is done before nofity*/
        if (ble_sm_pairing_support()) {
            if (!ble_enc_done) return;
        }
    
        memset(ble_hid_tx.buf, 0, BLE_HID_NOTIFY_DATA_BUF_LEN);
        ble_hid_tx.buf[2] = cmd;
        ble_notify_for_handle(ble_cb.con_handle, att_handle, ble_hid_tx.buf, BLE_HID_NOTIFY_DATA_BUF_LEN);
    }
}

void ble_hid_service_init(void)
{
    HID_SVC_DEBUG("ble_ble_hid_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_1812_HID_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_1812_HID_END_HANDLE;

    // register service with ATT Server
	ble_hid_service.start_handle   = start_handle;
	ble_hid_service.end_handle     = end_handle;
	ble_hid_service.read_callback  = &ble_hid_service_read_callback;
	ble_hid_service.write_callback = &ble_hid_service_write_callback;
	ble_hid_service.event_handler  = &ble_hid_service_event_callback;
	att_server_register_service_handler(&ble_hid_service);

    report_id1_client_config = CCCD_DFT;
    ble_hid_var_init();
}

void ble_hid_key_notify_do(uint8_t hid_key_msg)
{
    uint8_t hid_key_cmd = BLE_HID_KEY_NONE_CMD;

        switch (hid_key_msg) {
        case BLE_KEY_MSG_RELEASE:
            hid_key_cmd = BLE_HID_KEY_RELEASE_CMD;
            break;
        case BLE_KEY_MSG_UP:
            hid_key_cmd = BLE_HID_KEY_UP_CMD;
            break;
        case BLE_KEY_MSG_DOWN:
            hid_key_cmd = BLE_HID_KEY_DOWN_CMD;
            break;
        case BLE_KEY_MSG_LEFT:
            hid_key_cmd = BLE_HID_KEY_LEFT_CMD;
            break;
        case BLE_KEY_MSG_RIGHT:
            hid_key_cmd = BLE_HID_KEY_RIGHT_CMD;
            break;
        case BLE_KEY_MSG_MEUN:
            hid_key_cmd = BLE_HID_KEY_MEUN_CMD;
            break;
        case BLE_KEY_MSG_HOME:
            hid_key_cmd = BLE_HID_KEY_HOME_CMD;
            break;
        case BLE_KEY_MSG_OK:
            hid_key_cmd = BLE_HID_KEY_OK_CMD;
            break;
        case BLE_KEY_MSG_BACK:
            hid_key_cmd = BLE_HID_KEY_BACK_CMD;
            break;
        case BLE_KEY_MSG_VOL_UP:
            hid_key_cmd = BLE_HID_KEY_VOL_UP_CMD;
            break;
        case BLE_KEY_MSG_VOL_DOWN:
            hid_key_cmd = BLE_HID_KEY_VOL_DOWN_CMD;
            break;
        case BLE_KEY_MSG_POWER:
            hid_key_cmd = BLE_HID_KEY_PWR_CMD;
            break;

        default:
            break;
    }

    if (hid_key_cmd != BLE_HID_KEY_NONE_CMD) {
        ble_set_key_msg(BLE_KEY_MSG_NONE);
        ble_hid_service_notify_for_handle(ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, hid_key_cmd);
    }
}

AT(.text.app.proc.ble)
void ble_hid_service_proc(void)
{
    if (ble_cb.con_handle == 0) {
        return;
    }

    uint8_t hid_key_msg = ble_get_key_msg();

    if (hid_key_msg != BLE_KEY_MSG_NONE){
        lowpwr_sleep_delay_reset();
        ble_hid_key_notify_do(hid_key_msg);
    }
}

#endif // LE_REOMTE_PRODUCT_ID
