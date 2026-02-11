#include "include.h"
#include "ble_service_atvv.h"
#include "utils_ring_buffer.h"
#include "bsp_sdadc_voice.h"
#include "api_sysclk.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)

#define VOICE_SVC_DEBUG_EN 1

#if VOICE_SVC_DEBUG_EN
#define VOICE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define VOICE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_voice_service;
ble_voice_rx_cb_t ble_voice_rx_cb AT(.ble_voice.rx);
ble_voice_tx_t ble_voice_tx AT(.ble_voice.tx);
ring_buf_t ble_voice_ring_buf AT(.ble_voice.ring_buf);
ble_voice_var_t ble_voice_var AT(.ble_voice.var);
static uint8_t ring_buf[BLE_VOICE_RING_BUF_LEN] AT(.ble_voice.ring_buf);

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
        } break;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            VOICE_SVC_DEBUG("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            uint8_t status = param[0];
            uint16_t handle = param[1] | (param[2] << 8);
            uint16_t interval = param[3] | (param[4] << 8);
            uint16_t latency = param[5] | (param[6] << 8);
            uint16_t timeout = param[7] | (param[8] << 8);
            VOICE_SVC_DEBUG("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
            if (interval > 12) {
                VOICE_SVC_DEBUG("interval > 12, !!update conn param request 7.5ms ~ 15ms!!\n");
                //improve speech recognition rate
                ble_update_conn_param_2(handle, 6, 12, 49, 200);
            }
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
}

static uint16_t ble_voice_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t read_len = 0;
    VOICE_SVC_DEBUG("voice read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_AUDIO_01_VALUE_HANDLE:{
            VOICE_SVC_DEBUG("ATT_CHARACTERISTIC_AUDIO_01_VALUE_HANDLE\n");
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
    bool event_valid = true;
    uint8_t wptr = 0;
    VOICE_SVC_DEBUG("voice write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_AUDIO_01_CLIENT_CONFIGURATION_HANDLE:
            ble_voice_var.tx_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_CTL_01_CLIENT_CONFIGURATION_HANDLE:
            ble_voice_var.ctl_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_ATVV_CMD_01_VALUE_HANDLE:
            wptr = ble_voice_rx_cb.cmd_wptr & BLE_VOICE_CMD_BUF_MASK;
            ble_voice_rx_cb.cmd_wptr++;
            memcpy(ble_voice_rx_cb.cmd[wptr].buf, buffer, buffer_size);
            ble_voice_rx_cb.cmd[wptr].len = buffer_size;
            ble_voice_rx_cb.cmd[wptr].handle = attribute_handle;
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

AT(.com_text.enc.proc)
static void ble_voice_service_enqueue_enc_buf_proc(uint8_t *buffer, uint32_t size)
{
    utils_ring_buf_write(&ble_voice_ring_buf, buffer, size);
}

AT(.com_text.enc.proc)
void user_enc_buf_process(uint8_t *buffer, uint32_t size)
{
    ble_voice_service_enqueue_enc_buf_proc(buffer, size);
}

static uint32_t ble_voice_service_dequeue_enc_buf_proc(uint8_t *buffer, uint32_t size)
{
    return utils_ring_buf_read(&ble_voice_ring_buf, buffer, size);
}

static int16_t ble_voice_service_notify_for_handle(uint16_t att_handle, uint8_t *data, uint16_t data_size, uint8_t kick)
{
    if ((att_handle == ATT_CHARACTERISTIC_CTL_01_VALUE_HANDLE) && !ble_voice_var.ctl_client_config) {
        return -1;
    }
    if ((att_handle == ATT_CHARACTERISTIC_AUDIO_01_VALUE_HANDLE) && !ble_voice_var.tx_client_config) {
        return -1;
    }

    int16_t ret = ble_notify_for_handle_kick(ble_cb.con_handle, att_handle, data, data_size, kick);
    return ret;
}

AT(.text.app.proc.ble)
static void ble_voice_service_voice_data_notify_do(void)
{
    uint8_t voice_buf_index = 0;
    uint8_t real_audio_pkt_len = 0;
    uint8_t enc_buf[BLE_VOICE_ENC_DATA_LEN];
    uint8_t tx_available_num = 0;

    tx_available_num = txpkt_available_num(&ble_tx);
    if (tx_available_num < (BLE_VOICE_NOTIFY_AUDIO_PKT_NUM+1)) {
        VOICE_SVC_DEBUG(".");
        return; //groups of 8
    }

    uint32_t enc_len = ble_voice_service_dequeue_enc_buf_proc(enc_buf, BLE_VOICE_ENC_DATA_LEN);

    if (enc_len != BLE_VOICE_ENC_DATA_LEN) {
        return;
    }

    ble_voice_tx.voice_buf[AUDIO_BUF_SEQ_L_OFS] = (ble_voice_var.frame_num>>8) & 0xff;
    ble_voice_tx.voice_buf[AUDIO_BUF_SEQ_H_OFS] = ble_voice_var.frame_num & 0xff;
    ble_voice_tx.voice_buf[AUDIO_BUF_ID_OFS] = 0;
    ble_voice_tx.voice_buf[AUDIO_BUF_PREV_L_OFS] = enc_buf[1];
    ble_voice_tx.voice_buf[AUDIO_BUF_PREV_H_OFS] = enc_buf[0];
    ble_voice_tx.voice_buf[AUDIO_BUF_INDEX_OFS] = enc_buf[2];
    memcpy(&ble_voice_tx.voice_buf[AUDIO_BUF_DATA_OFS], &enc_buf[4], AUDIO_BUF_DATA_LEN);

    //AUDIO PACKET
    for (uint8_t cnt=0; cnt < BLE_VOICE_NOTIFY_AUDIO_PKT_NUM; cnt++) {
        voice_buf_index = cnt * BLE_VOICE_NOTIFY_AUDIO_PKT_LEN;
        real_audio_pkt_len = (cnt == (BLE_VOICE_NOTIFY_AUDIO_PKT_NUM-1)) ? BLE_VOICE_NOTIFY_AUDIO_END_PKT_LEN : BLE_VOICE_NOTIFY_AUDIO_PKT_LEN;
        ble_voice_service_notify_for_handle(ATT_CHARACTERISTIC_AUDIO_01_VALUE_HANDLE, &ble_voice_tx.voice_buf[voice_buf_index], real_audio_pkt_len, TXPKT_KICK_DISABLE); //voice data
        //VOICE_SVC_DEBUG("a 0x%x\n", res);
    }

    //AUDIO SYNC PACKET
    ble_voice_var.frame_num += 1;
    ble_voice_tx.ctl_buf[CTL_AUDIO_SYNC_PKT_CMD_OFS] = CTL_AUDIO_SYNC_CMD;
    ble_voice_tx.ctl_buf[CTL_AUDIO_SYNC_PKT_FRAME_NUM_L_OFS] = (ble_voice_var.frame_num>>8) & 0xff;
    ble_voice_tx.ctl_buf[CTL_AUDIO_SYNC_PKT_FRAME_NUM_H_OFS] = ble_voice_var.frame_num & 0xff;
    ble_voice_service_notify_for_handle(ATT_CHARACTERISTIC_CTL_01_VALUE_HANDLE, ble_voice_tx.ctl_buf, CTL_AUDIO_SYNC_PKT_LEN, TXPKT_KICK_ENABLE); //frame num
    //VOICE_SVC_DEBUG("n 0x%x\n", res);
}

AT(.text.app.proc.ble)
static void ble_voice_service_voice_data_notify_process(void)
{
    if (ble_voice_var.voice_status != VOICE_STATUS_START) {
        return;
    }

    ble_voice_service_voice_data_notify_do();
}

void ble_voice_service_init(void)
{
    VOICE_SVC_DEBUG("ble voice service init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_SERVICE_ATVV_VOICE_01_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_SERVICE_ATVV_VOICE_01_END_HANDLE;

    // register service with ATT Server
	ble_voice_service.start_handle   = start_handle;
	ble_voice_service.end_handle     = end_handle;
	ble_voice_service.read_callback  = &ble_voice_service_read_callback;
	ble_voice_service.write_callback = &ble_voice_service_write_callback;
	ble_voice_service.event_handler  = &ble_voice_service_event_callback;
	att_server_register_service_handler(&ble_voice_service);

    ble_voice_service_var_init();
    utils_ring_buf_init(&ble_voice_ring_buf, ring_buf, sizeof(ring_buf));
}

static void ble_voice_service_sdadc_start(void)
{
    VOICE_SVC_DEBUG("start voice_status:%d\n", ble_voice_var.voice_status);
    if (ble_voice_var.voice_status == VOICE_STATUS_START) { //防重开
        return;
    }
    lowpwr_sleep_auto_dis();
    ble_voice_var.timeout_tick = tick_get();
    ble_voice_var.voice_status = VOICE_STATUS_START;
    ble_voice_var.exit_clk = sys_clk_get();
    ble_voice_var.frame_num = 0;
    sys_clk_set(SYS_120M);
    adpcm_sample_idx_reset();
    txpkt_flush(&ble_tx);
    bsp_sdadc_voice_start();
}

static void ble_voice_service_sdadc_stop(void)
{
    VOICE_SVC_DEBUG("stop voice_status:%d\n", ble_voice_var.voice_status);
    if (ble_voice_var.voice_status == VOICE_STATUS_STOP) {
        return;
    }
    lowpwr_sleep_auto_en();
    ble_voice_var.voice_status = VOICE_STATUS_STOP;
    ble_voice_var.frame_num = 0;
    sys_clk_set(ble_voice_var.exit_clk);
    bsp_sdadc_voice_stop();
    utils_ring_buf_clear(&ble_voice_ring_buf);
}

static void ble_voice_service_voice_start_notify(void)
{
    if (ble_voice_var.voice_status == VOICE_STATUS_START) { //防重开
        return;
    }
    VOICE_SVC_DEBUG("ble_voice_service_voice_start_notify\n");
    ble_voice_tx.ctl_buf[CTL_START_SEARCH_PKT_CMD_OFS] = CTL_START_SEARCH_CMD;
    ble_voice_service_notify_for_handle(ATT_CHARACTERISTIC_CTL_01_VALUE_HANDLE, ble_voice_tx.ctl_buf, CTL_START_SEARCH_PKT_LEN, TXPKT_KICK_AUTO);
    ble_hid_service_notify_for_handle(ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, BLE_HID_KEY_VOICE_EN_CMD);
}

static void ble_voice_service_voice_stop_nsotify(void)
{
    if (ble_voice_var.voice_status == VOICE_STATUS_STOP) {
        return;
    }
    VOICE_SVC_DEBUG("ble_voice_service_voice_stop_notify\n");
    ble_voice_service_sdadc_stop();
    ble_voice_tx.ctl_buf[CTL_AUDIO_STOP_PKT_CMD_OFS] = CTL_AUDIO_STOP_CMD;
    ble_voice_service_notify_for_handle(ATT_CHARACTERISTIC_CTL_01_VALUE_HANDLE, ble_voice_tx.ctl_buf, CTL_AUDIO_STOP_PKT_LEN, TXPKT_KICK_AUTO);
}

AT(.text.app.proc.ble)
static void ble_voice_service_voice_timeout_process(void)
{
    if (ble_voice_var.voice_status == VOICE_STATUS_START) {
        if (tick_check_expire(ble_voice_var.timeout_tick, ATVV_VOICE_TIMEOUT)) {
            ble_voice_service_voice_stop_nsotify();
        }
    }
}

static void ble_voice_service_atv_cmd_process(uint8_t *buf, uint8_t buf_size)
{
    bool atv_cmd_resp_flag = 1;
    uint8_t atv_cmd_resp_len = 0;
    uint8_t atv_cmd = buf[0];
    uint16_t atv_codecs_sp = 0;
    VOICE_SVC_DEBUG("atv cmd:%d\n", atv_cmd);
    switch (atv_cmd) {
        case ATVV_GET_CAPS_CMD: {

            VOICE_SVC_DEBUG("!!update conn param request 7.5ms!!\n");
            //improve speech recognition rate
            ble_update_conn_param(ble_cb.con_handle, 6, 49, 100);

            ble_voice_tx.ctl_buf[CTL_GET_CAPS_RESP_PKT_CMD_OFS] = CTL_GET_CAPS_RESP_CMD;
            PUT_BE16(&ble_voice_tx.ctl_buf[CTL_GET_CAPS_RESP_PKT_VERSION_OFS], ATVV_INFO_VERSION);
            PUT_BE16(&ble_voice_tx.ctl_buf[CTL_GET_CAPS_RESP_PKT_CODECS_SP_OFS], ATVV_INFO_CODECS_SUPPORTED);
            PUT_BE16(&ble_voice_tx.ctl_buf[CTL_GET_CAPS_RESP_PKT_FRAME_LEN_OFS], ATVV_INFO_FRAME_LEN);
            PUT_BE16(&ble_voice_tx.ctl_buf[CTL_GET_CAPS_RESP_PKT_CHAR_LEN_OFS], ATVV_INFO_CHAR_LEN);
            atv_cmd_resp_len = CTL_GET_CAPS_RESP_PKT_LEN;
        } break;

        case ATVV_MIC_OPEN_CMD: {
            atv_codecs_sp = GET_BE16(&buf[ATVV_MIC_OPEN_PKT_CODEC_USED_OFS]);
            VOICE_SVC_DEBUG("atv codecs supported:%d\n", atv_codecs_sp);
            if (atv_codecs_sp & ATVV_INFO_CODECS_SUPPORTED) {
                ble_voice_tx.ctl_buf[CTL_AUDIO_START_PKT_CMD_OFS] = CTL_AUDIO_START_CMD;
                atv_cmd_resp_len = CTL_AUDIO_START_PKT_LEN;
                ble_voice_service_sdadc_start();
            } else {
                ble_voice_tx.ctl_buf[CTL_MIC_OPEN_ERROR_PKT_CMD_OFS] = CTL_MIC_OPEN_ERROR_CMD;
                PUT_BE16(&ble_voice_tx.ctl_buf[CTL_MIC_OPEN_ERROR_PKT_ERR_CODE_OFS], ATVV_INFO_MIC_ERROR_CODE);
                atv_cmd_resp_len = CTL_MIC_OPEN_ERROR_PKT_LEN;
            }
        } break;

        case ATVV_MIC_CLOSE_CMD: {
            ble_voice_tx.ctl_buf[CTL_AUDIO_STOP_PKT_CMD_OFS] = CTL_AUDIO_STOP_CMD;
            atv_cmd_resp_len = CTL_AUDIO_STOP_PKT_LEN;
            ble_voice_service_sdadc_stop();
        } break;

        default: {
            atv_cmd_resp_flag = 0;
        } break;
    }

    if (atv_cmd_resp_flag) {
        ble_voice_service_notify_for_handle(ATT_CHARACTERISTIC_CTL_01_VALUE_HANDLE, ble_voice_tx.ctl_buf, atv_cmd_resp_len, TXPKT_KICK_AUTO); //sync
    }
}

static void ble_voice_service_rx_cmd_process(void)
{
    if (ble_voice_rx_cb.cmd_rptr == ble_voice_rx_cb.cmd_wptr) {
        return;
    }
    uint8_t rptr = ble_voice_rx_cb.cmd_rptr & BLE_VOICE_CMD_BUF_MASK;
    uint8_t *buf = ble_voice_rx_cb.cmd[rptr].buf;
    uint8_t buf_size = ble_voice_rx_cb.cmd[rptr].len;
    uint16_t handle = ble_voice_rx_cb.cmd[rptr].handle;
    ble_voice_rx_cb.cmd_rptr++;
    switch (handle) {
        case ATT_CHARACTERISTIC_ATVV_CMD_01_VALUE_HANDLE:
            ble_voice_service_atv_cmd_process(buf, buf_size);
            break;

        default:
            break;
    }
}

AT(.text.app.proc.ble)
void ble_voice_service_proc(void)
{
    if (ble_cb.con_handle == 0) {
        return;
    }

    uint8_t voice_key_msg = ble_get_key_msg();

    if (voice_key_msg == BLE_KEY_MSG_VOICE_ON) {
        ble_set_key_msg(BLE_KEY_MSG_NONE);
        ble_voice_service_voice_start_notify();
    }

    ble_voice_service_rx_cmd_process();
    ble_voice_service_voice_data_notify_process();
    ble_voice_service_voice_timeout_process();
}

#endif  //LE_REOMTE_PRODUCT_ID
