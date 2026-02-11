#include "include.h"
#include "func.h"

typedef struct f_ui_t_ {
    
} f_ui_t;

compo_form_t *func_ui_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_form_add_image(frm, UI_BUF_HELLO_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TIME);
    return frm;
}


void func_ui_process(void)
{
    if (wko_io_read_bit() == RESET) {                                   // KEY0(WK0/PP)
        func_switch_to(FUNC_CHARGE,FUNC_SWITCH_LR_LEFT | FUNC_SWITCH_AUTO);
    }
    func_process();
}


void func_ui_message(u16 msg)
{
    switch (msg) {

        default:
            func_message(msg);
            break;
    }

}

void func_ui_enter(void)
{
    func_struct_init(sizeof(f_ui_t));
    func_cb.frm_main = func_ui_form_create();
}

void func_ui_exit(void)
{
    func_cb.last = FUNC_UI;
}

void func_ui(void)
{
    printf("%s\n", __func__);

    func_ui_enter();
    while (func_cb.sta == FUNC_UI) {
        func_ui_process();
        func_ui_message(msg_dequeue());

    }

    func_ui_exit();
}

