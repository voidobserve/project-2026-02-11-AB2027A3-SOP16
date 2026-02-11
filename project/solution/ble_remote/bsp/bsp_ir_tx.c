/**
******************************************************************************************************************************
*
*@file irtx_example.c
*
*@brief Source file for IR tx example
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/

#include "include.h"
#include "driver_ir.h"
#include "driver_gpio.h"

#if (BSP_IR_EN)
// User should care about that the allocated address should be in ram
AT(.ble_buf.irtx_dma)
uint32_t bsp_irtx_capture_buf[0x90/4];
static uint8_t bsp_irtx_cap_done = 0;

AT(.com_text.irq)
void bsp_irtx_cap_done_set(void)
{
    bsp_irtx_cap_done = 1;
}

void bsp_irtx_cap_done_clr(void)
{
    bsp_irtx_cap_done = 0;
}

uint8_t bsp_irtx_cap_done_get(void)
{
    return bsp_irtx_cap_done;
}

AT(.com_text.irq)
void bsp_irtx_irq(void)
{
    if (GETF_REG_IRDMAPEND_WDMA_APND()) {
        SETF_REG_IRDMAPEND_WDMA_APND();
    
        bsp_irtx_cap_done_set();
    }
    
    if (GETF_REG_IRDMAPEND_RDMA_APND()) {
        SETF_REG_IRDMAPEND_RDMA_APND();
    }  
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

void bsp_irtx_io_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = IRTX_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    //gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    
    gpio_init(IRTX_IO_PORT, &gpio_init_structure);
    
    gpio_func_mapping_config(IRTX_IO_PORT, IRTX_IO_PIN, GPIO_CROSSBAR_OUT_IRTX);
}  

void bsp_irtx_tx_init(void)
{
    irtx_param_t irtx_param;

    irtx_param.clk_sel = IR_CLK_X24MDIV;
    irtx_param.encode_format = IRTX_ENCODE_FORMAT_NEC;
    irtx_param.tx_data = IR_TX_DATA;
    irtx_param.key = IRTX_KEY_RELEASED;
    
    // Carrier Config
    irtx_param.carrier_config.carrier_en = IRTX_CARRIER_ENABLE;    
    irtx_param.carrier_config.mod_clksel = IRTX_MODE_CLKSEL_4M;
    irtx_param.carrier_config.ircw_duty = 79 / 3;
    irtx_param.carrier_config.ircw_length = 110;

    // Filter Config    
    irtx_param.filter_config.filter_en = IRTX_FILTER_ENABLE;
    irtx_param.filter_config.filter_len = 3;
    
    irtx_param.sync_code = IRTX_SYNC_OR_TRAILER_CODE_1;
    irtx_param.invert_en = IRTX_INVERT_OUTPUT_ENABLE;

    printf("%s init\n", __func__);

    // irtx io init
    bsp_irtx_io_init();

    irtx_base_init(IRTX, (const irtx_param_t *)&irtx_param);        
    
    //irtx_send();
}

void bsp_itx_send_data(uint32_t code)
{
    irtx_data_set(IRTX, code);
    
    irtx_send();
}

void bsp_irtx_capture_init(void)
{
    printf ("%s\r\n", __func__);

    irtx_param_t irtx_param;

    irtx_param.clk_sel = IR_CLK_X24MDIV;
    irtx_param.cap_edge = IRTX_CAP_EDGE_FALLING;

    /*Init IR RX To Capture IR Code*/
    bsp_irrx_io_init();

    /*Init IR TX To Transmit IR Code*/
    bsp_irtx_io_init();

    // Capture Init
    irtx_capture_init(IRTX, (const irtx_param_t *)&irtx_param);
    irtx_capture_buf_init(IRTX, bsp_irtx_capture_buf, sizeof(bsp_irtx_capture_buf));
    sys_irq_init(IRQn_IR_QDEC_LEDC, 0, bsp_irtx_irq);
}

void bsp_irtx_mode_init(bsp_irtx_mode_t mode)
{
    if (IRTX_MODE_SEND == mode) {
        bsp_irtx_tx_init();
    } else if (IRTX_MODE_CAPTURE == mode) {
        bsp_irtx_capture_init();
    }
}

/**
  * @brief  irtx_capture_handler_example.Call it in main
  * @retval None
  */
void bsp_irtx_capture_handler_process(void)
{
    if (bsp_irtx_cap_done_get()) {
        printf("\n");
        print_r(bsp_irtx_capture_buf, sizeof(bsp_irtx_capture_buf));
        
        bsp_irtx_cap_done_clr();

        // Learn IR Code and Transmit in IR TX capture mode, Only for test
        irtx_learn_buf_init(IRTX, bsp_irtx_capture_buf, sizeof(bsp_irtx_capture_buf));
    }
}
#endif //#if (BSP_IR_EN)
