/**
******************************************************************************************************************************
*
*@file irrx_example.c
*
*@brief Source file for IR rx example
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/

#include "include.h"
#include "driver_ir.h"
#include "driver_gpio.h"

#if (BSP_IR_EN)

static uint8_t irrx_done = 0;

AT(.com_text.irq)
void irrx_done_set(void)
{
    irrx_done = 1;
}

void irrx_done_clr(void)
{
    irrx_done = 0;
}

uint8_t irrx_done_get(void)
{
    return irrx_done;
}

AT(.com_text.irq)
void bsp_irrx_irq(void)     //解码中断
{
    /*
    if(GETF_REG_IRRXCON_KEYRELS() && GETF_REG_IRRXCON_RXPND())
    {
        SETF_REG_IRRXCPND_CRELSPND();
        SETF_REG_IRRXCPND_CRXSPND();

        irrx_done_set();
    }
    */
}

void bsp_irrx_io_init(void)
{
    gpio_init_typedef gpio_init_structure = {0};

    gpio_init_structure.gpio_pin = IRRX_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    //gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IRRX_IO_PORT, &gpio_init_structure);

    gpio_func_mapping_config(IRRX_IO_PORT, IRRX_IO_PIN, GPIO_CROSSBAR_IN_IRRX);
}

/*
void bsp_ir_clk_out_init(void)
{
    gpio_init_typedef gpio_init_structure = {0};

    gpio_init_structure.gpio_pin = IRCLK_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    //gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IRCLK_IO_PORT, &gpio_init_structure);

    gpio_func_mapping_config(IRCLK_IO_PORT, IRCLK_IO_PIN, GPIO_CROSSBAR_OUT_CLKOUT);
}
*/

void bsp_irrx_decode_init(void)
{
    irrx_param_t irrx_param;

    irrx_param.wakup_en = IRRX_WAKUP_SLEEP_ENABLE;
    irrx_param.data_format = IRRX_DATA_SELECT_32BIT;
    irrx_param.int_en = IRRX_INT_DISABLE;
    irrx_param.irrx_en = IRRX_ENABLE;
    irrx_param.clk_sel = IR_CLK_X24MDIV;

    printf ("%s\r\n", __func__);
    // irrx io init
    bsp_irrx_io_init();

    // ir clock io init
    //bsp_ir_clk_out_init();

    irrx_decode_init(IRRX, (const irrx_param_t *)&irrx_param);

    //sys_irq_init(IRQn_IR_QDEC_LEDC, 0, bsp_irrx_irq);
}

/**
  * @brief  bsp_irrx_handler_process.Call it in func_process
  * @retval None
  */
void bsp_irrx_handler_process(void)
{
    static uint32_t irrx_query_tick = 0;
    static uint16_t ir_press_cnt = 0;
    
    if (tick_check_expire(irrx_query_tick, 10)) {
        irrx_query_tick = tick_get();
        if(GETF_REG_IRRXCON_RXPND()) {
            ir_press_cnt ++;
            //在release pnd到来之前检测是否是长按
            if(GETF_REG_IRRXCON_KEYRELS()) {
                uint32_t irrx_dat = GET_REG_IRRXDAT();
                printf ("%s, irrx_dat = %08X, ir_press_cnt %d\n", __func__, irrx_dat, ir_press_cnt);
            
                if (ir_press_cnt > 200) {
                    irrx_dat |= 0xFF;       //最低位或上0xff，以区分长按短按
                    printf ("%s, key long press, irrx_dat = %08X\n", __func__, irrx_dat);
                }

                #if BSP_LEDC_EN
                if (irrx_dat == 0x50000908) {
                    bsp_ws2815_app_set(1);
                } else if (irrx_dat == 0x50000900) {
                    bsp_ws2815_app_set(0);
                }
                #endif
                
                ir_press_cnt = 0;
                SETF_REG_IRRXCPND_CRELSPND();
                SETF_REG_IRRXCPND_CRXSPND();
            }
        } else {
            ir_press_cnt = 0;
        }
    }
}

#endif //#if (BSP_IR_EN)
