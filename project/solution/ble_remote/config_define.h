/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/*****************************************************************************
 * Module    : VDDIO等级定义
 *****************************************************************************/
#define PMU_VDDIO_2V4                   0x0  //2.4V
#define PMU_VDDIO_2V5                   0x1
#define PMU_VDDIO_2V6                   0x2
#define PMU_VDDIO_2V7                   0x3
#define PMU_VDDIO_2V8                   0x4
#define PMU_VDDIO_2V9                   0x5
#define PMU_VDDIO_3V0                   0x6
#define PMU_VDDIO_3V1                   0x7
#define PMU_VDDIO_3V2                   0x8
#define PMU_VDDIO_3V3                   0x9
#define PMU_VDDIO_3V4                   0xA
#define PMU_VDDIO_3V5                   0xB
#define PMU_VDDIO_3V6                   0xC
#define PMU_VDDIO_3V7                   0xD
#define PMU_VDDIO_3V8                   0xE
#define PMU_VDDIO_3V9                   0xF

/*****************************************************************************
 * Module    : MIC相关定义
 *****************************************************************************/
#define MIC_BIAS_1V8                    0x0  //1.8V
#define MIC_BIAS_2V0                    0x1
#define MIC_BIAS_2V2                    0x2
#define MIC_BIAS_2V4                    0x3
#define MIC_BIAS_2V5                    0x4
#define MIC_BIAS_2V6                    0x5
#define MIC_BIAS_2V7                    0x6
#define MIC_BIAS_2V8                    0x7

#define MIC_BIAS_RES_0K                 0x0
#define MIC_BIAS_RES_1K                 0x1
#define MIC_BIAS_RES_2K                 0x2
#define MIC_BIAS_RES_4K                 0x4
#define MIC_BIAS_RES_8K                 0x8

#define MIC_ANA_GAIN_0                  0x0
#define MIC_ANA_GAIN_1                  0x1
#define MIC_ANA_GAIN_2                  0x2
#define MIC_ANA_GAIN_3                  0x3
#define MIC_ANA_GAIN_4                  0x4
#define MIC_ANA_GAIN_5                  0x5
#define MIC_ANA_GAIN_6                  0x6
#define MIC_ANA_GAIN_7                  0x7

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
#define FSIZE_1M                        0x100000
#define FSIZE_2M                        0x200000
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

/*****************************************************************************
 * Module    :  LE REMOTE 方案列表
 *****************************************************************************/
#define LE_REMOTE_DEFAULT               0    //MSBC
#define LE_REMOTE_ATVV                  1    //ATVV(0.4) ADPCM

#endif //CONFIG_DEFINE_H
