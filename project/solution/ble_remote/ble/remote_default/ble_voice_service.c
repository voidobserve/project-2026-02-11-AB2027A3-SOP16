#include "include.h"
#include "ble_service.h"
#include "utils_ring_buffer.h"
#include "bsp_sdadc_voice.h"
#include "api_sysclk.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)

#define VOICE_SVC_DEBUG_EN 1

#if VOICE_SVC_DEBUG_EN
#define VOICE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define VOICE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_voice_service;

static uint16_t report_id5_client_config;
static uint16_t report_id6_client_config;
static uint16_t report_id7_client_config;
static uint16_t report_id8_client_config;


ble_voice_rx_cb_t ble_voice_rx_cb AT(.ble_voice.rx);
ble_voice_tx_t ble_voice_tx AT(.ble_voice.tx);
ring_buf_t ble_voice_ring_buf AT(.ble_voice.ring_buf);
uint8_t ble_voice_buf[240] AT(.ble_voice.ring_buf);
ble_voice_var_t ble_voice_var AT(.ble_voice.var);

AT(.com_rodata.msbc.header)
const uint8_t ble_voice_msbc_header[4][2] = {{0x01, 0x08}, {0x01, 0x38}, {0x01, 0xc8}, {0x01, 0xf8}};
const uint8_t ble_voice_msbc_packet_handle_tbl[] = {ATT_CHARACTERISTIC_2A4D_06_VALUE_HANDLE, ATT_CHARACTERISTIC_2A4D_07_VALUE_HANDLE, ATT_CHARACTERISTIC_2A4D_08_VALUE_HANDLE};
const uint8_t ble_voice_handle59_value[] = {0x27, 0x17, 0x32, 0xba, 0x30, 0x18, 0x64, 0x19, 0xa0, 0x00, 0x00, 0x00};
const uint8_t ble_voice_handle5c_value[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
const uint8_t ble_voice_handle62_value[] = {0x02};

static void ble_voice_service_var_init(void)
{
    memset(&ble_voice_var, 0, sizeof(ble_voice_var_t));
}

static void ble_voice_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
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
            VOICE_SVC_DEBUG("-->BLE_VOICE_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint8_t disc_reason = param[2];
            memcpy(&ble_cb.con_handle, &param[0], 2);
            VOICE_SVC_DEBUG("-->BLE_VOICE_EVENT_DISCONNECTED:%x, disc_reason:0x%x\n",ble_cb.con_handle, disc_reason);
            ble_cb.con_handle = 0;
            report_id5_client_config = CCCD_DFT;
            report_id6_client_config = CCCD_DFT;
            report_id7_client_config = CCCD_DFT;
            report_id8_client_config = CCCD_DFT;
        } break;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            VOICE_SVC_DEBUG("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            uint8_t status = param[0];
            uint16_t handle = param[1] | (param[2] << 8);
            uint16_t interval = param[3] | (param[4] << 8);
            uint16_t latency = param[5] | (param[6] << 8);
            uint16_t timeout = param[7] | (param[8] << 8);
            VOICE_SVC_DEBUG("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
}

static uint16_t ble_voice_service_read_cb_proc(uint16_t data_size, const uint8_t *data, uint16_t offset, uint8_t *read_buf, uint16_t read_buf_size)
{
    uint16_t read_size = data_size;
    read_size = read_size - offset;
    read_size = (read_size > read_buf_size) ? read_buf_size : read_size;
    memcpy(read_buf, data+offset, read_size);
    return read_size;
}

static uint16_t ble_voice_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t read_len = 0;
    VOICE_SVC_DEBUG("voice read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A4D_0b_VALUE_HANDLE:{
            VOICE_SVC_DEBUG("ATT_CHARACTERISTIC_2A4D_0b_VALUE_HANDLE\n");
            read_len = sizeof(ble_voice_handle59_value);
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len = ble_voice_service_read_cb_proc(read_len, ble_voice_handle59_value, offset, buffer, buffer_size);
        } break;

        case ATT_CHARACTERISTIC_2A4D_0c_VALUE_HANDLE:{
            VOICE_SVC_DEBUG("ATT_CHARACTERISTIC_2A4D_0c_VALUE_HANDLE\n");
            read_len = sizeof(ble_voice_handle5c_value);
            if (buffer == NULL) {
                break;
            }
            read_len = ble_voice_service_read_cb_proc(read_len, ble_voice_handle5c_value, offset, buffer, buffer_size);
        } break;

        case ATT_CHARACTERISTIC_2A4D_0e_VALUE_HANDLE:{
            VOICE_SVC_DEBUG("ATT_CHARACTERISTIC_2A4D_0e_VALUE_HANDLE\n");
            read_len = sizeof(ble_voice_handle62_value);
            if (buffer == NULL) {
                break;
            }
            read_len = ble_voice_service_read_cb_proc(read_len, ble_voice_handle62_value, offset, buffer, buffer_size);
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }

	return read_len;
}

static int ble_voice_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    uint8_t wptr = 0;
    VOICE_SVC_DEBUG("voice write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A4D_04_VALUE_HANDLE:{
            wptr = ble_voice_rx_cb.cmd_wptr & BLE_VOICE_CMD_BUF_MASK;
            ble_voice_rx_cb.cmd_wptr++;
            memcpy(ble_voice_rx_cb.cmd[wptr].buf, buffer, buffer_size);
            ble_voice_rx_cb.cmd[wptr].len = buffer_size;
            ble_voice_rx_cb.cmd[wptr].handle = attribute_handle;
        } break;

        case ATT_CHARACTERISTIC_2A4D_05_CLIENT_CONFIGURATION_HANDLE:
            report_id5_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_2A4D_06_CLIENT_CONFIGURATION_HANDLE:
            report_id6_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_2A4D_07_CLIENT_CONFIGURATION_HANDLE:
            report_id7_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_2A4D_08_CLIENT_CONFIGURATION_HANDLE:
            report_id8_client_config = GET_LE16(&buffer[0]);
            break;

        default:
            break;
    }
	return ATT_ERR_NO_ERR;
}

