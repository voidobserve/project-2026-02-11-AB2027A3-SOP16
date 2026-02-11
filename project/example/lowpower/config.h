/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define UART_DEBUG_EN                   1                               //串口打印调试使能，默认PB3
#define BUCK_MODE_EN                    1                               //BUCK MODE使能, 0:LDO MODE  1:BUCK MODE
#define SYS_PWROFF_MODE                 POWEROFF_MODE2                  //关机模式配置，MODE1:只支持wk0唤醒  MODE2：支持更多唤醒源
#define SYS_SLEEP_LEVEL                 SLEEP_LEVEL_LV2                 //休眠等级配置，LV1：sleep跑晶振  LV2：sleep跑RC

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_128K                      //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              0x2000                          //程序保留区空间大小（CM_SIZE）
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序使用空间大小
#define FLASH_ERASE_4K                  1                               //是否支持4K擦除
#define FLASH_DUAL_READ                 1                               //是否支持2线模式
#define FLASH_QUAD_READ                 0                               //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                               //SPI FLASH提速。

#include "config_extra.h"

#endif
