#include "include.h"
#include "driver_gpio.h"
#if BSP_IR_EN
#include "driver_ir.h"
#endif

#if (BSP_LEDC_EN)
#include "driver_ledc.h"
#include "driver_tmr.h"

uint32_t conversion_color = 0;
AT(.com_rodata)
uint32_t color_buf[LEDC_FRAME_LEN] = {0};
AT(.com_rodata)
uint32_t code_buf[LEDC_RGB_CODE_LEN] = {0};

#if DOUBLE_BUFFER_SWITCH
AT(.com_rodata)
uint32_t code_buf1[LEDC_RGB_CODE_LEN] = {0};
AT(.com_text)
volatile unsigned char switch_buf_state = 0;
#endif

volatile unsigned int timer_count = 0;

volatile unsigned char fill_state = 0;
volatile unsigned char ledc_kick_state = 0;

static uint8_t bsp_ws2815_app_en = 0;

uint32_t sysclk_sel_tbl[] = {2000, 16000, 24000, 48000, 60000, 80000, 120000, 160000};

void bsp_ledc_start(void)
{
    ledc_cmd(ENABLE);
}

void bsp_ledc_stop(void)
{
    ledc_cmd(DISABLE);
}

void bsp_ws2815_app_set(uint8_t enable)
{
    bsp_ws2815_app_en = enable;

    printf("bsp_ws2815_app_en %x\n", bsp_ws2815_app_en);

    #if 0
    if (enable) {
        bsp_ledc_start();
    }else {
        bsp_ledc_stop();
    }
    #endif
}
/***********************Hardware Init***************************/
AT(.com_text.isr)
void bsp_basetimer_isr(void)
{
    if (TMR1CON & (1 << 9)) {
        TMR1CPND = 1 << 9;
        timer_count++;


        if((timer_count >= DMA_TICK_INTERVAL) && (ledc_kick_state == 1)){
            timer_count = 0;
            ledc_kick();
        }
    }
}

void bsp_basetimer_init(void)
{
    clk_gate0_cmd(CLK_GATE0_TMR1, CLK_EN);

    tmr_base_init_typedef tmr_base_init_struct;

    tmr_base_init_struct.clock_source = TMR_SYSTEM_CLOCK;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;                   // 1MHz
    tmr_base_init_struct.period = sysclk_sel_tbl[SYS_CLK_SEL] - 1;          // 5Hz
    tmr_base_init(TMR1, &tmr_base_init_struct);

    tmr_pic_config(TMR1, bsp_basetimer_isr, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR1, ENABLE);
}

AT(.com_text.isr)
void bsp_ledc_isr(void)
{
    //we should clear pending flag no matter which pending comes.
    //if (ledc_get_flag(LEDC_FLAG_DMA)) {
    if (1) {
        if (ledc_get_flag(LEDC_FLAG_DONE)) {
            ledc_clear_flag(LEDC_FLAG_DONE);
            ledc_kick_state = 0;
        }

        if (ledc_get_flag(LEDC_FLAG_RST)) {
            ledc_clear_flag(LEDC_FLAG_RST);
        }

        if (ledc_get_flag(LEDC_FLAG_DMA)) {
            ledc_clear_flag(LEDC_FLAG_DMA);

            #if (SAVE_MEMORY_ENABLE)
            #if (DOUBLE_BUFFER_SWITCH)

            if (switch_buf_state == 0) {
                ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);
            }else if (switch_buf_state == 1) {
                ledc_dma_kick((u32)&code_buf1[0], LEDC_RGB_CODE_LEN);
            }
            #else

            ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);

            #endif//#if DOUBLE_BUFFER_SWITCH
            #endif//#if SAVE_MEMORY_ENABLE
        }
    }
}

AT(.com_text.irq)
void bsp_ledc_irq(void)     //解码中断
{
    //#if (BSP_IR_EN)
    //bsp_irrx_irq();
    //#endif

    bsp_ledc_isr();
}

/*************************ws2815 software application******************************/
static uint32_t ws2815_rgb_color_format_conversion(uint32_t display_color,
                                                unsigned char display_format)
{
    uint32_t color_temp = 0;

    switch (display_format) {
        case LEDC_RGB://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 8);
            display_color |= ((color_temp & 0x00ff00) << 8);
            display_color |= (color_temp & 0x0000ff);
            break;
        case LEDC_GRB://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            //格式一致，无需转换
            break;
        case LEDC_RBG://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 8);
            display_color |= ((color_temp & 0x00ff00) >> 8);
            display_color |= ((color_temp & 0x0000ff) << 16);
            break;
        case LEDC_GBR://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= (color_temp & 0xff0000);
            display_color |= ((color_temp & 0x00ff00) >> 8);
            display_color |= ((color_temp & 0x0000ff) << 8);
            break;
        case LEDC_BGR://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 16);
            display_color |= ((color_temp & 0x00ff00) << 8);
            display_color |= ((color_temp & 0x0000ff) << 8);
            break;
        case LEDC_BRG://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 16);
            display_color |= (color_temp & 0x00ff00);
            display_color |= ((color_temp & 0x0000ff) << 16);
            break;
    }

    return display_color;
}

