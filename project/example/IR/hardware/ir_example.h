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
#ifndef _IR_EXAMPLE_H
#define _IR_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define IRRX_IO_PORT        GPIOA_REG
#define IRRX_IO_PIN         GPIO_PIN_3

#define IRTX_IO_PORT        GPIOA_REG
#define IRTX_IO_PIN         GPIO_PIN_4

//#define IRCLK_IO_PORT       GPIOA_REG
//#define IRCLK_IO_PIN        GPIO_PIN_5

//TX Data
#define IR_TX_DATA      0x89abcdef

typedef enum
{
    D7C6_PREAMBLE,
    D7C6_DATA,
    D7C6_CUSTOM,
}d7c6_component_t;

// This value is consistent with timing_freq
#define IR_TIMING_FREQ          1000000

#define T_US                    (1000 * 1000 / IR_TIMING_FREQ)	//X us per clock tick
//#define T_WIDTH                 (6 * IR_TIMING_FREQ / 10 / 1000)  //600us
#define TICK_CNT				(600 / T_US)  //clock tick counter for 600us

void irrx_example_init(uint8_t irrx_example_mode);
void irtx_example_init(uint8_t irtx_example_mode);
void irrx_handler_example(void);
void irtx_learn_handler_example(void);
void irtx_extension_d7c6_handler_example(void);
void irtx_send_handler_example(void);

void ir_learn_timer_send_init(void);
void ir_learn_tx_handler_example(void);
#ifdef __cplusplus
}
#endif

#endif
