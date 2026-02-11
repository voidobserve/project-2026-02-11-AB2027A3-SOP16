#ifndef _BSP_KEY_H
#define _BSP_KEY_H


// Key Property
#define KEY_SCAN_TIMES          4           //按键防抖的扫描次数
#define KEY_UP_TIMES            4           //抬键防抖的扫描次数
#define KEY_LONG_TIMES          150         //长按键的次数
#define KEY_HOLD_TIMES          35          //连按的频率次数
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)

//CTP Message, 范围: 0xA0 ~ 0xAF
#define MSG_CTP_TOUCH           0xA0        //接触
#define MSG_CTP_CLICK           0xA1        //单击
#define MSG_CTP_SHORT_LEFT      0xA2        //短划
#define MSG_CTP_SHORT_UP        0xA3
#define MSG_CTP_SHORT_RIGHT     0xA4
#define MSG_CTP_SHORT_DOWN      0xA5
#define MSG_CTP_LONG            0xA6        //长划
#define MSG_CTP_LONG_LEFT       0xA7
#define MSG_CTP_LONG_UP         0xA8
#define MSG_CTP_LONG_RIGHT      0xA9
#define MSG_CTP_LONG_DOWN       0xAA
#define MSG_CTP_LONG_LIFT       0xAB        //长按抬起
#define MSG_CTP_COVER           0xAC

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05

typedef struct {
    u16 key_id;
    u16 key_press_cnt;
    u8 release_debounce_cnt;
    u8 power_on_flag;
} key_status_typedef;

void bsp_key_init(void);
u16 bsp_key_scan(u8 mode);
void bsp_key_set_power_on_flag(bool flag);
bool bsp_key_get_power_on_flag(void);


#endif // _BSP_KEY_H
