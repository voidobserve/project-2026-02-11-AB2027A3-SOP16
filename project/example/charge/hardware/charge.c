#include "driver_charge.h"
#include "charge.h"
#include "saradc_vbat.h"

// start charge -> [trigle stage] -> constant current & constant voltage -> finish charge
void my_charge_init(void)
{
    charge_init_typedef charge_init_struct;

    charge_init_struct.const_curr = CHARGE_CUR_100mA;
    charge_init_struct.cutoff_curr = CHARGE_CUTOFF_CUR_30mA;
    charge_init_struct.cutoff_volt = CHARGE_CUTOFF_VOL_4V35;
    charge_init_struct.dcin_reset = CHARGE_DCIN_RESET_DIS;
    charge_init_struct.trick_curr = CHARGE_CUR_5mA;
    charge_init_struct.trick_curr_en = ENABLE;
    charge_init_struct.mode = CHARGE_MODE_FULL_DISCONNECT;

    charge_init(&charge_init_struct);

    vbat_detect_init();
}

// 测试函数
void my_charge_example(void)
{
    static u32 delay_cnt1;
    static u32 delay_cnt2;
    u8 dc_status;

    if (tick_check_expire(delay_cnt1, 500)) {
        delay_cnt1 = tick_get();
        dc_status = charge_detect_dc();
        if (dc_status == CHARGE_DC_STATUS_ONLINE_BUT_ERR) {
            printf("CHARGE LINE VOLTAGE ERROR!!!\n");
        } else {
            printf("\ncharge_cfg.sta(0:un-init  1:off  2:off-but-connect  3:trickle  4:const curr  5:const vol):\n");
            printf("--->>> sta: %X\n", charge_get_status());
            printf("--->>> DC online: %X\n", dc_status);
            printf("power: %d\n", get_vbat_level(0));
            printf("vbat: %d\n", get_vbat_level(1));
            printf("cutoff current condition: %X\n", charge_cutoff_cur_condition());
            printf("cutoff voltage condition: %X\n", charge_cutoff_vol_condition());
            printf("trickle voltage condition: %X\n", charge_trickle_vol_condition());
        }
    }

    if (tick_check_expire(delay_cnt2, 10)) {
        delay_cnt2 = tick_get();
        charge_process();
    }
}
