#include "include.h"
#include "func.h"

typedef struct f_charge_t_ {
    
} f_charge_t;

enum{
    COMPO_ANIM = 1024,
};

compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    // compo_form_add_image(frm, UI_BUF_HELLO_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_ANIMATION_CHARGE172320_202X_BIN);
    compo_picturebox_set_pos(pic, 86, 160);
    compo_picturebox_cut(pic, 0, 25);
    compo_setid(pic,COMPO_ANIM);


    return frm;
}

static u8 cut_x;
void func_charge_process(void)
{
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ANIM);
    static u32 ticks = 0;
    // 每20ms更新一帧
    if (tick_check_expire(ticks, 80)) {
        ticks = tick_get();
        cut_x += 1;  
        
        if (cut_x >= 24) { 
            cut_x = 0;
        }
        
        compo_picturebox_cut(pic, cut_x, 25);
    }

    if (wko_io_read_bit() == RESET) {                                   // KEY0(WK0/PP)
        func_switch_to(FUNC_UI,FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
    }
    func_process();
}


void func_charge_message(u16 msg)
{
    switch (msg) {

        default:
            func_message(msg);
            break;
    }

}

void func_charge_enter(void)
{
    func_struct_init(sizeof(f_charge_t));
    func_cb.frm_main = func_charge_form_create();
}

void func_charge_exit(void)
{
    func_cb.last = FUNC_CHARGE;
}

void func_charge(void)
{
    printf("%s\n", __func__);

    func_charge_enter();
    while (func_cb.sta == FUNC_CHARGE) {
        func_charge_process();
        func_charge_message(msg_dequeue());

    }

    func_charge_exit();
}

