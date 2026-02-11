#ifndef _BSP_KEY_H
#define _BSP_KEY_H
#include "driver_gpio.h"

// Key Property
#define KEY_SCAN_TIMES          3             //按键防抖的扫描次数
#define KEY_UP_TIMES            6             //抬键防抖的扫描次数
#define KEY_LONG_TIMES          150           //长按键的次数
#define KEY_HOLD_TIMES          35            //连按的频率次数
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05
#define KEY_ID_K5               0x06
#define KEY_ID_K6               0x07
#define KEY_ID_K7               0x08
#define KEY_ID_K8               0x09
#define KEY_ID_K9               0x0a
#define KEY_ID_K10              0x0b
#define KEY_ID_K11              0x0c
#define KEY_ID_K12              0x0d
#define KEY_ID_K13              0x0e

typedef struct {
    u16 key_id;
    u16 key_press_cnt;
    u8 release_debounce_cnt;
    u8 power_on_flag;
} key_status_typedef;

#if LE_REMOTE_HOLD_KEY_LOWPOWER_EN
typedef struct {
    uint16_t key_msg;
    gpio_typedef *gpiox_1;
    uint16_t gpio_pin_1;
    gpio_typedef *gpiox_2;
    uint16_t gpio_pin_2;
} keymsg_2_iopin_typedef;
bool bsp_key_get_key_pin_from_msg(uint16_t msg, keymsg_2_iopin_typedef* keymsg_2_iopin);
#endif  //LE_REMOTE_HOLD_KEY_LOWPOWER_EN

void bsp_key_init(void);
u16 bsp_key_scan(u8 mode);
void bsp_key_set_power_on_flag(bool flag);
bool bsp_key_get_power_on_flag(void);


#endif // _BSP_KEY_H
