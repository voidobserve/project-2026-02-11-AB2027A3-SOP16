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

#include "driver_ir.h"
#include "driver_gpio.h"
#include "ir_example.h"


extern void irrx_io_init(void);

// User should care about that the allocated address should be in ram
AT(.ble_buf.irtx_dma)
uint32_t irtx_capture_buf[0x90/4];
AT(.ble_buf.irtx_learn_dma)
uint16_t irtx_learn_buf[0x90/2];
uint8_t irtx_learn_buf_idx = 0;


uint16_t ir_learn_rx_buf[128];
uint16_t ir_learn_tx_buf[128];

static uint8_t irtx_cap_done = 0;

AT(.com_text.irq)
void irtx_cap_done_set(void)
{
    irtx_cap_done = 1;
}

void irtx_cap_done_clr(void)
{
    irtx_cap_done = 0;
}

uint8_t irtx_cap_done_get(void)
{
    return irtx_cap_done;
}

AT(.com_text.irq)
void irtx_irq(void)
{
    if (GETF_REG_IRDMAPEND_WDMA_APND()) {
        SETF_REG_IRDMAPEND_WDMA_APND();

        irtx_cap_done_set();
    }

    if (GETF_REG_IRDMAPEND_RDMA_APND()) {
        SETF_REG_IRDMAPEND_RDMA_APND();
    }

    if (GETF_REG_IRDMAPEND_RDMA_HPND()) {
        SETF_REG_IRDMAPEND_RDMA_HPND();
    }

    if (GETF_REG_IRTXPEND_IRTX_PND()) {
        SETF_REG_IRTXPEND_IRTX_PND();
    }
}

void irtx_io_init(void)
{
    //mapping_config 必须在GPIO初始化之前，否则发送红外数据会有电平异常
    gpio_func_mapping_config(IRTX_IO_PORT, IRTX_IO_PIN, GPIO_CROSSBAR_OUT_IRTX);

    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = IRTX_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    //gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IRTX_IO_PORT, &gpio_init_structure);


}

void irtx_send_example_init(void)
{
    irtx_param_t irtx_param;

    printf("%s\n", __func__);

    irtx_param.clk_sel = IR_CLK_X24MDIV;
    irtx_param.encode_format = IRTX_ENCODE_FORMAT_NEC;
    irtx_param.tx_data = IR_TX_DATA;
    irtx_param.key = IRTX_KEY_PRESSED;

    // Carrier Config
    irtx_param.carrier_config.carrier_en = IRTX_CARRIER_DISABLE;
    irtx_param.carrier_config.mod_clksel = IRTX_MODE_CLKSEL_4M;
    irtx_param.carrier_config.ircw_duty = 79 / 3;
    irtx_param.carrier_config.ircw_length = 110;

    // Filter Config
    irtx_param.filter_config.filter_en = IRTX_FILTER_ENABLE;
    irtx_param.filter_config.filter_len = 3;

    irtx_param.sync_code = IRTX_SYNC_OR_TRAILER_CODE_1;
    irtx_param.invert_en = IRTX_INVERT_OUTPUT_DISABLE;

    // irtx io init
    irtx_io_init();

    irtx_base_init(IRTX, (const irtx_param_t *)&irtx_param);
}

void irtx_learn_example_init(void)
{
    printf ("%s\n", __func__);

    irtx_param_t irtx_param;

    irtx_param.clk_sel = IR_CLK_X24MDIV;
    irtx_param.cap_edge = IRTX_CAP_EDGE_FALLING;

    // Carrier Config
    irtx_param.carrier_config.carrier_en = IRTX_CARRIER_ENABLE;
    irtx_param.carrier_config.mod_clksel = IRTX_MODE_CLKSEL_4M;
    irtx_param.carrier_config.ircw_duty = 79 / 3;
    irtx_param.carrier_config.ircw_length = 110;
    irtx_param.dma_config.rdma_mode=IRTX_DMA_MODE_16BIT;
    irtx_param.dma_config.wdma_mode=IRTX_DMA_MODE_16BIT;
    /*Init IR RX To Capture IR Code*/
    irrx_io_init();

    /*Init IR TX To Transmit IR Code*/
    irtx_io_init();

    // Capture Init
    irtx_capture_init(IRTX, (const irtx_param_t *)&irtx_param);
    irtx_capture_buf_init(IRTX, ir_learn_rx_buf, sizeof(ir_learn_rx_buf)/sizeof(ir_learn_rx_buf[0]));

    irtx_softmodulation_init(IRTX, (const irtx_param_t *)&irtx_param);

    irtx_pic_config(irtx_irq, 0, IRTX_FLAG_RDMA_APND | IRTX_FLAG_WDMA_APND, ENABLE);
}

void irtx_extension_d7c6_example_init(void)
{
    irtx_param_t irtx_param;

    printf("%s\n", __func__);

    irtx_param.clk_sel = IR_CLK_X24MDIV;

    // Carrier Config
    irtx_param.carrier_config.carrier_en = IRTX_CARRIER_ENABLE;
    irtx_param.carrier_config.mod_clksel = IRTX_MODE_CLKSEL_4M;
    irtx_param.carrier_config.ircw_duty = 99 / 3;
    irtx_param.carrier_config.ircw_length = 99;
    irtx_param.dma_config.rdma_mode = IRTX_DMA_MODE_16BIT;
    // irtx io init
    irtx_io_init();

    irtx_softmodulation_init(IRTX, (const irtx_param_t *)&irtx_param);

    irtx_pic_config(irtx_irq, 0, IRTX_FLAG_RDMA_APND, ENABLE);
}

