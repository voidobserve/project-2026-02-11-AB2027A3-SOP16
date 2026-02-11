#ifndef _BSP_UART_DEBUG_H
#define _BSP_UART_DEBUG_H

#include "include.h"

#define UART_DEBUG_PORT_SEL             GPIOB_REG
#define UART_DEBUG_PIN_SEL              GPIO_PIN_9

void bsp_uart_debug_init(void);

#endif // _BSP_UART_DEBUG_H
