#ifndef _FUNC_H
#define _FUNC_H

#include "func_bt.h"
#include "func_le_fcc.h"
#include "func_le_bqb_rf.h"
#include "func_iodm.h"
#include "func_idle.h"
#include "func_lowpwr.h"
#include "func_switching.h"

//task number
enum {
    FUNC_NULL = 0,
    FUNC_BT,
    FUNC_LE_FCC,
    FUNC_LE_BQB_RF,
    FUNC_IODM,
    FUNC_TBOX,
    FUNC_PWROFF,
    FUNC_IDLE,

    FUNC_ECIG_REMINDER,

    FUNC_UI,
    FUNC_CHARGE,
};

//task control block
typedef struct {
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
    void *f_cb;                                     //当前任务缓存指针
    compo_form_t *frm_main;                         //当前窗体
} func_cb_t;
extern func_cb_t func_cb;

void func_struct_init(int size);
void func_switch_to(u8 sta, u16 switch_mode);
void func_backing_to(void);                         //页面滑动回退功能
void func_back_to(void);                            //页面按键回退功能
u8 func_directly_back_to(void);                     //页面直接回退,无动画效果

void func_process(void);
void func_message(u16 msg);

void func_run(void);
void func_idle(void);
void func_bt(void);
void func_le_fcc(void);
void func_le_bqb_rf(void);
void func_iodm(void);
void func_tbox(void);

#endif
