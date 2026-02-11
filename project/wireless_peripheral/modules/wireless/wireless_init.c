#include "include.h"

#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

const uint8_t wl_adv_data[] = {
    0x03, 0xff, 0x42, 0x06,
};


void bsp_wireless_link_info_write(wireless_addr_info_typedef *addr_info)
{
    /* Store Info */
}

bool bsp_wireless_link_info_read(wireless_addr_info_typedef *addr_info)
{
    return false;
}

void wireless_adv_init(bool adv_en)
{
    wireless_addr_info_typedef wireless_addr_info;
    wireless_adv_param_t wireless_adv_param;

    /* ASSERT: adv data len (e.g. wl_adv_data buf size) little than or equal 31B */

    memset(&wireless_adv_param, 0x00, sizeof(wireless_adv_param));
    memcpy(&wireless_adv_param.data_buf, wl_adv_data, sizeof(wl_adv_data));
    wireless_adv_param.data_len    = sizeof(wl_adv_data);
    wireless_adv_param.intv        = 0x30;
    wireless_adv_param.channel_map = ADV_ALL_CHNLS_EN;

#if WIRELESS_CONN_EN
    if (bsp_wireless_link_info_read(&wireless_addr_info)) {
        TRACE("config direct adv - addr: ");
        TRACE_R(wireless_addr_info.addr, 6);
        wireless_adv_param.adv_type       = ADV_TYPE_DIRECT_LO_IND;
        wireless_adv_param.peer_addr_type = wireless_addr_info.address_type;
        memcpy(wireless_adv_param.peer_addr, wireless_addr_info.addr, 6);
    } else {
        TRACE("config undirect adv\n");
        wireless_adv_param.adv_type = ADV_TYPE_IND;
    }
#else
    wireless_adv_param.adv_type = ADV_TYPE_SCAN_IND;
#endif

    wireless_adv_param_set(&wireless_adv_param);

    if (adv_en) {
        TRACE("adv enable\n");
        wireless_adv_ctrl(true);
    }
}

void wireless_module_init(void)
{
    printf("%s: slave\n", __func__);

    wireless_setup_param_t wireless_setup_param;
    memset(&wireless_setup_param, 0x00, sizeof(wireless_setup_param));
    wireless_setup_param.conn_nb_support = WIRELESS_CONN_NB;
    wireless_setup_param.conn_phy_forced = WIRELESS_PHY_FORCED;
    wireless_setup(&wireless_setup_param);

    wireless_adv_init(true);

#if WIRELESS_SCAN_EN
    wireless_scan_param_set(0, 100, 50);
    wireless_scan_ctrl_2(true, WIRELESS_SCAN_TIMEOUT);
#endif
}

void wireless_module_deinit(void)
{
    wireless_off();
}
