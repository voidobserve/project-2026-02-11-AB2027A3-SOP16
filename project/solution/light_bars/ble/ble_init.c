#include "include.h"

///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          8
#define MAX_NOTIFY_LEN          128     //max=512
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

ble_cb_t ble_cb AT(.ble_buf.var);

BT_WORK_MODE cfg_bt_work_mode            = MODE_NORMAL;

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_set_work_mode(BT_WORK_MODE mode)
{
    cfg_bt_work_mode = mode;
}

BT_WORK_MODE ble_get_work_mode(void)
{
    return cfg_bt_work_mode;
}

void ble_txpkt_init(void)
{
    memset(&ble_tx, 0x00, sizeof(ble_tx));

    txpkt_init(&ble_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    ble_tx.send_kick = ble_send_kick;
}

void ble_sm_param_init(void)
{
    uint8_t security_req_en = 0;
    uint8_t auth_req = SM_AUTHREQ_BONDING | SM_AUTHREQ_MITM_PROTECTION;
    uint8_t io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT;

    ble_sm_init(security_req_en, io_capability, auth_req);
}

void ble_adv_param_init(void)
{
    adv_param_t adv_info;
    memset(&adv_info, 0, sizeof(adv_param_t));
    adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data));
    adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
    adv_info.adv_int_min = adv_info.adv_int_max = 0x30;
	adv_info.adv_type = ADV_TYPE_IND;
    adv_info.channel_map = ADV_ALL_CHNLS_EN;
    adv_info.adv_en = 1;

    ble_set_adv_param(&adv_info);
}

#if LE_SCAN_EN
void ble_scan_param_init(void)
{
    scan_param_t param;

    memset(&param, 0, sizeof(scan_param_t));

    param.scan_type = 0;
    param.scan_interval = 0xA0;
    param.scan_window = 0x30;

    ble_set_scan_param(&param);
    ble_scan_en();
}
#endif

void ble_periph_init(void)
{
    ble_txpkt_init();
#if LE_SM_EN
    ble_sm_param_init();
#endif
    gap_random_address_set_mode(GAP_PUBLIC_ADDRESS_TYPE);
    att_server_init(ble_get_profile_data());
    ble_service_init();
    ble_adv_param_init();
#if LE_SCAN_EN
    ble_scan_param_init();
#endif
}

void bsp_ble_init(void)
{
    memset(&ble_cb, 0x00, sizeof(ble_cb));

    bt_init();

    ble_init_callback_register(ble_periph_init);

    bt_setup();
}
