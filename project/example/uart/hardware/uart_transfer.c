#include "include.h"
#include "driver_gpio.h"
#include "driver_uart.h"
#include "uart_transfer.h"

#if (UART_UDET_EXAMPLE_SEL == UART_EXAMPLE)
uart_transfer_cb_typedef uart_rx = {
    .done = 0,
    .len = 0,
    .timeout = 50, //ms
};


AT(.com_text.isr)
void uart_isr(void)
{
    if (uart_get_flag(UART, UART_IT_RX) != RESET) {
        uart_rx.tick = tick_get();
        if (uart_rx.len < sizeof(uart_rx.buf)) {
            uart_rx.buf[uart_rx.len++] = uart_receive_data(UART);
        }

        uart_clear_flag(UART, UART_IT_RX);
    }
}

void uart_transfer_init(u32 baud)
{
    uart_init_typedef uart_init_struct;
    gpio_init_typedef gpio_init_structure;

    clk_gate0_cmd(CLK_GATE0_UART, CLK_EN);

    // RX
    gpio_init_structure.gpio_pin = UART_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(UART_RX_PORT, &gpio_init_structure);

    // TX
    gpio_init_structure.gpio_pin = UART_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(UART_TX_PORT, &gpio_init_structure);

    gpio_func_mapping_config(UART_RX_PORT, UART_RX_PIN, GPIO_CROSSBAR_IN_UARTRX);
    gpio_func_mapping_config(UART_TX_PORT, UART_TX_PIN, GPIO_CROSSBAR_OUT_UARTTX);

    uart_init_struct.baud = baud;
    uart_init_struct.mode = UART_SEND_RECV_MODE;
    uart_init_struct.word_len = UART_WORD_LENGTH_8b;
    uart_init_struct.stop_bits = UART_STOP_BIT_1BIT;
    uart_init_struct.one_line_enable = UART_SEPARATE;
    /* Only UART Valid */
    uart_init_struct.baud_fix = UART_BAUD_FIX;
    /* Only UART Valid */
    uart_init_struct.clock_source = UART_INC_CLOCK;
    uart_init(UART, &uart_init_struct);

    uart_pic_config(UART, uart_isr, 0, UART_IT_RX, ENABLE);

    uart_cmd(UART, ENABLE);
}

void uart_transfer_example(void)
{
    u8 *p = uart_rx.buf;
    static uint32_t uart_tx_tick = 0;

    if (((tick_get() - uart_rx.tick) >= uart_rx.timeout) && uart_rx.len) {
        //uart_rx.done = 1;
        while (uart_rx.len) {
            uart_send_data(UART, *p++);
            while (uart_get_flag(UART, UART_IT_TX) != SET);
            uart_rx.len --;
        }
    }

    if (tick_check_expire(uart_tx_tick, 1000)) {
        uart_tx_tick = tick_get();

        printf("uart tx test!");
    }
}
#elif (UART_UDET_EXAMPLE_SEL == UDET_EXAMPLE)
uart_transfer_cb_typedef udet_rx = {
    .done = 0,
    .len = 0,
    .timeout = 50, //ms
};

AT(.com_text.isr)
void udet_isr(void)
{
    if (uart_get_flag(UDET, UART_IT_RX) != RESET) {
        udet_rx.tick = tick_get();
        if (udet_rx.len < sizeof(udet_rx.buf)) {
            udet_rx.buf[udet_rx.len++] = uart_receive_data(UDET);
        }

        uart_clear_flag(UDET, UART_IT_RX);
    }
}

void udet_transfer_init(u32 baud)
{
    uart_init_typedef udet_init_struct;
    gpio_init_typedef gpio_init_structure;

    clk_udet_clk_set(XOSC24M_CLK);  //udet 必须调用
    clk_gate0_cmd(CLK_GATE0_UDET, CLK_EN);

    // RX
    gpio_init_structure.gpio_pin = UDET_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(UDET_RX_PORT, &gpio_init_structure);

    // TX
    gpio_init_structure.gpio_pin = UDET_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(UDET_TX_PORT, &gpio_init_structure);

    gpio_func_mapping_config(UDET_RX_PORT, UDET_RX_PIN, GPIO_CROSSBAR_IN_UDETRX);
    gpio_func_mapping_config(UDET_TX_PORT,  UDET_TX_PIN, GPIO_CROSSBAR_OUT_UDETTX);

    udet_init_struct.baud = baud;
    udet_init_struct.mode = UART_SEND_RECV_MODE;
    udet_init_struct.word_len = UART_WORD_LENGTH_8b;
    udet_init_struct.stop_bits = UART_STOP_BIT_1BIT;
    udet_init_struct.one_line_enable = UART_SEPARATE;
    /* Only UDET Valid */
    udet_init_struct.baud_fix = UART_BAUD_AUTO;
    /* Only UDET Valid */
    udet_init_struct.clock_source = UART_INC_CLOCK;
    uart_init(UDET, &udet_init_struct);

    uart_pic_config(UDET, udet_isr, 0, UART_IT_RX, ENABLE);

    uart_cmd(UDET, ENABLE);
}

void udet_transfer_example(void)
{
    u8 *p = udet_rx.buf;
    static uint32_t udet_tx_tick = 0;

    if (((tick_get() - udet_rx.tick) >= udet_rx.timeout) && udet_rx.len) {
        //uart_rx.done = 1;
        while (udet_rx.len) {
            uart_send_data(UDET, *p++);
            while (uart_get_flag(UDET, UART_IT_TX) != SET);
            udet_rx.len --;
        }
    }

    if (tick_check_expire(udet_tx_tick, 1000)) {
        udet_tx_tick = tick_get();

       // printf("udet tx test!");
    }
}

