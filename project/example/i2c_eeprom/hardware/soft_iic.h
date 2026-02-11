#ifndef _BSP_IICSW_H_
#define _BSP_IICSW_H_

#include "driver_gpio.h"
#include "typedef.h"
#include "sfr.h"


#define IIC_DELAY_US          1
#define bsp_iicsw_delay_us(us) delay_us(us)

#define SOFT_I2C_SCL_GPIO       GPIOA_REG
#define SOFT_I2C_SCL_PIN        GPIO_PIN_1
#define SOFT_I2C_SDA_PORT       GPIOA_REG
#define SOFT_I2C_SDA_PIN        GPIO_PIN_2


#define IIC_SDA_PORT SOFT_I2C_SCL_GPIO->data
#define IIC_SDA_SET  SOFT_I2C_SCL_GPIO->io_set
#define IIC_SDA_CLR  SOFT_I2C_SCL_GPIO->io_clr
#define IIC_SDA_PU   SOFT_I2C_SCL_GPIO->pupd[0]
#define IIC_SDA_PD   SOFT_I2C_SCL_GPIO->pupd[1]
#define IIC_SDA_DIR  SOFT_I2C_SCL_GPIO->dir
#define IIC_SDA_FEN  SOFT_I2C_SCL_GPIO->fen
#define IIC_SDA_PIN  SOFT_I2C_SDA_PIN


#define IIC_SCL_PORT SOFT_I2C_SDA_PORT->data
#define IIC_SCL_SET  SOFT_I2C_SDA_PORT->io_set
#define IIC_SCL_CLR  SOFT_I2C_SDA_PORT->io_clr
#define IIC_SCL_PU   SOFT_I2C_SDA_PORT->pupd[0]
#define IIC_SCL_PD   SOFT_I2C_SDA_PORT->pupd[1]
#define IIC_SCL_DIR  SOFT_I2C_SDA_PORT->dir
#define IIC_SCL_FEN  SOFT_I2C_SDA_PORT->fen
#define IIC_SCL_PIN  SOFT_I2C_SCL_PIN



#define IIC_SDA_LVL (IIC_SDA_PORT & IIC_SDA_PIN)
#define IIC_SCL_LVL (IIC_SCL_PORT & IIC_SCL_PIN)

#define IIC_NACK 1
#define IIC_ACK  0


void bsp_iicsw_init (void);
void bsp_iicsw_read_bytes (uint8_t devaddr, uint8_t addr, uint8_t *dat, uint32_t len);
void bsp_iicsw_write_bytes (uint8_t devaddr, uint8_t addr, uint8_t *dat, uint32_t len);
void bsp_iicsw_master_example(void);



#endif /* _BSP_IICSW_H_ */
