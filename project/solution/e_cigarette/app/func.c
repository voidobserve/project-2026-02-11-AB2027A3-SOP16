#include "include.h"
#include "bsp_ecigs.h"

#define FUNC_CREATE_CNT                       ((int)(sizeof(tbl_func_create) / sizeof(tbl_func_create[0])))
#define FUNC_ENTRY_CNT                        ((int)(sizeof(tbl_func_entry) / sizeof(tbl_func_entry[0])))

typedef struct func_t_ {
    int func_idx;
    void *func;
} func_t;

func_cb_t func_cb AT(.buf.func_cb);

extern void func_ui(void);
extern void func_charge(void);
extern void func_bt(void);
extern void func_ecig_reminder(void);

compo_form_t *func_ui_form_create(void);
compo_form_t *func_charge_form_create(void);
compo_form_t *func_ecig_reminder_form_create(void);

const func_t tbl_func_create[] = {
    {FUNC_UI,                           func_ui_form_create},
    {FUNC_CHARGE,                       func_charge_form_create},
#if BSP_ECIGS_EN
    {FUNC_ECIG_REMINDER,                func_ecig_reminder_form_create},
#endif
};

const func_t tbl_func_entry[] = {
#if FUNC_BT_EN
    {FUNC_BT,                           func_bt},
#endif
    {FUNC_UI,                           func_ui},
    {FUNC_CHARGE,                       func_charge},
#if BSP_ECIGS_EN
    {FUNC_ECIG_REMINDER,                func_ecig_reminder},
#endif
};

//根据任务名创建窗体。此处调用的创建窗体函数不要调用子任务的控制结构体
compo_form_t *func_create_form(u8 sta)
{
    compo_form_t *frm = NULL;
    compo_form_t *(*func_create)(void) = NULL;
    for (int i = 0; i < FUNC_CREATE_CNT; i++) {
        if (tbl_func_create[i].func_idx == sta) {
            func_create = tbl_func_create[i].func;
            frm = func_create();
            break;
        }
    }
    if (frm == NULL) {
        halt(HALT_FUNC_SORT);
    }
    return frm;
}

//切换
void func_switch_to(u8 sta, u16 switch_mode)
{
    compo_form_t *frm = func_create_form(sta);                          		//创建下一个任务的窗体
    bool res = func_switching(switch_mode, NULL);         						//切换动画
    compo_form_destroy(frm);                                                    //切换完成或取消，销毁窗体
    if (res) {
        func_cb.sta = sta;
    }
}

//页面滑动回退功能
void func_backing_to(void)
{
    func_switch_to(FUNC_UI, FUNC_SWITCH_LR_RIGHT);   //返回上一个界面
}

//页面按键回退功能
void func_back_to(void)
{
    func_switch_to(FUNC_UI, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);    //返回上一个界面
}


//页面直接回退,无动画效果
u8 func_directly_back_to(void)
{
    if (func_cb.last && func_cb.last != func_cb.sta) {
        func_cb.sta = func_cb.last;
    }
    return func_cb.sta;
}

#if BSP_ECIGS_EN
AT(.text.func.smoke)
void func_smoke_process(void)
{
    u8 sta = get_smoke_sta();

    if(sta != IDLE) {
        func_cb.sta = FUNC_ECIG_REMINDER;
    }
}
#endif

AT(.text.app.proc.func)
void func_process(void)
{
    WDT_CLR();

#if GUI_SELECT
    tft_bglight_frist_set_check();
    compo_update();                                     //更新组件
    gui_process();                                      //刷新UI
#endif

#if BSP_SARADC_EN
    bsp_saradc_process();
#endif

#if BSP_CHARGE_EN
    bsp_charge_process();
#endif

#if BSP_TSEN_EN
    bsp_tsensor_process();
#endif // BSP_TSEN_EN

#if BSP_ECIGS_HOT_EN
     e_cigs_event_handler();
#endif

    prod_test_process();

#if BSP_ECIGS_EN
    func_smoke_process();
#endif

#if SYS_SLEEP_EN
    if(sleep_process(bt_is_sleep)) {

    }
#endif

#if FUNC_BT_EN
    func_bt_sub_process();
#endif
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    bool event_valid = true;
    switch (msg) {

#if BSP_KEY_EN
        case KEY_ID_PP | KEY_SHORT_UP:      // PP按键短按抬起
            printf("PP key short key up\n");

            break;

        case KEY_ID_PP | KEY_LONG:          // PP长按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_HOLD:          // PP连按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_LONG_UP:       // PP长按抬起
            printf("PP key long up\n");
            // func_cb.sta = FUNC_PWROFF;
            break;

        case KEY_ID_PP | KEY_DOUBLE:        // PP双击
            printf("PP key double down\n");
            break;

        case KEY_ID_PP | KEY_THREE:         // PP三击
            printf("PP key triple down\n");
            break;

        case KEY_ID_K1 | KEY_SHORT_UP:         // K1短按抬起
            printf("K1 key short key up\n");
            break;

        case KEY_ID_K2 | KEY_SHORT_UP:         // K2短按抬起
            printf("K2 key short key up\n");
            break;

        case KEY_ID_K3 | KEY_SHORT_UP:         // K3短按抬起
            printf("K3 key short key up\n");
            break;

        case KEY_ID_K4 | KEY_SHORT_UP:         // K4短按抬起
            printf("K4 key short key up\n");
            break;
#endif

        default:
            event_valid = false;
            break;
    }

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
    //销毁窗体
    if (func_cb.frm_main != NULL) {
        compo_form_destroy(func_cb.frm_main);
    }

    //释放FUNC控制结构体
    if (func_cb.f_cb != NULL) {
        heap_free(func_cb.f_cb);
    }

    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
}

AT(.text.func)
void func_struct_init(int size)
{
    if (size == 0) {
        return;
    }

    func_cb.f_cb = heap_malloc(size);
    if (!func_cb.f_cb) {
        printf("f_cb zalloc err: %d\n", size);
        halt(HALT_FUNC_STRUCT);
    }
    memset(func_cb.f_cb, 0, size);
}


AT(.text.func)
void func_run(void)
{
    int i;
    void (*func_entry)(void) = NULL;

    printf("%s\n", __func__);

#if GUI_SELECT
    func_cb.sta = FUNC_UI;
#else
    func_cb.sta = FUNC_BT;
#endif


    for (;;) {
        func_enter();
        for (i = 0; i < FUNC_ENTRY_CNT; i++) {
            if (tbl_func_entry[i].func_idx == func_cb.sta) {
                func_entry = tbl_func_entry[i].func;
                func_entry();
                break;
            }
        }
        if (func_cb.sta == FUNC_PWROFF) {
            func_pwroff();
        } else if (i >= FUNC_ENTRY_CNT) {
            printf("func_entry err:%d\n", func_cb.sta);
            halt(HALT_FUNC_SORT);
        }

        func_exit();
    }
}
