/**
******************************************************************************************************************************
*
*@file ir_example.h
*
*@brief Header file for IR example
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _BSP_IR_H
#define _BSP_IR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    IRTX_MODE_SEND,
    IRTX_MODE_CAPTURE,
} bsp_irtx_mode_t;

#define IRRX_IO_PORT        GPIOA_REG
#define IRRX_IO_PIN         GPIO_PIN_3

#define IRTX_IO_PORT        GPIOA_REG
#define IRTX_IO_PIN         GPIO_PIN_4

#define IRCLK_IO_PORT       GPIOA_REG
#define IRCLK_IO_PIN        GPIO_PIN_5

//TX Data
#define IR_TX_DATA      0x89abcdef

void bsp_itx_send_data(uint32_t code);
void bsp_irtx_capture_handler_process(void);
void bsp_irtx_mode_init(bsp_irtx_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
