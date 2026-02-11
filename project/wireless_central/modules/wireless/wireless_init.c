#include "include.h"
#include "wireless.h"


void wireless_module_init(void)
{
    printf("%s: master\n", __func__);

    wireless_setup_param_t wireless_setup_param;
    memset(&wireless_setup_param, 0x00, sizeof(wireless_setup_param));
    wireless_setup_param.conn_nb_support = WIRELESS_CONN_NB;
    wireless_setup_param.conn_phy_forced = WIRELESS_PHY_FORCED;
    wireless_setup(&wireless_setup_param);

    wireless_scan_param_set(0, 100, 50);
    wireless_scan_ctrl_2(true, WIRELESS_SCAN_TIMEOUT);
}

void wireless_module_deinit(void)
{
    wireless_off();
}