AT(.com_text.msbc.proc)
static void ble_voice_service_enqueue_msbc_buf_proc(uint8_t *buffer, uint32_t size)
{
    static uint8_t header_index = 0;
    uint8_t voice_buf_temp[BLE_VOICE_NOTIFY_DATA_BUF_LEN] = {0};
    voice_buf_temp[0] = ble_voice_msbc_header[header_index][0];
    voice_buf_temp[1] = ble_voice_msbc_header[header_index][1];
    header_index = (header_index == 3) ? 0 : (header_index+1); //轮询4个msbc包头
    memcpy(&voice_buf_temp[2], buffer, size);
    utils_ring_buf_write(&ble_voice_ring_buf, voice_buf_temp, BLE_VOICE_NOTIFY_DATA_BUF_LEN);
}

AT(.com_text.msbc.proc)
void user_enc_buf_process(uint8_t *buffer, uint32_t size)
{
    ble_voice_service_enqueue_msbc_buf_proc(buffer, size);
}

static uint32_t ble_voice_service_dequeue_msbc_buf_proc(uint8_t *buffer, uint32_t size)
{
    return utils_ring_buf_read(&ble_voice_ring_buf, buffer, size);
}

static void ble_voice_service_data_notify_for_handle(uint16_t att_handle, uint8_t *data, uint16_t data_size)
{
    if((att_handle == ATT_CHARACTERISTIC_2A4D_06_VALUE_HANDLE) && !report_id6_client_config){
        return;
    }
    if((att_handle == ATT_CHARACTERISTIC_2A4D_07_VALUE_HANDLE) && !report_id7_client_config){
        return;
    }
    if((att_handle == ATT_CHARACTERISTIC_2A4D_08_VALUE_HANDLE) && !report_id8_client_config){
        return;
    }

    ble_notify_for_handle(ble_cb.con_handle, att_handle, data, data_size);
}

AT(.text.app.proc.ble)
static void ble_voice_service_voice_data_notify_process(void)
{
    if (ble_voice_var.voice_status != VOICE_STATUS_START) {
        return;
    }

    uint8_t att_handle = 0;
    static uint8_t att_handle_index = 0;

    uint32_t len = ble_voice_service_dequeue_msbc_buf_proc(ble_voice_tx.buf, BLE_VOICE_NOTIFY_DATA_BUF_LEN);

    if(len){
        att_handle = ble_voice_msbc_packet_handle_tbl[att_handle_index]; //用户协议需要
        ble_voice_service_data_notify_for_handle(att_handle, ble_voice_tx.buf, BLE_VOICE_NOTIFY_DATA_BUF_LEN);
        att_handle_index = (att_handle_index == 2) ? 0 : (att_handle_index+1); //轮询3个handle 0x46、0x4a、0x4e
    }
}

