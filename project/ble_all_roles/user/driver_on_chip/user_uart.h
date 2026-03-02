#ifndef __USER_UART_H__
#define __USER_UART_H__

#include "include.h"

#define UART_TX_PORT                GPIOB_REG
#define UART_TX_PIN                 GPIO_PIN_4
#define UART_RX_PORT                GPIOB_REG
#define UART_RX_PIN                 GPIO_PIN_0 

// #define USER_UART_BAUD 9600
#define USER_UART_BAUD 115200
// #define USER_UART_BAUD 512000
// #define USER_UART_BAUD 1500000

typedef struct {
    u8 done;
    u16 len;
    u32 timeout;
    u32 tick;
    u8 buf[32];
} uart_transfer_cb_typedef;

extern uart_transfer_cb_typedef uart_rx;

void uart_transfer_init(u32 baud);
// void uart_transfer_example(void);

void user_uart_init(void);

u8 uart_rxbuffer_get_byte(void);

#endif