#ifndef _UART_TRANSFER_H
#define _UART_TRANSFER_H

#define UART_EXAMPLE                1
#define UDET_EXAMPLE                2
#define UART_UDET_EXAMPLE           3

#define UART_UDET_EXAMPLE_SEL       UART_EXAMPLE

#define UART_TX_PORT                GPIOA_REG
#define UART_TX_PIN                 GPIO_PIN_0
#define UART_RX_PORT                GPIOA_REG
#define UART_RX_PIN                 GPIO_PIN_1

#define UDET_TX_PORT                GPIOA_REG
#define UDET_TX_PIN                 GPIO_PIN_2
#define UDET_RX_PORT                GPIOA_REG
#define UDET_RX_PIN                 GPIO_PIN_3

typedef struct {
    u8 done;
    u16 len;
    u32 timeout;
    u32 tick;
    u8 buf[32];
} uart_transfer_cb_typedef;


void uart_transfer_init(u32 baud);
void uart_transfer_example(void);
void udet_transfer_example(void);
void udet_transfer_init(u32 baud);

#endif // _UART_TRANSFER_H
