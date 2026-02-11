#include "include.h"
#include "driver_com.h"
#include "bsp_saradc_key.h"
#include "bsp_io_key.h"
#include "bsp_keyscan.h"

#if BSP_KEY_EN

static key_status_typedef key_status;

//AT(.com_rodata.key)
//u16 allow_multiple_key_table[] = {
//    KEY_ID_PP | KEY_SHORT_UP,
//};

#if LE_REMOTE_HOLD_KEY_LOWPOWER_EN
#if BSP_KEYSCAN_EN
AT(.com_rodata.keymsg_2_iopin_tab)
const keymsg_2_iopin_typedef keymsg_2_iopin_tab[] = {
    {KEY_ID_K1  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_1},  //KA1
    {KEY_ID_K2  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_2},  //KA2
    {KEY_ID_K3  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_3},  //KA3
    {KEY_ID_K4  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_4},  //KA4
    {KEY_ID_K5  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_5},  //KA5
    {KEY_ID_K6  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  GPIOA_REG,  GPIO_PIN_6},  //KA6
    {KEY_ID_K7  |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_1,  GPIOA_REG,  GPIO_PIN_2},  //KB2
    {KEY_ID_K8  |  KEY_HOLD,  NULL,       0,           GPIOA_REG,  GPIO_PIN_1},  //KI1  一端接下拉
    {KEY_ID_K9  |  KEY_HOLD,  NULL,       0,           GPIOA_REG,  GPIO_PIN_2},  //KI2
  
    // {KEY_ID_K20 |  KEY_HOLD,  GPIOA_REG,  GPIO_PIN_0,  NULL,       0},       //KJ0  一端接上拉
};
#elif (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
AT(.com_rodata.keymsg_2_iopin_tab)
const keymsg_2_iopin_typedef keymsg_2_iopin_tab[] = {
    {KEY_ID_K1 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_3,  GPIOA_REG,  GPIO_PIN_0},
    {KEY_ID_K2 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_3,  GPIOA_REG,  GPIO_PIN_1},
    {KEY_ID_K3 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_3,  GPIOA_REG,  GPIO_PIN_2},
    {KEY_ID_K4 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_4,  GPIOA_REG,  GPIO_PIN_0},
    {KEY_ID_K5 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_4,  GPIOA_REG,  GPIO_PIN_1},
    {KEY_ID_K6 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_4,  GPIOA_REG,  GPIO_PIN_2},
    {KEY_ID_K7 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_5,  GPIOA_REG,  GPIO_PIN_0},
    {KEY_ID_K8 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_5,  GPIOA_REG,  GPIO_PIN_1},
    {KEY_ID_K9 | KEY_HOLD,  GPIOA_REG,  GPIO_PIN_5,  GPIOA_REG,  GPIO_PIN_2},
};
#endif

bool bsp_key_get_key_pin_from_msg(uint16_t msg, keymsg_2_iopin_typedef* keymsg_2_iopin)
{
    if ((msg & 0xff00) != KEY_HOLD) {
        return false;
    }

    for (uint8_t i=0; i < (sizeof(keymsg_2_iopin_tab)/sizeof(keymsg_2_iopin_typedef)); i++) {
        if (msg == keymsg_2_iopin_tab[i].key_msg) {
            // printf("msg:0x%x\n", msg);
            memcpy(keymsg_2_iopin, &keymsg_2_iopin_tab[i], sizeof(keymsg_2_iopin_typedef));
            return true;
        }
    }

    return false;
}
#endif  //LE_REMOTE_HOLD_KEY_LOWPOWER_EN

// 处理短按、长按、连按
AT(.com_text.key.process)
u16 key_process(u16 key_id)
{
    u16 key_return = KEY_ID_NO;

    if (key_id == KEY_ID_NO || key_id != key_status.key_id) {               // 抬键消抖
        if (key_status.release_debounce_cnt < KEY_UP_TIMES) {
            key_status.release_debounce_cnt++;
        } else {
            if (key_status.key_press_cnt >= KEY_LONG_TIMES) {               // 长按抬键
                key_return = key_status.key_id | KEY_LONG_UP;
            } else if (key_status.key_press_cnt >= KEY_SCAN_TIMES) {        // 短按抬键
                key_return = key_status.key_id | KEY_SHORT_UP;
            }

            key_status.key_id = key_id;
            key_status.key_press_cnt = 0;
        }
    } else {
        key_status.key_press_cnt++;
        key_status.release_debounce_cnt = 0;                         
        if (key_status.key_press_cnt == KEY_SCAN_TIMES) {               // 去抖
            key_return = key_status.key_id;
        } else if (key_status.key_press_cnt == KEY_LONG_TIMES) {        // 长按
            key_return = key_status.key_id | KEY_LONG;
        } else if (key_status.key_press_cnt == KEY_LONG_HOLD_TIMES) {   // 连按
            key_return = key_status.key_id | KEY_HOLD;
            key_status.key_press_cnt = KEY_LONG_TIMES;
        }
    }

    return key_return;
}

// mode: 0-事件检测  1-状态检测
AT(.com_text.key.scan)
u16 bsp_key_scan(u8 mode)
{
    u16 key, key_id = KEY_ID_NO;

#if BSP_IOKEY_EN
    key_id = bsp_get_io_key_id();
#endif // BSP_IOKEY_EN

#if BSP_ADKEY_EN
    if (key_id == KEY_ID_NO) {
        key_id = bsp_get_adkey_id();            // 根据ADC采样值返回键值
    }
#endif // BSP_ADKEY_EN

#if BSP_KEYSCAN_EN
    if (key_id == KEY_ID_NO) {
        key_id = bsp_get_keyscan_id();
    }
#endif // BSP_KEYSCAN_EN

    if (mode) {
        return key_id;
    }

    if (bsp_key_get_power_on_flag()) {
        if (key_id == KEY_ID_PP) {
            key_id = KEY_ID_NO;
        } else {
            bsp_key_set_power_on_flag(false);
        }
    }

    key = key_process(key_id);              // 短按、长按、连按处理
    key = key_multi_press_process(key);     // 多击处理

    if(key){
        msg_enqueue(key);
    }

    return key;
}

void bsp_key_init(void)
{
    memset(&key_status, 0, sizeof(key_status));

    //multiple_key_init(100, allow_multiple_key_table, sizeof(allow_multiple_key_table) / sizeof(u16));
    multiple_key_init(100, NULL, 0);

#if BSP_ADKEY_EN
    bsp_adkey_init();
#endif // BSP_ADKEY_EN

#if BSP_IOKEY_EN
    bsp_io_key_init();
#endif // BSP_IOKEY_EN

#if BSP_KEYSCAN_EN
    bsp_keyscan_init();
#endif // BSP_KEYSCAN_EN
}

AT(.com_text.key.set.power_on)
void bsp_key_set_power_on_flag(bool flag)
{
    key_status.power_on_flag = flag;
}

AT(.com_text.key.get.power_on)
bool bsp_key_get_power_on_flag(void)
{
    return key_status.power_on_flag;
}

#endif // BSP_KEY_EN
