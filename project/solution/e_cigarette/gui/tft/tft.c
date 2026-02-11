#include "include.h"
#include "driver_spi.h"
#include "bsp_pwm.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


AT(.com_text.tft_spi)
static void tft_te_refresh(void)
{
    compo_cb.rtc_update = true;
    os_gui_draw();
}


AT(.com_text.tft_spi)
void tft_soft_te_tick(void)
{
    static u8 cnt = 0;
    cnt++;
    if (cnt % 16 == 0) {     //todo: 默认16ms推一次屏, 调整撕裂需调整frame rate
        cnt = 0;
        tft_te_refresh();
        if (sys_cb.te_bglight_cnt > 0) {
            sys_cb.te_bglight_cnt--;
            if (sys_cb.te_bglight_cnt == 0) {
                sys_cb.tft_bglight_frist_set = true;
            }
        }
    }
}

AT(.com_text.tft_spi)
void tft_frame_start(void)
{
    tft_write_data_start();
}

AT(.com_text.tft_spi)
void tft_frame_end(void)
{
    tft_write_end();
    if (sys_cb.tft_bglight_kick)
    {
        sys_cb.tft_bglight_kick = false;
        sys_cb.te_bglight_cnt = 5; //5TE后打开背光
    }
}


//设置背光亮度
void tft_bglight_set_level(uint8_t level, bool stepless_en)
{
    int8_t  base_duty = 0;  //根据限流电阻调整占空比增益
    int8_t  duty = 0;

    if(!stepless_en)
    {
        level = level * (100 / 5);
    }
    if(100 < level)
    {
        level = 100;
    }
    duty = base_duty + level;
    sys_cb.tft_bglight_duty = duty;

    if (sys_cb.tft_bglight_last_duty != sys_cb.tft_bglight_duty)
    {
        bsp_pwm_duty_set(sys_cb.tft_bglight_duty);
        sys_cb.tft_bglight_last_duty = sys_cb.tft_bglight_duty;
    }
}

//背光亮度初始设置检测
void tft_bglight_frist_set_check(void)
{
    if(!sys_cb.tft_bglight_frist_set) {
        return ;
    }

    printf("%s\n", __func__);
    sys_cb.tft_bglight_frist_set = false;

    sys_cb.tft_bglight_last_duty = 0;
    //todo:后续根据客户定制调整
    if(0 == sys_cb.tft_bglight_duty)
    {
        sys_cb.tft_bglight_duty = 100;
    }
    tft_bglight_set_level(sys_cb.tft_bglight_duty, true);

}

void tft_init(void)
{
    spi_init_typedef spi_init_structure;

    port_tft_init();
    port_tft_reset();

    //SPI init
    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);
    spi_init_structure.baud = 13000000;        
    spi_init_structure.bus_mode = SPI_2WIRE_MODE;
    spi_init_structure.role_mode = SPI_ROLE_MODE_MASTER;
    spi_init_structure.output_data_edge = SPI_OUTPUT_DATA_EDGE_FALLING;
    spi_init_structure.sample_clk_edge = SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF;
    spi_init(SPI1, &spi_init_structure);
    spi_cmd(SPI1, ENABLE);
    spi_pic_config(SPI1, tft_spi_isr, 0, ENABLE);
    TRACE("TFT ID: %x\n", tft_read_id());

#if (GUI_SELECT == GUI_TFT_80_ST7735)
    tft_80_st7735_init();
#elif (GUI_SELECT == GUI_TFT_172_320)
    tft_172_320_init();
#endif

    tft_set_window(0, 0, GUI_SCREEN_WIDTH - 1, GUI_SCREEN_HEIGHT - 1);

    bsp_timer3_pwm_init();          //BL
    sys_cb.tft_bglight_kick = true; //延时打开背光
}

void tft_exit(void)
{
    bsp_timer3_pwm_deinit();
    port_tft_exit();
    spi_deinit(SPI1);
}

