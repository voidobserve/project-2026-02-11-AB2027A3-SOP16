#include "include.h"
#include "driver_charge.h"
#include "bsp_charge.h"
#include "bsp_saradc_vbat.h"


#if BSP_CHARGE_EN

#define RECHARGE_THRESHOLD                      (4200)      //4.2V, this value should not less than 3V

const char charge_status_info[][29] = {
    "[charge] uninitialized\n",
    "[charge] vusb disconnected\n",
    "[charge] finish-but-connect\n",
    "[charge] trickle cur state\n",
    "[charge] const cur state\n",
    "[charge] const vol state\n",
};

// start charge -> [trigle stage] -> constant current & constant voltage -> finish charge
void bsp_charge_init(void)
{
    charge_init_typedef charge_init_struct;

    charge_init_struct.const_curr = CHARGE_CUR_100mA;
    charge_init_struct.cutoff_curr = CHARGE_CUTOFF_CUR_30mA;
    charge_init_struct.cutoff_volt = CHARGE_CUTOFF_VOL_4V35;
    charge_init_struct.dcin_reset = CHARGE_DCIN_RESET_DIS;
    charge_init_struct.trick_curr = CHARGE_CUR_20mA;
    charge_init_struct.trick_curr_en = ENABLE;
    charge_init_struct.mode = CHARGE_MODE_FULL_DISCONNECT;

    charge_init(&charge_init_struct);
}

// 测试函数
AT(.text.app.proc.charge)
void bsp_charge_process(void)
{
    static u32 delay_cnt1;
    static u32 delay_cnt2;
    static u8 charge_status_last = CHARGE_STA_OFF;
    u8 charge_status = charge_status_last;

    charge_status = charge_get_status();
    if (charge_status != charge_status_last) {
        charge_status_last = charge_status;
        printf(charge_status_info[charge_status]);
    }

    if (charge_status_last > CHARGE_STA_OFF_BUT_DC_IN) {
        lowpwr_pwroff_delay_reset();
        lowpwr_sleep_delay_reset();
    }

    if (tick_check_expire(delay_cnt1, 100)) {
        delay_cnt1 = tick_get();
        charge_process();

        /*if stop charging by battery fully charged, we do recharging here*/
        if ((charge_get_status() == CHARGE_STA_OFF_BUT_DC_IN) && (charge_get_mode() != CHARGE_MODE_FULL_KEEP)) {
            #if BSP_VBAT_DETECT_EN
                uint32_t vbat_cur = bsp_get_vbat_level(1);

                if(vbat_cur < RECHARGE_THRESHOLD) {
                    printf("[charge] start re-charging\n");
                    charge_status_update(CHARGE_STA_ON_CON_CURR);
                }
            #endif
        }
    }

    if (tick_check_expire(delay_cnt2, 2000)) {
        delay_cnt2 = tick_get();
        if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE_BUT_ERR) {
            printf("[charge] CHARGE LINE VOLTAGE ERROR!!!\n");
        }
    }
}

#endif // BSP_CHARGE_EN
