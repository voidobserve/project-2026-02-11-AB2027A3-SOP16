#include "wireless.h"

//配对广播
const uint8_t adv_data_const_pair[] = {
    // Manufacturer Specific Data
    0x03, 0xff, 0x12, 0x34,
};

//关机后广播
const uint8_t adv_data_const_poweroff[] = {
    // Manufacturer Specific Data
    0x03, 0xFF, 0x00, 0x00,
};

uint8_t wl_get_adv_data(u8 *adv_buf, WL_RM_ADV_PKT adv_pkt)
{
    u32 data_len = 0;
    wireless_addr_info_typedef wireless_addr_info;
    bool have_bd = bsp_wireless_link_info_read(&wireless_addr_info);  //bonding info

    printf("%s, adv_pkt:%d, have_bd:%d\n", __func__, adv_pkt, have_bd);

    if ((adv_pkt == WL_RM_ADV_PKT_DEFAULT) && !have_bd) {
        //上电无配对记录，自动开配对广播
        data_len = sizeof(adv_data_const_pair);

        memcpy(adv_buf, adv_data_const_pair, sizeof(adv_data_const_pair));
    } else if ((adv_pkt == WL_RM_ADV_PKT_POWERON) && have_bd) {
        //按键开机发定向广播
    } else if (adv_pkt == WL_RM_ADV_PKT_PAIR) {
        //配对广播
        data_len = sizeof(adv_data_const_pair);
        memcpy(adv_buf, adv_data_const_pair, sizeof(adv_data_const_pair));
    }

    return data_len;
}

void wl_adv_init(WL_RM_ADV_PKT adv_pkt)
{
    wireless_addr_info_typedef wireless_addr_info;
    wireless_adv_param_t wireless_adv_param;

    /* ASSERT: adv data len (e.g. wl_adv_data buf size) little than or equal 31B */

    memset(&wireless_adv_param, 0x00, sizeof(wireless_adv_param));

    wireless_adv_param.data_len = wl_get_adv_data((u8 *)&wireless_adv_param.data_buf, adv_pkt);
    wireless_adv_param.intv     = 0x30;

    if (bsp_wireless_link_info_read(&wireless_addr_info) && (adv_pkt == WL_RM_ADV_PKT_POWERON || adv_pkt == WL_RM_ADV_PKT_DEFAULT)) {
        printf("config direct adv - addr: ");
        print_r(wireless_addr_info.addr, 6);
        wireless_adv_param.adv_type       = ADV_TYPE_DIRECT_LO_IND;
        wireless_adv_param.peer_addr_type = wireless_addr_info.address_type;
        memcpy(wireless_adv_param.peer_addr, wireless_addr_info.addr, 6);
    } else {
        printf("config undirect adv\n");
        wireless_adv_param.adv_type = ADV_TYPE_IND;
    }

    wireless_adv_param_set(&wireless_adv_param);
    wireless_adv_ctrl(true);
}

void wl_adv_ctl_set(uint8_t key_msg)
{
    wireless_addr_info_typedef wireless_addr_info;

    if ((key_msg == KEY_MSG_POWER) && !wireless_con_handle_get()) {
        wireless_adv_ctrl(false);
        wl_adv_init(WL_RM_ADV_PKT_POWERON);
    } else if ((key_msg == KEY_MSG_POWER) && wireless_con_handle_get()) {
        wireless_adv_ctrl(false);
    } else if (key_msg == KEY_MSG_PAIR) {
        wireless_adv_ctrl(false);
        wireless_addr_info.valid = false;
        bsp_wireless_link_info_write(&wireless_addr_info);
        if (wireless_con_handle_get()) {
            wireless_disconnect(wireless_con_handle_get());
            delay_5ms(400);  //2s后发配对广播,根据需求修改
        }
        wl_adv_init(WL_RM_ADV_PKT_PAIR);
    }
}

AT(.text.app.proc.ble)
void wl_adv_proc(void)
{

}
