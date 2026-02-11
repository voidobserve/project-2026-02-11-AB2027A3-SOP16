#ifndef _PORT_TFT_H
#define _PORT_TFT_H

#include "driver_gpio.h"

#if (CHIP_PACKAGE_SELECT == CHIP_2021T3)
#define TFT_SPI_CS_PORT         GPIOA_REG       //CS
#define TFT_SPI_CS_PIN          GPIO_PIN_6

#define TFT_SPI_CLK_PORT        GPIOA_REG       //CLK
#define TFT_SPI_CLK_PIN         GPIO_PIN_3

#define TFT_SPI_DATA_PORT       GPIOA_REG       //D0
#define TFT_SPI_DATA_PIN        GPIO_PIN_2

#define TFT_SPI_RS_PORT         GPIOA_REG       //DC
#define TFT_SPI_RS_PIN          GPIO_PIN_4

#define TFT_SPI_BL_PORT         GPIOA_REG       //BL
#define TFT_SPI_BL_PIN          GPIO_PIN_7

#define TFT_SPI_EN_PORT         GPIOA_REG       //LCD EN
#define TFT_SPI_EN_PIN          GPIO_PIN_8

#define TFT_SPI_RST_PORT        GPIOA_REG       //RESET
#define TFT_SPI_RST_PIN         GPIO_PIN_5

#elif (CHIP_PACKAGE_SELECT == CHIP_2020T_D)

#define TFT_SPI_CS_PORT         GPIOA_REG       //CS
#define TFT_SPI_CS_PIN          GPIO_PIN_10

#define TFT_SPI_CLK_PORT        GPIOA_REG       //CLK
#define TFT_SPI_CLK_PIN         GPIO_PIN_7

#define TFT_SPI_DATA_PORT       GPIOA_REG       //D0
#define TFT_SPI_DATA_PIN        GPIO_PIN_6

#define TFT_SPI_RS_PORT         GPIOA_REG       //DC
#define TFT_SPI_RS_PIN          GPIO_PIN_8

#define TFT_SPI_BL_PORT         GPIOA_REG       //BL
#define TFT_SPI_BL_PIN          GPIO_PIN_11

#define TFT_SPI_EN_PORT         GPIOA_REG       //LCD EN
#define TFT_SPI_EN_PIN          GPIO_PIN_12

#define TFT_SPI_RST_PORT        GPIOA_REG       //RESET
#define TFT_SPI_RST_PIN         GPIO_PIN_9

#endif

#define TFT_SPI_CS_EN()         gpio_reset_bits(TFT_SPI_CS_PORT, TFT_SPI_CS_PIN);
#define TFT_SPI_CS_DIS()        gpio_set_bits(TFT_SPI_CS_PORT, TFT_SPI_CS_PIN);

#define TFT_RS_CMD()            gpio_reset_bits(TFT_SPI_RS_PORT, TFT_SPI_RS_PIN);
#define TFT_RS_DATA()           gpio_set_bits(TFT_SPI_RS_PORT, TFT_SPI_RS_PIN);

#define LCD_BL_EN()             gpio_set_bits(TFT_SPI_BL_PORT, TFT_SPI_BL_PIN);
#define LCD_BL_DIS()            gpio_reset_bits(TFT_SPI_BL_PORT, TFT_SPI_BL_PIN);

void port_tft_init(void);
void port_tft_exit(void);
void port_tft_reset(void);

#endif
