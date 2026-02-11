#ifndef __USER_UART_FOT_H__
#define __USER_UART_FOT_H__

#include "include.h"
 

#define HSUART_FOT_DRIVER_RX_BUF_LEN          128
#define HSUART_FOT_DRIVER_TX_BUF_LEN          128

#define HSUART_FOT_DRIVER_BAUD                115200
#define HSUART_FOT_DRIVER_PORT_SEL            GPIOB_REG
#define HSUART_FOT_DRIVER_PIN_SEL             GPIO_PIN_9  //VUSB
#define HSUART_FOT_DRIVER_OV_CNT              0x20        //unit: time for transmitting 1 bit

void hsuart_fot_driver_init(void);
void hsuart_fot_driver_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len);
 

#endif
