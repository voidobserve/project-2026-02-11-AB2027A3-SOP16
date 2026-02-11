#include "include.h"
#include "func.h"
#include "bsp_ecigs.h"
#if BSP_ECIGS_EN

typedef struct f_ecig_reminder_t_ {
    u32 tick;
    u8 last_sta;
} f_ecig_reminder_t;

enum{
    COMPO_TXT_STA = 1,
};
compo_form_t *func_ecig_reminder_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 25);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(txt,COMPO_TXT_STA);
    // compo_form_add_image(frm, UI_BUF_HELLO_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TIME);
    return frm;
}


void func_ecig_reminder_process(void)
{
    f_ecig_reminder_t *f_ecig_reminder = (f_ecig_reminder_t*)func_cb.f_cb;
    compo_textbox_t *txt = compo_getobj_byid(COMPO_TXT_STA);
    u8 curr_sta = get_smoke_sta();
    if (f_ecig_reminder->last_sta != curr_sta) {
        f_ecig_reminder->last_sta = curr_sta;
        f_ecig_reminder->tick = tick_get();
        switch(curr_sta){
            case SMOKING:
                printf("SMOKING........\n");
                compo_textbox_set(txt, "SMOKING");
                break;
            case SHORT_CIRCUIT:
                printf("SHORT_CIRCUIT........\n");
                compo_textbox_set(txt, "SHORT_CIRCUIT");
                break;
            case OPEN_CIRCUIT:
                printf("OPEN_CIRCUIT........\n");
                compo_textbox_set(txt, "OPEN_CIRCUIT");
                break;
            case LOW_POWER:
                printf("LOW_POWER........\n");
                compo_textbox_set(txt, "LOW_POWER");
                break;
            case SMOKE_TIMEOUT:
                printf("SMOKE_TIMEOUT........\n");
                compo_textbox_set(txt, "SMOKE_TIMEOUT");
                break;
            default:
                break;
        }
    }
    
    if(tick_check_expire(f_ecig_reminder->tick, 1500)){
        func_directly_back_to();
    }

    func_process();
}


void func_ecig_reminder_message(u16 msg)
{
    switch (msg) {

        default:
            func_message(msg);
            break;
    }
}

void func_ecig_reminder_enter(void)
{
    func_struct_init(sizeof(f_ecig_reminder_t));
    func_cb.frm_main = func_ecig_reminder_form_create();
    f_ecig_reminder_t *f_ecig_reminder = (f_ecig_reminder_t*)func_cb.f_cb;
    f_ecig_reminder->tick = tick_get();
}

void func_ecig_reminder_exit(void)
{
    func_cb.last = FUNC_ECIG_REMINDER;
}

void func_ecig_reminder(void)
{
    printf("%s\n", __func__);

    func_ecig_reminder_enter();
    while (func_cb.sta == FUNC_ECIG_REMINDER) {
        func_ecig_reminder_process();
        func_ecig_reminder_message(msg_dequeue());

    }

    func_ecig_reminder_exit();
}

#endif