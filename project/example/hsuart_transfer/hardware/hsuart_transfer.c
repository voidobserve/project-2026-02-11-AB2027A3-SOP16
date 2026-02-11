#include "include.h"
#include "driver_com.h"
#include "driver_hsuart.h"
#include "driver_gpio.h"
#include "hsuart_transfer.h"

#define HSUART_TX_PORT              GPIOA_REG
#define HSUART_TX_PIN               GPIO_PIN_0
#define HSUART_RX_PORT              GPIOA_REG
#define HSUART_RX_PIN               GPIO_PIN_1
#define HSUART_KEY_PORT             GPIOA_REG
#define HSUART_KEY_PIN              GPIO_PIN_7

#define RECV_DMA_BUF_MAX_LEN        64
u8 receive_dma_buf[RECV_DMA_BUF_MAX_LEN];
u8 len = 0;


AT(.com_periph.hsuart.isr)
void hsuart_isr_callback(){

	if(hsuart_get_flag(HSUART_IT_RX)){
		hsuart_clear_flag(HSUART_IT_RX);
		// 原数据发回上位机
		len = hsuart_get_fifo_counter();
	 	hsuart_receive_data_from_fifo(receive_dma_buf, len);
        hsuart_dma_start(HSUT_TRANSMIT, (uint32_t)receive_dma_buf, len);
        while (hsuart_get_flag(HSUART_IT_TX) == RESET);
	}
	// 不定长
	if(hsuart_get_flag(HSUART_IT_RX_TMR_OV)){
		hsuart_clear_flag(HSUART_IT_RX_TMR_OV);
		hsuart_dma_start(HSUT_RECEIVE, (uint32_t)receive_dma_buf, RECV_DMA_BUF_MAX_LEN);
	}
}

// 初始化HSUART
void hsuart_transfer_init(u32 baud)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

    // Rx
    gpio_init_structure.gpio_pin = HSUART_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_RX_PORT, &gpio_init_structure);

    // Tx
    gpio_init_structure.gpio_pin = HSUART_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(HSUART_TX_PORT, &gpio_init_structure);

    // IO Key
    gpio_init_structure.gpio_pin = HSUART_KEY_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_KEY_PORT, &gpio_init_structure);

    gpio_func_mapping_config(HSUART_TX_PORT, HSUART_TX_PIN, GPIO_CROSSBAR_OUT_HURTX);
    gpio_func_mapping_config(HSUART_RX_PORT, HSUART_RX_PIN, GPIO_CROSSBAR_IN_HURRX);

    hsuart_init_struct.baud = baud;
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_LOOP;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_1BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_PLLDIV4_CLK;   // When selecting clock PLLDIV4, ensure that the sys_clk is SYS_60M or higher.
    hsuart_init_struct.one_line_en = HSUT_ONELINE_DIS;
    hsuart_init(&hsuart_init_struct);

    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);

    hsuart_dma_start(HSUT_RECEIVE, (uint32_t)receive_dma_buf, RECV_DMA_BUF_MAX_LEN);

	// 接收中断
	hsuart_pic_config(hsuart_isr_callback,0,HSUART_IT_RX|HSUART_IT_RX_TMR_OV,ENABLE);
	// 空闲中断
	hsuart_rx_idle_config(50,ENABLE);
}

// 测试函数
void hsuart_transfer_example(void)
{
    u8 len;
    u8 recv_buf[10];
    static u8 key_allow = 1;
//    static u32 delay_cnt;

    if (gpio_read_bit(HSUART_KEY_PORT, HSUART_KEY_PIN) == RESET) {
        if (key_allow) {
            key_allow = 0;

            printf("\nkey_down~\n");
            len = hsuart_get_fifo_counter();
            if (len) {
                hsuart_receive_data_from_fifo(recv_buf, len);
                printf("receive data from fifo[%d]: ", len);
                print_r(recv_buf, len);

                hsuart_dma_start(HSUT_TRANSMIT, (uint32_t)recv_buf, len);
                while (hsuart_get_flag(HSUART_IT_TX) == RESET);
            }
        }
    } else {
        key_allow = 1;
    }

//    if (tick_check_expire(delay_cnt, 1000)) {
//        delay_cnt = tick_get();
//        printf("fifo_cnt: %d  dma_buf: ", hsuart_get_fifo_counter());
//        print_r(receive_dma_buf, RECV_DMA_BUF_MAX_LEN);
//    }
//	(void*)delay_cnt;
}
