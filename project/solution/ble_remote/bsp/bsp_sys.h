#ifndef _BSP_SYS_H
#define _BSP_SYS_H
#include "bsp_key.h"

typedef struct {
    u32 rst_reason;
    u32 wakeup_reason;

#if SYS_SLEEP_EN
    u32 sleep_time;
    u32 sleep_delay;
    u32 deep_sleep_time;
    u32 deep_sleep_delay;
    u8  sleep_sta;
    bool sleep_prevent;
    u8  deep_sleep_flag;  //0:light sleep; 1:deep sleep.
#if LE_REMOTE_HOLD_KEY_LOWPOWER_EN
    keymsg_2_iopin_typedef sleep_keymsg_2_iopin;
#endif  //LE_REMOTE_HOLD_KEY_LOWPOWER_EN
#endif

#if SYS_OFF_TIME
    u32 pwroff_time;
    u32 pwroff_delay;
#endif
    bool lp_wk_dis;
} sys_cb_t;
extern sys_cb_t sys_cb;

void bsp_sys_init(void);
void bsp_periph_init(void);
void sys_ram_info_dump(void);
void usr_tmr5ms_thread_do(void);

#endif // _BSP_SYS_H

