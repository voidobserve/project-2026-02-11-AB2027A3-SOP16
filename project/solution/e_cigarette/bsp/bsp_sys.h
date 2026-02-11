#ifndef _BSP_SYS_H
#define _BSP_SYS_H


typedef struct {
    u32 rst_reason;
    u32 wakeup_reason;
#if SYS_SLEEP_EN
    u32 sleep_time;
    u32 sleep_delay;
    u8  sleep_sta;
    bool sleep_prevent;
#endif
#if SYS_OFF_TIME
    u32 pwroff_time;
    u32 pwroff_delay;
#endif

    bool tft_bglight_kick;          //背光控制
    u8   tft_bglight_duty;          //背光pwm占空比
    u8   tft_bglight_last_duty;     //背光pwm上一次占空比
	bool tft_bglight_frist_set;
    u8   te_bglight_cnt;            //在收到需要打开背光控制时，推完第一帧数据后延时打开背光
    bool lp_wk_dis;
} sys_cb_t;
extern sys_cb_t sys_cb;

void bsp_sys_init(void);
void bsp_periph_init(void);
void sys_ram_info_dump(void);
void usr_tmr5ms_thread_do(void);

#endif // _BSP_SYS_H

