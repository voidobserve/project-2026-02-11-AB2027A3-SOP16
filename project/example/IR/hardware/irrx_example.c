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

#include "driver_ir.h"
#include "driver_gpio.h"
#include "ir_example.h"

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
void irrx_irq(void)
{
    if (GETF_REG_IRRXCON_KEYRELS() && GETF_REG_IRRXCON_RXPND()) {
        SETF_REG_IRRXCPND_CRXSPND();
        SETF_REG_IRRXCPND_CRELSPND();
        
        irrx_done_set();
    }
}

void irrx_io_init(void)
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
void ir_clk_out_init(void)
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

void irrx_decode_example(void)
{
    irrx_param_t irrx_param;

    irrx_param.wakup_en = IRRX_WAKUP_SLEEP_DISABLE;
    irrx_param.data_format = IRRX_DATA_SELECT_32BIT;
    irrx_param.int_en = IRRX_INT_ENABLE;
    irrx_param.irrx_en = IRRX_ENABLE;
    irrx_param.clk_sel = IR_CLK_X24MDIV;

    // irrx io init
    irrx_io_init();
    
    //ir_clk_out_init();
    
    irrx_decode_init(IRRX, (const irrx_param_t *)&irrx_param);

    sys_irq_init(IRQn_IR_QDEC_LEDC, 0, irrx_irq);
}

void irrx_wake_example(void)
{
    irrx_param_t irrx_param;

    irrx_param.wakup_en = IRRX_WAKUP_SLEEP_ENABLE;
    irrx_param.data_format = IRRX_DATA_SELECT_32BIT;
    irrx_param.int_en = IRRX_INT_ENABLE;
    irrx_param.clk_sel = IR_CLK_X24MDIV;

    // irrx io init
    irrx_io_init();

    irrx_wake_init(IRRX, (const irrx_param_t *)&irrx_param);
    
    LPMCON |= BIT(0);//sleep mode

    printf("irrx wakeup\n");
}

void irrx_example_init(uint8_t irrx_example_mode)
{
    switch (irrx_example_mode) {
        case 1:
            irrx_decode_example();
            break;
        case 2:
            irrx_wake_example();
            break;
        default:
            break;
    }
}

/**
  * @brief  irrx fiq hanlder.Call it in main
  * @retval None
  */
void irrx_handler_example(void)
{
    if (irrx_done_get()) {
        printf ("%s, IRRXDAT = %08X\n", __func__, GET_REG_IRRXDAT());
        irrx_done_clr();

        // IR tx repeat test
        irtx_repeat_send();
    }
}

