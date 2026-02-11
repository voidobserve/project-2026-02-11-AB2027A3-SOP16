#include "include.h"
#include "wireless.h"

#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

void bsp_wireless_link_info_write(wireless_addr_info_typedef *addr_info)
{
    bsp_param_write((u8 *)addr_info, PARAM_WL_LINK_INFO, PARAM_WL_LINK_INFO_LEN);
    bsp_param_sync();
}

bool bsp_wireless_link_info_read(wireless_addr_info_typedef *addr_info)
{
    bsp_param_read((u8 *)addr_info, PARAM_WL_LINK_INFO, PARAM_WL_LINK_INFO_LEN);
    if (addr_info->valid) {
        return true;
    } else {
        return false;
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

    wl_adv_init(WL_RM_ADV_PKT_DEFAULT);
}

void wireless_module_deinit(void)
{
    wireless_off();
}
