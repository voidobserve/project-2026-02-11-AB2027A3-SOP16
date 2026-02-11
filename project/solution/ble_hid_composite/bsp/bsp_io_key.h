#ifndef _BSP_IO_KEY_H
#define _BSP_IO_KEY_H


#include "driver_gpio.h"

#define IOKEY_KEY1_PORT         GPIOA_REG           // Set to [NULL] will disable this KEY.
#define IOKEY_KEY1_PIN          GPIO_PIN_1
#define IOKEY_KEY2_PORT         GPIOA_REG           
#define IOKEY_KEY2_PIN          GPIO_PIN_2
#define IOKEY_KEY3_PORT         GPIOA_REG           
#define IOKEY_KEY3_PIN          GPIO_PIN_3
#define IOKEY_KEY4_PORT         GPIOA_REG           
#define IOKEY_KEY4_PIN          GPIO_PIN_4
#define IOKEY_KEY5_PORT         GPIOA_REG           
#define IOKEY_KEY5_PIN          GPIO_PIN_5
#define IOKEY_KEY6_PORT         GPIOA_REG           
#define IOKEY_KEY6_PIN          GPIO_PIN_6

#define IO_KEY_WK0_EN           0


void bsp_io_key_init(void);
u8 bsp_get_io_key_id(void);


#endif // _BSP_IO_KEY_H
