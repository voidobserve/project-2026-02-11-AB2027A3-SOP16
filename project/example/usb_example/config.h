/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "config_define.h"


/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_256K                      //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              0x2000                          //程序保留区空间大小（CM_SIZE）
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序使用空间大小
#define FLASH_ERASE_4K                  1                               //是否支持4K擦除
#define FLASH_DUAL_READ                 1                               //是否支持2线模式
#define FLASH_QUAD_READ                 0                               //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                               //SPI FLASH提速

#define CM_SIZE                         0x2000                          //参数区大小，至少8k(需4K对齐)
#define CM_START                        (FLASH_SIZE - CM_SIZE)          //参数区默认放在flash最后面

/*****************************************************************************
 * Module    : 降噪功能配置
 *****************************************************************************/
#define AINS3_EN                        0           //AINS3降噪算法使能
#define AINS3_PART_LEN                  64          //只支持60和64个样点处理一次
#define AINS3_NT                        0           //0~20，降噪等级，越大降噪越强
#define AINS3_PRIOR_OPT_IDX             6           //0~20，先验信噪比门限，越小人声越多

#define AINS4_EN                        0           //AINS4降噪算法使能
#define AINS4_PART_LEN                  64          //只支持60和64个样点处理一次
#define AINS4_NR_SUPPRESS               10          //0~20，降噪等级，越大降噪越强
#define AINS4_PRIOR_OPT_IDX             3           //0~20，先验信噪比门限，越小人声越多

/*****************************************************************************
 * Module    : FOTA功能配置
 *****************************************************************************/
#define AB_FOT_EN                       1                       //FOTA升级使能
#define AB_FOT_TYPE                     AB_FOT_TYPE_PACK        //FOTA升级方式选择
#define AB_FOT_CHL_USB_EN               1                       //FOTA升级USB通道使能 (需开启USB_VENDOR_EN)

#if (FLASH_SIZE == FSIZE_256K)
#define FOT_PACK_START                  0x23000
#define FOT_PACK_SIZE                   0x19000
#elif (FLASH_SIZE == FSIZE_512K)
#define FOT_PACK_START                  0x4B000
#define FOT_PACK_SIZE                   0x32000
#endif

/*****************************************************************************
 * Module    : USB Device Config
 *****************************************************************************/
#define USB_CDC_EN                      0
#define USB_HID_EN                      1
#define USB_VENDOR_EN                   1
#define USB_SPEAKER_EN                  1
#define USB_MIC_EN                      1
#define USB_AUDIO_EN                    (USB_SPEAKER_EN || USB_MIC_EN)
#define USB_ASSN_EN                     (USB_HID_EN && USB_CDC_EN)
/*****************************************************************************
 * Module    : 版本号管理
 *****************************************************************************/
#define SW_VERSION		                "V0.0.1"                //只能使用数字0-9,ota需要转码


#endif // USER_CONFIG_H
