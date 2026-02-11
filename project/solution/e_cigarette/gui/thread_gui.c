#include "include.h"
#include "thread_gui.h"


void gui_graphic_init(const void *p);
bool gui_line_draw(void);
void gui_frame_refresh(void);
void gui_frame_draw(void);
void tft_frame_start(void);
void tft_frame_end(void);


static struct gui_cb_t {
    bool flag_gui_need_refresh;
    volatile bool flag_gui_need_draw;
    volatile bool flag_gui_drawing;
    volatile bool flag_gui_draw_lost;
} gui_cb;


AT(.com_text.thread.gui)
void os_gui_draw(void)
{
    if (gui_cb.flag_gui_need_draw) {
        gui_cb.flag_gui_draw_lost = false;
        gui_cb.flag_gui_need_draw = false;
        gui_cb.flag_gui_drawing = true;             //开始绘图
        thread_driver_msg_post(THREAD_DRIVER_MSG_GUI_DRAW);
    } else {
        gui_cb.flag_gui_draw_lost = true;
    }
}

AT(.text.widget)
void gui_widget_refresh(void)
{
    gui_cb.flag_gui_need_refresh = true;
}

AT(.text.gui)
void os_gui_init(const gui_init_param_t *param)
{
    memset(&gui_cb, 0, sizeof(gui_cb));
    gui_graphic_init(param);
}

AT(.text.gui)
void os_gui_draw_w4_done(void)
{
    gui_cb.flag_gui_need_refresh = false;
    gui_cb.flag_gui_need_draw = false;
    while (gui_cb.flag_gui_drawing);
    gui_cb.flag_gui_draw_lost = false;
}


AT(.text.gui)
ALIGNED(256)
void gui_process(void)
{
    bool flag_refresh;
    if (gui_cb.flag_gui_need_refresh) {
        GLOBAL_INT_DISABLE();
        if (gui_cb.flag_gui_drawing) {
            flag_refresh = false;
        } else if (!gui_cb.flag_gui_need_draw) {
            //未刷新，或已刷新未开始绘图且TE时间有余，则重新刷新到最新
            gui_cb.flag_gui_need_draw = false;
            flag_refresh = true;
        } else {
            flag_refresh = false;
        }
        GLOBAL_INT_RESTORE();
        if (flag_refresh) {
            gui_frame_refresh();
            gui_cb.flag_gui_need_refresh = false;
            gui_cb.flag_gui_need_draw = true;
            GLOBAL_INT_DISABLE();
            if (gui_cb.flag_gui_draw_lost) {
                os_gui_draw();
            }
            GLOBAL_INT_RESTORE();
        }
    }
}

AT(.com_text.thread.driver)
void thread_driver_gui_event_callback(uint8_t msg)
{
    switch (msg) {
    case THREAD_DRIVER_MSG_GUI_DRAW:
        //定时器或TE信号，刷新GUI页面
        tft_frame_start();
        gui_frame_draw();
        tft_frame_end();
        gui_cb.flag_gui_drawing = false;            //绘图完成
        break;

    default:
        break;
    }
}

