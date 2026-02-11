#include "include.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
#include "ble_service.h"
#include "ble_adv.h"
#elif (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
#include "ble_service_atvv.h"
#include "ble_adv_atvv.h"
#endif

func_cb_t func_cb AT(.buf.func_cb);

AT(.text.app.proc.func)
void func_process(void)
{
    WDT_CLR();

#if BSP_SARADC_EN
    bsp_saradc_process();
#endif

#if BSP_CHARGE_EN
    bsp_charge_process();
#endif

#if BSP_IR_EN
    bsp_irtx_capture_handler_process();
#endif
    prod_test_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    bool event_valid = true;

    switch (msg) {

#if BSP_KEY_EN
        case KEY_ID_K1 | KEY_SHORT:                        // K1按键 向上
            printf("K1 key_up short key\n");
            ble_set_key_msg(BLE_KEY_MSG_UP);
            break;
        case KEY_ID_K2 | KEY_SHORT:                        // K2按键 向下
            printf("K2 key_down short key\n");
            ble_set_key_msg(BLE_KEY_MSG_DOWN);
            break;
        case KEY_ID_K3 | KEY_SHORT:                        // K3按键 向左
            printf("K3 key_left short key\n");
            ble_set_key_msg(BLE_KEY_MSG_LEFT);
            break;
        case KEY_ID_K4 | KEY_SHORT:                        // K4按键 向右
            printf("K4 key_right short key\n");
            ble_set_key_msg(BLE_KEY_MSG_RIGHT);
            break;
        case KEY_ID_K5 | KEY_SHORT:                        // K5按键 菜单
            printf("K5 key_meun short key\n");
            ble_set_key_msg(BLE_KEY_MSG_MEUN);
            break;
        case KEY_ID_K6 | KEY_SHORT:                        // K6按键 HOME
            printf("K6 key_home short key\n");
            ble_set_key_msg(BLE_KEY_MSG_HOME);
            break;
        case KEY_ID_K7 | KEY_SHORT:                        // K7按键 确认
            printf("K7 key_ok short key\n");
            ble_set_key_msg(BLE_KEY_MSG_OK);
            break;
        case KEY_ID_K8 | KEY_SHORT:                        // K8按键 后退
            printf("K8 key_back short key\n");
            ble_set_key_msg(BLE_KEY_MSG_BACK);
            break;
        case KEY_ID_K9 | KEY_SHORT:                        // K9按键 音量加
            printf("K9 key_vol_up short key\n");
            ble_set_key_msg(BLE_KEY_MSG_VOL_UP);
            break;
        case KEY_ID_K10 | KEY_SHORT:                        // K10按键 音量减
            printf("K10 key_vol_down short key\n");
            ble_set_key_msg(BLE_KEY_MSG_VOL_DOWN);
            break;
#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_DEFAULT)
        case KEY_ID_K11 | KEY_LONG:                        // K11按键 语音
            printf("K11 key_voice_ctl long key\n");
            ble_set_key_msg(BLE_KEY_MSG_VOICE_ON);
            break;
        case KEY_ID_K11 | KEY_LONG_UP:
            printf("K11 key_voice_ctl long key up\n");
            ble_set_key_msg(BLE_KEY_MSG_VOICE_OFF);
            break;
#elif (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
        case KEY_ID_K11 | KEY_SHORT:                        // K11按键 语音
            printf("K11 key_voice_ctl short key\n");
            ble_set_key_msg(BLE_KEY_MSG_VOICE_ON);
            break;
#endif
        case KEY_ID_K12 | KEY_SHORT:                        // K12按键 power
            printf("K12 key_pwr short\n");
            ble_adv_ctl_set(BLE_KEY_MSG_POWER);
            ble_set_key_msg(BLE_KEY_MSG_POWER);
            break;

        case KEY_ID_K13 | KEY_SHORT:                        // K13按键 pair (菜单 + home)
            printf("K13 key_pair short\n");
            ble_adv_ctl_set(BLE_KEY_MSG_PAIR);
            break;

        default: {
            uint8_t key_id = msg & 0xff;
            uint16_t key_up = msg & 0xff00;
            if ((key_id <= KEY_ID_K13) && (key_id > KEY_ID_NO)) {
                if ((key_up == KEY_SHORT_UP) || (key_up == KEY_LONG_UP)) {
                    // printf("key up\n");
                    ble_set_key_msg(BLE_KEY_MSG_RELEASE);
                    #if LE_REMOTE_HOLD_KEY_LOWPOWER_EN
                        sys_cb.sleep_keymsg_2_iopin.key_msg = 0;
                    #endif
                } else {
                    event_valid = false;
                }
            } else {
                event_valid = false;
            }
        } break;
#else

        default:
            event_valid = false;
            break;
#endif
    }

#if (LE_REMOTE_HOLD_KEY_LOWPOWER_EN)
    bsp_key_get_key_pin_from_msg(msg, &sys_cb.sleep_keymsg_2_iopin);
#endif

    if (event_valid) {
        lowpwr_pwroff_delay_reset();
        lowpwr_sleep_delay_reset();
    }
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    bsp_param_sync();

    lowpwr_sleep_delay_reset();

    lowpwr_pwroff_delay_reset();
}

AT(.text.func)
void func_exit(void)
{
    func_cb.sta = FUNC_IDLE;
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_LE_FCC_EN
        case FUNC_LE_FCC:
            func_le_fcc();
            break;
#endif

#if FUNC_LE_BQB_RF_EN
        case FUNC_LE_BQB_RF:
            func_le_bqb_rf();
            break;
#endif

#if FUNC_IODM_EN
        case FUNC_IODM:
            func_iodm();
            break;
#endif

#if FUNC_TBOX_EN
        case FUNC_TBOX:
            func_tbox();
            break;
#endif

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif

        case FUNC_PWROFF:
            func_pwroff();
            break;

        default:
            func_exit();
            break;
        }
    }
}
