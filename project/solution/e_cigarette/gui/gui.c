#include "include.h"

#if GUI_SELECT

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//推屏缓存大小(双份)
#define GUI_LINES_BUF_SIZE              (GUI_SCREEN_WIDTH * GUI_LINES_CNT * 2 * 2)

//以下缓存大小一般情况下不需要修改
#define GUI_ELE_BUF_SIZE                1024                    //element缓存
#define GUI_WGT_BUF_SIZE                2048                    //widget缓存
#define GUI_MAXSIZE_TEMPBUF             3072                    //中间临时计算缓存3k
#define GUI_MAXSIZE_RLEBUF              512                     //RLE解码缓存
#define GUI_MAX_FONT_SIZE               128                     //单个字最大尺寸

static u8 gui_lines_buf[GUI_LINES_BUF_SIZE] AT(.disp.buf);      //推屏缓存(双份)
static u8 gui_temp_buf[GUI_MAXSIZE_TEMPBUF] AT(.disp.buf);      //中间计算缓存
static u8 gui_element_buf[GUI_ELE_BUF_SIZE] AT(.disp.buf);      //Element Buf
static u8 gui_widget_buf[GUI_WGT_BUF_SIZE] AT(.disp.buf);       //Widgets Buf

//GUI初始化配置表
static const gui_init_param_t tbl_gui_init_param = {
    .screen_width = GUI_SCREEN_WIDTH,
    .screen_height = GUI_SCREEN_HEIGHT,
    .element_buf = gui_element_buf,
    .widget_buf = gui_widget_buf,
    .element_buf_size = GUI_ELE_BUF_SIZE,
    .widget_buf_size = GUI_WGT_BUF_SIZE,
    .temp_buf = gui_temp_buf,
    .temp_buf_size = GUI_MAXSIZE_TEMPBUF,
    .lines_buf = gui_lines_buf,
    .lines_buf_size = GUI_LINES_BUF_SIZE,
    .lines_count = GUI_LINES_CNT,
    .maxsize_rlebuf = GUI_MAXSIZE_RLEBUF,
    .font_res_addr = UI_BUF_0FONT_FONT_BIN,
    .max_font_size = GUI_MAX_FONT_SIZE,
    .font_wspace = GUI_FONT_W_SPACE,
    .font_hspace = GUI_FONT_H_SPACE,
};

//GUI相关初始化
void gui_init(void)
{
    ctp_init();
    tft_init();
    os_gui_init(&tbl_gui_init_param);
    compos_init();
//    sys_cb.sleep_en = 1;            //允许进休眠
//    sys_cb.gui_sleep_sta = 0;
}

void gui_sleep(void)
{
//    if (!sys_cb.gui_sleep_sta) {
//        os_gui_draw_w4_done();      //关tft前要等当前帧刷完
//        tft_exit();
//        ctp_exit();
//        power_gate_3v3_off();
//        sys_cb.gui_sleep_sta = 1;
//        //printf("gui_sleep\n");
//    }
}

void gui_wakeup(void)
{
//    if (sys_cb.gui_sleep_sta) {
//        power_gate_3v3_on();
//        //tft_init();
//        ctp_init();
//        tft_init();
//        gui_widget_refresh();
//        sys_cb.gui_sleep_sta = 0;
//        //printf("gui_wakeup\n");
//    }
}

//蓝屏
AT(.com_text.hwio)
void gui_halt(u32 halt_no)
{
    int i;
    LCD_BL_EN();               //打开背光
    tft_frame_end();
    for (i=0; i<20000; i++) {
        asm("nop");                  //足够的延时，保证前面SPI推完
    }
    tft_frame_start();
    for (i=0; i<GUI_SCREEN_HEIGHT; i++) {
        gui_fill_rgb565(gui_lines_buf, COLOR_BLUE, GUI_SCREEN_WIDTH);
        if (i >= GUI_SCREEN_CENTER_Y - 14 && i < GUI_SCREEN_CENTER_Y + 14) {
            gui_fill_num(gui_lines_buf + GUI_SCREEN_CENTER_X * 2 - 44, halt_no, i - (GUI_SCREEN_CENTER_Y - 14));
        }
        tft_spi_send(gui_lines_buf, GUI_SCREEN_WIDTH * 2);
    }
    tft_frame_end();
}

#else
void gui_sleep(void){}
void gui_wakeup(void){}
#endif  // GUI_SELECT