static void ws2815_rgb_color_out_32to24bit_code_fill(uint32_t buf_code_grb[],uint32_t buf_grb[])
{//小端格式
    unsigned int i = 0,i_add = 0;
    unsigned char j = 1;
    uint32_t but_temp = 0;

    uint32_t loop_cnt = LEDC_RGB_CODE_LEN / 3 * 4;

    for (i = 0; i < loop_cnt; i++) {
        switch (j) {
            case 0:
                buf_code_grb[i_add-1] = buf_grb[i];
                i_add--;
                j = j+2;
                break;
            case 1:
                buf_code_grb[i_add] = buf_grb[i];
                j++;
                break;
            case 2:
                but_temp = (buf_grb[i] & 0x000000ff);
                buf_code_grb[i_add-1] |= (but_temp << 24);
                buf_code_grb[i_add] = (buf_grb[i] >> 8);
                j++;
                break;
            case 3:
                but_temp = (buf_grb[i] & 0x0000ffff);
                buf_code_grb[i_add-1] |= (but_temp << 16);
                buf_code_grb[i_add] = (buf_grb[i] >> 16);
                j++;
                break;
            case 4:
                but_temp = (buf_grb[i] & 0x00ffffff);
                buf_code_grb[i_add-1] |= (but_temp << 8);
                buf_code_grb[i_add] = (buf_grb[i] >> 24);
                j = 0;
                break;
        }

        i_add++;
    }
}

void bsp_ws2815_ledc_init(uint8_t display_format)
{
    gpio_init_typedef gpio_init_structure;
    ledc_init_typedef ledc_init_struct;
    ledc_timing_init_typedef ledc_timing_init_struct;

    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_EN);
    clk_ledc_clk_set(CLK_LEDC_CLK_XOSC24M);

    bsp_basetimer_init();

    /* LEDC Base Ctrl Init */
    ledc_init_struct.output_invert = LEDC_OUT_INV_DIS;
    ledc_init_struct.format = LEDC_IN_FORMAT_RGB;
    ledc_init_struct.idle_level = LEDC_IDLE_LEVEL_LOW;
    ledc_init_struct.valid_data_mode = LEDC_VALID_DATA_DIR;
    ledc_init_struct.byte_inv_number = LEDC_BYTE_INV_1B;
    ledc_init(&ledc_init_struct);

    /* LEDC Timing Init */
    ledc_timing_init_struct.baud = 30 - 1;
    ledc_timing_init_struct.delay = 8;
    ledc_timing_init_struct.code_0_high = 8;
    ledc_timing_init_struct.code_1_high = 22;
    ledc_timing_init_struct.reset_high = 20;
    ledc_timing_init_struct.reset_low = 400;
    ledc_timing_init(&ledc_timing_init_struct);

    /* GPIO Init */
    gpio_init_structure.gpio_pin = GPIO_PIN_1;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;

    /* Mapping Config */
    gpio_init(GPIOA_REG, &gpio_init_structure);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_1, GPIO_CROSSBAR_OUT_LEDCDAT);

    /* Register interrupt, this interrupt source is not used by the module alone */
    /* Adjust the interrupt processing logic of related modules properly */
    ledc_pic_config(bsp_ledc_irq, 0, LEDC_FLAG_RST | LEDC_FLAG_DMA, ENABLE);

    /* Initialize procedure parameters */
    ledc_set_frame_length(LEDC_FRAME_LEN);
    ledc_set_loop_number(0);

    /* Startup module */
    ledc_cmd(ENABLE);
}

void bsp_ws2815_ledc_exec(uint8_t display_format)
{
    if (!bsp_ws2815_app_en) {
        return;
    }

    uint16_t i = 0;

    if (ledc_kick_state == 0) {


        switch (fill_state) {
            case 0:
                conversion_color = ws2815_rgb_color_format_conversion(SECOND_DISPLAY_COLOR,display_format);
                fill_state++;
                break;
            case 1:
                conversion_color = ws2815_rgb_color_format_conversion(THIRD_DISPLAY_COLOR,display_format);
                fill_state++;
                break;
            case 2:
                conversion_color = ws2815_rgb_color_format_conversion(FIRST_DISPLAY_COLOR,display_format);
                fill_state = 0;
                break;
        }

        for (i = 0; i < LEDC_FRAME_LEN; i++) {
            color_buf[i] = conversion_color;
        }
        #if DOUBLE_BUFFER_SWITCH
        if (switch_buf_state == 0) {
            ws2815_rgb_color_out_32to24bit_code_fill(&code_buf1[0],&color_buf[0]);
            switch_buf_state = 1;
            ledc_dma_kick((u32)&code_buf1[0], LEDC_RGB_CODE_LEN);
        }else if (switch_buf_state == 1) {
            ws2815_rgb_color_out_32to24bit_code_fill(&code_buf[0],&color_buf[0]);
            switch_buf_state = 0;
            ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);
        }

        #else

        ws2815_rgb_color_out_32to24bit_code_fill(&code_buf[0],&color_buf[0]);

        ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);

        #endif
        ledc_kick_state = 1;
    }
}

#endif  //#if (BSP_LEDC_EN)