#elif (UART_UDET_EXAMPLE_SEL == UART_UDET_EXAMPLE)
uart_transfer_cb_typedef uart_rx = {
    .done = 0,
    .len = 0,
    .timeout = 50, //ms
};

uart_transfer_cb_typedef udet_rx = {
    .done = 0,
    .len = 0,
    .timeout = 50, //ms
};

AT(.com_text.isr)
void uart_udet_isr(void)
{
    if (uart_get_flag(UART, UART_IT_RX) != RESET) {
        uart_rx.tick = tick_get();
        if (uart_rx.len < sizeof(uart_rx.buf)) {
            uart_rx.buf[uart_rx.len++] = uart_receive_data(UART);
        }

        uart_clear_flag(UART, UART_IT_RX);
    }

    if (uart_get_flag(UDET, UART_IT_RX) != RESET) {
        udet_rx.tick = tick_get();
        if (udet_rx.len < sizeof(udet_rx.buf)) {
            udet_rx.buf[udet_rx.len++] = uart_receive_data(UDET);
        }

        uart_clear_flag(UDET, UART_IT_RX);
    }
}

void uart_transfer_init(u32 baud)
{
    uart_init_typedef uart_init_struct;
    gpio_init_typedef gpio_init_structure;

    clk_gate0_cmd(CLK_GATE0_UART, CLK_EN);

    // RX
    gpio_init_structure.gpio_pin = UART_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(UART_RX_PORT, &gpio_init_structure);

    // TX
    gpio_init_structure.gpio_pin = UART_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(UART_TX_PORT, &gpio_init_structure);

    gpio_func_mapping_config(UART_RX_PORT, UART_RX_PIN, GPIO_CROSSBAR_IN_UARTRX);
    gpio_func_mapping_config(UART_TX_PORT, UART_TX_PIN, GPIO_CROSSBAR_OUT_UARTTX);

    uart_init_struct.baud = baud;
    uart_init_struct.mode = UART_SEND_RECV_MODE;
    uart_init_struct.word_len = UART_WORD_LENGTH_8b;
    uart_init_struct.stop_bits = UART_STOP_BIT_1BIT;
    uart_init_struct.one_line_enable = UART_SEPARATE;
    /* Only UART Valid */
    uart_init_struct.baud_fix = UART_BAUD_FIX;
    /* Only UART Valid */
    uart_init_struct.clock_source = UART_INC_CLOCK;
    uart_init(UART, &uart_init_struct);

    uart_pic_config(UART, uart_udet_isr, 0, UART_IT_RX, ENABLE);

    uart_cmd(UART, ENABLE);
}

void udet_transfer_init(u32 baud)
{
    uart_init_typedef udet_init_struct;
    gpio_init_typedef gpio_init_structure;

    clk_udet_clk_set(XOSC24M_CLK);  //udet 必须调用
    clk_gate0_cmd(CLK_GATE0_UDET, CLK_EN);

    // RX
    gpio_init_structure.gpio_pin = UDET_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(UDET_RX_PORT, &gpio_init_structure);

    // TX
    gpio_init_structure.gpio_pin = UDET_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(UDET_TX_PORT, &gpio_init_structure);

    gpio_func_mapping_config(UDET_RX_PORT, UDET_RX_PIN, GPIO_CROSSBAR_IN_UDETRX);
    gpio_func_mapping_config(UDET_TX_PORT,  UDET_TX_PIN, GPIO_CROSSBAR_OUT_UDETTX);

    udet_init_struct.baud = baud;
    udet_init_struct.mode = UART_SEND_RECV_MODE;
    udet_init_struct.word_len = UART_WORD_LENGTH_8b;
    udet_init_struct.stop_bits = UART_STOP_BIT_1BIT;
    udet_init_struct.one_line_enable = UART_SEPARATE;
    /* Only UDET Valid */
    udet_init_struct.baud_fix = UART_BAUD_FIX;
    /* Only UDET Valid */
    udet_init_struct.clock_source = UART_INC_CLOCK;
    uart_init(UDET, &udet_init_struct);

    uart_pic_config(UDET, uart_udet_isr, 0, UART_IT_RX, ENABLE);

    uart_cmd(UDET, ENABLE);
}

void uart_transfer_example(void)
{
    u8 *p = uart_rx.buf;
    static uint32_t uart_tx_tick = 0;

    if (((tick_get() - uart_rx.tick) >= uart_rx.timeout) && uart_rx.len) {
        //uart_rx.done = 1;
        while (uart_rx.len) {
            uart_send_data(UART, *p++);
            while (uart_get_flag(UART, UART_IT_TX) != SET);
            uart_rx.len --;
        }
    }

    if (tick_check_expire(uart_tx_tick, 1000)) {
        uart_tx_tick = tick_get();

        printf("uart tx test!");
    }
}

void udet_transfer_example(void)
{
    u8 *p = udet_rx.buf;
    static uint32_t udet_tx_tick = 0;

    if (((tick_get() - udet_rx.tick) >= udet_rx.timeout) && udet_rx.len) {
        while (udet_rx.len) {
            uart_send_data(UDET, *p++);
            while (uart_get_flag(UDET, UART_IT_TX) != SET);
            udet_rx.len --;
        }
    }

    if (tick_check_expire(udet_tx_tick, 1000)) {
        udet_tx_tick = tick_get();

        //printf("udet tx test!");
    }
}

#endif