void irtx_example_init(uint8_t irtx_example_mode)
{
    switch (irtx_example_mode) {
        case 0:
            irtx_send_example_init();
            break;
        case 1:
            irtx_learn_example_init();
            break;
        case 2:
            irtx_extension_d7c6_example_init();
            break;
    }
}


void irtx_send_handler_example(void)
{
    static uint32_t irtx_send_tick = 0;
    static uint32_t irtx_code = 0;

    if (tick_check_expire(irtx_send_tick, 100)) {
        irtx_send_tick = tick_get();
        irtx_code ++;

        irtx_data_set(IRTX, irtx_code);

        irtx_send();
    }
}

static int ir_learn_data_len_get(uint16_t *data_buf)
{

	uint16_t ir_data_len=0;

	for(uint16_t i=0;i<1024;i++)
    {
		if(0x7fff ==data_buf[i]&&0x7fff ==data_buf[i+1]&&0x7fff ==data_buf[i+2]&&0x7fff ==data_buf[i+3])
		{
			ir_data_len=i;
			return ir_data_len;
		}
	}
	return 0;
}

/**
  * @brief  irtx_learn_handler_example.Call it in main
  * @retval None
  */
void irtx_learn_handler_example(void)
{
    if (irtx_cap_done_get())
    {
        //print_r(ir_learn_rx_buf, sizeof(ir_learn_rx_buf));
        uint16_t data_len= ir_learn_data_len_get(ir_learn_rx_buf);
        for(uint16_t i=0;i<data_len+1;i++){
          printf("ir_learn_rx_buf[%03d]= %04x\n",i,ir_learn_rx_buf[i]);
        }
        memset(ir_learn_tx_buf,0,sizeof(ir_learn_tx_buf));
        memcpy(ir_learn_tx_buf,ir_learn_rx_buf,sizeof(ir_learn_rx_buf));
        memset(ir_learn_rx_buf,0,sizeof(ir_learn_rx_buf));
        irtx_cap_done_clr();

        // Learn IR Code and Transmit in IR TX capture mode, Only for test
        CLRF_REG_IRDMACON_IR_WDMA_EN();
        irtx_softmodulation_buf_init(IRTX, ir_learn_tx_buf, data_len);
        SETF_REG_IRDMACON_IR_WDMA_EN();

    }
}

void irtx_d7c6_timing_set(uint8_t component_data, uint8_t bit_width)
{
    for (uint8_t bit = 0; bit < bit_width; bit++) {
        uint16_t tick_cnt;
        uint8_t data = component_data >> bit;
        if (data & 0x01) {
            tick_cnt = (TICK_CNT) & ~BIT(15);         //1T low level

            memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &tick_cnt, 2);

            tick_cnt = TICK_CNT | BIT(15);            //1T high level

            memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &tick_cnt, 2);

            tick_cnt = TICK_CNT | BIT(15);            //1T high level

            memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &tick_cnt, 2);
        } else {
            tick_cnt = (TICK_CNT) & ~BIT(15);         //1T low level

            memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &tick_cnt, 2);

            tick_cnt = TICK_CNT | BIT(15);            //1T high level

            memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &tick_cnt, 2);
        }
    }
}

void irtx_component(uint8_t type, uint8_t *pcomponent_data)
{
    uint8_t component_data = *pcomponent_data;
    if (D7C6_PREAMBLE == type) {
        uint16_t preamble = (4 * TICK_CNT) | BIT(15);     //0.6ms high level preamble transfer to CLK tick counter.

        memcpy(&irtx_learn_buf[irtx_learn_buf_idx++], &preamble, 2);
    } else if (D7C6_DATA == type) {

        irtx_d7c6_timing_set(component_data, 7);
    } else if (D7C6_CUSTOM == type) {

        irtx_d7c6_timing_set(component_data, 6);
    }
}

void irtx_sequence_correct(void)
{
    /*ic logical error, application need to fill more 32bits low level data*/
    irtx_learn_buf[0] = 0x0010;
    irtx_learn_buf[1] = 0x0010;
    irtx_learn_buf_idx = 2;
}

void irtx_d7c6_send(uint8_t data, uint8_t custom)
{
    irtx_learn_buf_idx = 0;
    memset(irtx_learn_buf, 0x00, sizeof(irtx_learn_buf));

    irtx_sequence_correct();

    irtx_component(D7C6_PREAMBLE, NULL);
    irtx_component(D7C6_DATA, &data);
    irtx_component(D7C6_CUSTOM, &custom);

    /*we must make sure dma length equl to sequence length*/
    irtx_softmodulation_buf_init(IRTX, irtx_learn_buf, irtx_learn_buf_idx);
}

void irtx_extension_d7c6_handler_example(void)
{
    static uint32_t irtx_learn_tick = 0;
    static uint8_t cnt = 0;

    if (tick_check_expire(irtx_learn_tick, 30)) {
        irtx_learn_tick = tick_get();
        cnt ++;
        irtx_d7c6_send(cnt, cnt + 1);
        //irtx_d7c6_send(0x7f, 0x3f);

        //print_r(irtx_learn_buf, sizeof(irtx_learn_buf));
    }
}





