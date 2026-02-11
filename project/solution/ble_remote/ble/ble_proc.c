#include "include.h"
#include "ble_fota_service.h"
#include "ble_service.h"
#include "ble_service_atvv.h"

//#define BLE_ADV_SLEEP_INTERVAL    800   //500ms
//#define BLE_CON_SLEEP_INTERVAL    400   //500ms
//#define BLE_CON_SLEEP_LATENCY     0
//#define BLE_CON_SLEEP_TIMEOUT     1000   //10s


//static struct ble_param_t {
//    uint16_t adv_interval;
//    uint16_t conn_interval;
//    uint16_t conn_latency;
//    uint16_t conn_timeout;
//} ble_param;

volatile uint8_t ble_key_msg = BLE_KEY_MSG_NONE;  //扩展按键消息数量

static bool ble_latency_en_dly_flag = 0;
static uint32_t ble_latency_en_dly_tick = -1;

//static void ble_update_param_before_enter_sleep(void)
//{
//    if (ble_cb.con_handle) {
//        ble_get_conn_param(ble_cb.con_handle, &ble_param.conn_interval, &ble_param.conn_latency, &ble_param.conn_timeout);
//        ble_update_conn_param(ble_cb.con_handle, BLE_CON_SLEEP_INTERVAL, BLE_CON_SLEEP_LATENCY, BLE_CON_SLEEP_TIMEOUT);
//    } else {
//        ble_get_adv_interval(&ble_param.adv_interval);
//        ble_set_adv_interval(BLE_ADV_SLEEP_INTERVAL);
//    }
//}
//
//static void ble_update_param_after_exit_sleep(void)
//{
//    if (ble_cb.con_handle) {
//        ble_update_conn_param(ble_cb.con_handle, ble_param.conn_interval, ble_param.conn_latency, ble_param.conn_timeout);
//    } else {
//        ble_set_adv_interval(ble_param.adv_interval);
//    }
//}

AT(.com_sleep.ble.sleep)
bool ble_user_service_pending(void)
{
    return ble_cb.pending;
}

AT(.text.app.proc.ble)
void ble_user_set_service_pending(void)
{
    ble_cb.pending = 1;
}

AT(.text.app.proc.ble)
void ble_user_clear_service_pending(void)
{
    ble_cb.pending = 0;
}

void ble_enter_sleep_process(void)
{
    ble_enable_latency(ble_cb.con_handle);
    //ble_update_param_before_enter_sleep();
}

void ble_exit_sleep_process(void)
{
    // ble_disable_latency(ble_cb.con_handle);
    //ble_update_param_after_exit_sleep();
}

void ble_set_key_msg(uint8_t msg)
{
    ble_key_msg = msg;
}

AT(.text.app.proc.ble)
uint8_t ble_get_key_msg(void)
{
    return ble_key_msg;
}

void ble_latency_en_dly_tick_init(void)
{
    ble_latency_en_dly_flag = 1;
    ble_latency_en_dly_tick = tick_get();
}

AT(.text.app.proc.ble)
void ble_latency_en_dly_proc(void)
{
    if (ble_latency_en_dly_flag == 1) {
        if (tick_check_expire(ble_latency_en_dly_tick, LE_REMOTE_LATENCY_EN_DLY)) {
            ble_latency_en_dly_flag = 0;
            ble_enable_latency(ble_cb.con_handle);
            // printf("ble_latency_en_dly_proc\n");
        }
    }
}

AT(.text.app.proc.ble)
void ble_app_proc(void)
{
    ble_generic_access_service_proc();
    ble_generic_att_service_proc();
    ble_battery_service_proc();
    ble_hid_service_proc();
    ble_voice_service_proc();

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)
    ble_atvv_adv_proc();
#endif

#if LE_FOTA_EN
    ble_fota_process();
#endif

    ble_latency_en_dly_proc();

    //此函数放最后
    ble_user_clear_service_pending();
}

AT(.com_sleep.ble.sleep)
bool ble_proc_pending(void)
{
    bool res = false;

#if LE_FOTA_EN
    res |= ble_fota_is_start();
#endif

    res |= ble_user_service_pending();

    return res;
}
