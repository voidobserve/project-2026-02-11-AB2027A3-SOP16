/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/*****************************************************************************
 * Module    : 系统定义
 *****************************************************************************/
#define XOSC_CLK_HZ                     32768

/*****************************************************************************
 * Module    : 芯片选型
 *****************************************************************************/
#define CHIP_2021T3                     0   //QFN32
#define CHIP_2020T_D                    1   //QFN32

/*****************************************************************************
 * Module    : 显示相关配置选择列表
 *****************************************************************************/
//显示驱动屏选择
#define DISPLAY_NO                      0                                       //无显示模块
#define DISPLAY_TFT                     0x800                                   //选用彩屏做为显示驱动

#define GUI_NO                          DISPLAY_NO                              //无主题，无显示
#define GUI_TFT_80_ST7735               (DISPLAY_TFT | 0x01)                    //彩屏 80 * 160
#define GUI_TFT_172_320                 (DISPLAY_TFT | 0x02)                    //彩屏 172 * 320

#define CTP_NO                          0
#define CTP_CST8X                       1

/*****************************************************************************
 * Module    : 烟控算法选择
 *****************************************************************************/
#define ECIG_CONTROL_NULL               0   //关闭烟控
#define ECIG_CONTROL_V1                 1   //用saradc（10bit）采集
#define ECIG_CONTROL_V2                 2   //2020T-D芯片专用，12bit adc采集

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
#define FSIZE_1M                        0x100000
#define FSIZE_2M                        0x200000
#define FSIZE_4M                        0x400000
#define FSIZE_8M                        0x800000
#define FSIZE_16M                       0x1000000
#define FSIZE_32M                       0x2000000
#define FSIZE_512K                      0x80000
#define FSIZE_256K                      0x40000
#define FSIZE_128K                      0x20000

/*****************************************************************************
 * Module    : FOTA升级方式选择列表
 *****************************************************************************/
#define AB_FOT_TYPE_PACK                1   //FOTA压缩升级（代码做压缩处理，升级完成需做解压才可正常运行）


/*****************************************************************************
 * Module    : 软关机模式定义
 *****************************************************************************/
#define PWROFF_NO_USED                  0
#define PWROFF_MODE1                    1
#define PWROFF_MODE2                    2

/*****************************************************************************
 * Module    : GPIO 定义
 *****************************************************************************/
/* IO INDEX */
#define GPIO_NULL                       0x00
#define GPIO_PA0                        0xA0
#define GPIO_PA1                        0xA1
#define GPIO_PA2                        0xA2
#define GPIO_PA3                        0xA3
#define GPIO_PA4                        0xA4
#define GPIO_PA5                        0xA5
#define GPIO_PA6                        0xA6
#define GPIO_PA7                        0xA7
#define GPIO_PA8                        0xA8
#define GPIO_PA9                        0xA9
#define GPIO_PA10                       0xAA
#define GPIO_PA11                       0xAB
#define GPIO_PA12                       0xAC
#define GPIO_PA13                       0xAD
#define GPIO_PA14                       0xAE
#define GPIO_PA15                       0xAF
#define GPIO_PB0                        0xB0
#define GPIO_PB1                        0xB1
#define GPIO_PB2                        0xB2
#define GPIO_PB3                        0xB3
#define GPIO_PB4                        0xB4
#define GPIO_PB5                        0xB5
#define GPIO_PB6                        0xB6
#define GPIO_PB7                        0xB7
#define GPIO_PB8                        0xB8
#define GPIO_PB9                        0xB9
/* IO SWAPPER */
#define GPIO_PORT_GET(x)                (((x & 0xF0) == 0xA0)? GPIOA_REG: GPIOB_REG)
#define GPIO_PIN_GET(x)                 (1 << (x & 0x0F))

#endif //CONFIG_DEFINE_H