static void ble_voice_service_ctl_notify_for_handle(uint16_t att_handle, uint8_t cmd, uint16_t cmd_size)
{
    if(report_id5_client_config){
        memset(ble_voice_tx.buf, 0, BLE_VOICE_NOTIFY_DATA_BUF_LEN);
        ble_voice_tx.buf[0] = cmd;
        ble_notify_for_handle(ble_cb.con_handle, att_handle, ble_voice_tx.buf, cmd_size);
    }
}

void ble_voice_service_init(void)
{
    VOICE_SVC_DEBUG("ble voice service init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_1812_VOICE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_1812_VOICE_END_HANDLE;

    // register service with ATT Server
	ble_voice_service.start_handle   = start_handle;
	ble_voice_service.end_handle     = end_handle;
	ble_voice_service.read_callback  = &ble_voice_service_read_callback;
	ble_voice_service.write_callback = &ble_voice_service_write_callback;
	ble_voice_service.event_handler  = &ble_voice_service_event_callback;
	att_server_register_service_handler(&ble_voice_service);

    ble_voice_service_var_init();
    msbc_encode_init();
    utils_ring_buf_init(&ble_voice_ring_buf, ble_voice_buf, sizeof(ble_voice_buf));

    report_id5_client_config = CCCD_DFT;
    report_id6_client_config = CCCD_DFT;
    report_id7_client_config = CCCD_DFT;
    report_id8_client_config = CCCD_DFT;
}

static void ble_voice_service_start_proc(void)
{
    if (ble_voice_var.voice_status == VOICE_STATUS_START) { //防重开
        return;
    }
    VOICE_SVC_DEBUG("ble voice remote control start\n");

    ble_voice_var.exit_clk = sys_clk_get();
    sys_clk_set(SYS_120M);

    lowpwr_sleep_auto_dis();
    ble_voice_var.voice_status = VOICE_STATUS_START;
    ble_voice_service_ctl_notify_for_handle(ATT_CHARACTERISTIC_2A4D_05_VALUE_HANDLE, BLE_VOICE_ENABLE_CMD, BLE_VOICE_NOTIFY_CTL_BUF_LEN);
    ble_hid_service_notify_for_handle(ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, BLE_HID_KEY_VOICE_EN_CMD);
}

static void ble_voice_service_stop_proc(void)
{
    if (ble_voice_var.voice_status == VOICE_STATUS_STOP) {
        return;
    }
    VOICE_SVC_DEBUG("ble voice remote control stop\n");

    lowpwr_sleep_auto_en();

    ble_voice_var.voice_status = VOICE_STATUS_STOP;
    bsp_sdadc_voice_stop();
    utils_ring_buf_clear(&ble_voice_ring_buf);
    ble_voice_service_ctl_notify_for_handle(ATT_CHARACTERISTIC_2A4D_05_VALUE_HANDLE, BLE_VOICE_DISABLE_CMD, BLE_VOICE_NOTIFY_CTL_BUF_LEN);
    ble_hid_service_notify_for_handle(ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, BLE_HID_KEY_RELEASE_CMD);
    sys_clk_set(ble_voice_var.exit_clk);
}

static void ble_voice_service_rx_cmd_process(void)
{
    uint8_t rptr;
    uint16_t handle;
    uint8_t *rx_data;

    if (ble_voice_rx_cb.cmd_rptr == ble_voice_rx_cb.cmd_wptr) {
        return;
    }
    rptr = ble_voice_rx_cb.cmd_rptr & BLE_VOICE_CMD_BUF_MASK;
    ble_voice_rx_cb.cmd_rptr++;
    handle = ble_voice_rx_cb.cmd[rptr].handle;
    rx_data = ble_voice_rx_cb.cmd[rptr].buf;

    switch (handle) {
        case ATT_CHARACTERISTIC_2A4D_04_VALUE_HANDLE:
            if(rx_data[4] == BLE_VOICE_ENABLE_CMD){
                bsp_sdadc_voice_start();
            }
            break;

        default:
            break;
    }
}

AT(.text.app.proc.ble)
void ble_voice_service_proc(void)
{
    if(ble_cb.con_handle == 0) {
        return;
    }

    uint8_t voice_key_msg = ble_get_key_msg();

    if (voice_key_msg == BLE_KEY_MSG_VOICE_ON) {
        ble_set_key_msg(BLE_KEY_MSG_NONE);
        ble_voice_service_start_proc();
    }else if (voice_key_msg == BLE_KEY_MSG_VOICE_OFF) {
        ble_set_key_msg(BLE_KEY_MSG_NONE);
        ble_voice_service_stop_proc();
    }

    ble_voice_service_voice_data_notify_process();

    ble_voice_service_rx_cmd_process();
}

#endif // LE_REOMTE_PRODUCT_ID
