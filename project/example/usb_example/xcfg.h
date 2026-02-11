//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u8 uosc_both_cap;                           //OSC基础电容: 0:0PF, 1:6PF
    u8 uosci_cap;                               //自定义OSCI电容
    u8 uosco_cap;                               //自定义OSCO电容
    u32 ft_osc_cap_en                    : 1;   //优先使用产测电容值
    u8 xm_keep_start[0];                        //For Keep Area Start
    u8 osc_both_cap;                            //产测OSC基础电容
    u8 osci_cap;                                //产测OSCI电容
    u8 osco_cap;                                //产测OSCO电容
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
