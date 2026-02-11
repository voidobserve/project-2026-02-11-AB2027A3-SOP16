#include "include.h"

///////////////////////////////////////////////////////////////////////////
#define MAX_TX_NUM          8
#define MAX_TX_LEN          128     //max=512
#define TX_POOL_SIZE       (MAX_TX_LEN + sizeof(struct txbuf_tag)) * MAX_TX_NUM

BT_WORK_MODE cfg_bt_work_mode            = MODE_NORMAL;

AT(.ble_cache.att)
uint8_t ble_tx_pool[TX_POOL_SIZE];

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

    txpkt_init(&ble_tx, ble_tx_pool, MAX_TX_NUM, MAX_TX_LEN);
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

    memset(&adv_info,0,sizeof(adv_info));
    adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data));
    adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
    adv_info.adv_int_min = adv_info.adv_int_max = 0x30;
    adv_info.adv_type = ADV_TYPE_IND;
    adv_info.adv_en = 1;
    adv_info.channel_map = ADV_ALL_CHNLS_EN;

    ble_set_adv_param(&adv_info);
}

void ble_periph_init(void)
{
    ble_txpkt_init();
    ble_sm_param_init();
    gap_random_address_set_mode(GAP_PUBLIC_ADDRESS_TYPE);
    att_server_init(ble_get_profile_data());
    ble_service_init();
    ble_adv_param_init();
}

void bsp_ble_init(void)
{
    bt_init();

    ble_init_callback_register(ble_periph_init);

    bt_setup();
}
