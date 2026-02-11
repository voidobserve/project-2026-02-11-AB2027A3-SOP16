#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__ 

#include "include.h"

/*****************************************************************************
 * Module    : RGBCW配置
 *****************************************************************************/
#define BSP_RGBCW_EN                    0                       //RGBCW
#define BSP_RGBCW_HALFPOWER_EN          1                       //半功率RGBCW
#define BSP_RGBCW_FULLPOWER_EN          0                       //全功率RGBCW

#if BSP_RGBCW_HALFPOWER_EN && BSP_RGBCW_FULLPOWER_EN
#error "can not set 1 same time"
#endif
/*****************************************************************************
 * Module    : WS2812库配置
 *****************************************************************************/
#define WS2812_LIB_EN                   1                       // WS2812库使能
#define WS2812_JUST_DIRECT_RGB          0                       // WS2812库只用于直驱RGB, 打开这个开关后不会发送幻彩数据, 只会使用这个库来生成RGB效果
#define WS2812_LED_NUM_MAX              255                     // 灯珠数量
#define WS2812_NEO_TYPE                 NEO_BRG                 // 幻彩数据RGB排列
#define WS2812_LEDC_PORT                GPIOA_REG               // 幻彩驱动IO PORT
#define WS2812_LEDC_PIN                 GPIO_PIN_6              // 幻彩驱动IO PIN
#define UWS_SPEED_MAX                   510                     // 速度范围, 非必要勿修改
#define UWS_SPEED_MIN                   10                      // 速度范围, 非必要勿修改
#define UWS_SPEED_DEFAULT               500                     // 默认速度, 不可超过范围
#define UWS_POWERUP_DEFAULT             1                       // 1:上电默认开灯, 0:上电默认关灯

#include "ws2812fx.h"
#include "ws2812_Adafruit_NeoPixel.h"
#include "ws2812fx_effect.h"
#include "ws2812_my_effect.h" 

#endif