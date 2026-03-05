#include "user_uart.h"
#include "driver_gpio.h"
#include "driver_uart.h"

#define UART_RX_BUF_SIZE 128

// 增加环形缓冲区结构体
typedef struct
{
    u8 buffer[UART_RX_BUF_SIZE];
    u8 head;
    u8 tail;
    u8 count;
} uart_rx_buffer_t;

static volatile uart_rx_buffer_t uart_rx_buffer = {0};

// uart_transfer_cb_typedef uart_rx = {
//     .done = 0,
//     .len = 0,
//     .timeout = 50, // ms
// };

// 获取接收缓冲区中有效的数据个数，单位：字节Byte
u8 uart_rxbuffer_get_count(void)
{
    return uart_rx_buffer.count;
}

// 从接收缓冲区中取出一个字节数据
u8 uart_rxbuffer_get_byte(void)
{
    u8 rxbyte;

    if (0 == uart_rx_buffer.count)
    {
        // 缓冲区空
        return 0xFF;
    }

    // 先偏移索引，再取出数据
    uart_rx_buffer.tail = (uart_rx_buffer.tail + 1) % UART_RX_BUF_SIZE;
    rxbyte = uart_rx_buffer.buffer[uart_rx_buffer.tail];
    uart_rx_buffer.count--;
    return rxbyte;
}

AT(.com_text.isr) // 定义代码应该被放置在中断服务程序的代码段中，如果不加该修饰，在运行时，跳转到该段代码，会导致芯片复位
void uart_rxbuffer_put_byte(u8 byte)
{
// 目前的逻辑：缓冲区满，覆盖旧的数据
// 先偏移索引，再存放数据
#if 1
    uart_rx_buffer.head = (uart_rx_buffer.head + 1) % UART_RX_BUF_SIZE;
    uart_rx_buffer.buffer[uart_rx_buffer.head] = byte;
    uart_rx_buffer.count++;
    if (uart_rx_buffer.count > UART_RX_BUF_SIZE)
    {
        uart_rx_buffer.count = UART_RX_BUF_SIZE;
    }
#else
    uart_rx_buffer.head++;
    if (uart_rx_buffer.head >= UART_RX_BUF_SIZE)
    {
        uart_rx_buffer.head = UART_RX_BUF_SIZE;
    }
    uart_rx_buffer.buffer[uart_rx_buffer.head] = byte;
    uart_rx_buffer.count++;
    if (uart_rx_buffer.count > UART_RX_BUF_SIZE)
    {
        uart_rx_buffer.count = UART_RX_BUF_SIZE;
    }
#endif
}

AT(.com_text.isr)
void uart_isr(void)
{
    volatile u8 byte;
    if (uart_get_flag(UART, UART_IT_RX) != RESET)
    {
        byte = (u8)uart_receive_data(UART);
        uart_rxbuffer_put_byte(byte);
        uart_clear_flag(UART, UART_IT_RX); // 清空接收标志位
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

void user_uart_putchar(char ch)
{
    while (uart_get_flag(UART, UART_IT_TX) != SET)
        ; // 等待发送完成
    uart_send_data(UART, (u16)ch);
}

void user_uart_init(void)
{
    uart_transfer_init(USER_UART_BAUD);

#if USER_DEBUG_ENABLE
    // 只在调试时使用，最终需要去掉这个功能
    my_printf_init(user_uart_putchar); // 注册用户自定义的打印函数
    my_printf("user_uart_init\n");
#endif
}