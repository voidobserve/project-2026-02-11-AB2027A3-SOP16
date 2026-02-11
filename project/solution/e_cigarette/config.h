/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_LE_FCC_EN                  0   //是否打开蓝牙FCC测试功能, 使用单线串口通信, PB3, 1500000
#define FUNC_LE_BQB_RF_EN               0   //是否打开蓝牙BQB测试功能, 使用串口通信, RX:PA0, TX:PA1 ,9600
#define FUNC_IODM_EN                    0   //是否打开小牛测控功能, 使用单线串口通信, VUSB, 115200
#define FUNC_TBOX_EN                    0   //是否打开测试盒产测功能，请阅读prod_test_tbox.h的测试盒产测说明
#define FUNC_IDLE_EN                    1   //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_120M                 //选择系统时钟
#define BUCK_MODE_EN                    0                       //是否BUCK MODE, 0:LDO MODE  1:BUCK MODE
#define WK0_10S_RESET                   1                       //WK pin 10秒复位功能
#define SYS_SLEEP_EN                    1                       //系统休眠使能
#define SYS_SLEEP_TIME                  0                       //自动休眠时间(秒), 0为不自动休眠
#define SYS_PWROFF_MODE                 PWROFF_MODE2            //软关机模式选择，模式1 VDDIO掉电，模式2 VDDIO不掉电
#define SYS_OFF_TIME                    0                       //自动关机时间(秒), 0为不自动关机
#define SYS_PWRUP_TIME                  2000                    //长按开机时间(毫秒), 0为立即开机
#define BSP_UART_DEBUG_EN               GPIO_PB3                //串口打印调试使能[Baud=1500000]
#define BSP_ADKEY_EN                    0                       //AD按键使能[Default: PB0]
#define BSP_IOKEY_EN                    1                       //IO按鍵使能[WK0:PP;]
#define BSP_TOUCH_KEY_EN                0                       //触摸按键功能
#define BSP_CHARGE_EN                   1                       //充电功能使能
#define BSP_VBAT_DETECT_EN              1                       //电量检测功能使能
#define LP_PWROFF_VAL			        2200				    //低电关机电量值（mv）
#define BSP_TSEN_EN                     0                       //内置温度传感器采集功能
#define DONGLE_AUTH_EN                  0                       //是否配置加密狗授权，需打开xcfg.xm中的config(BUF, "加密狗授权密钥", "程序运行需要判断的合法授权密钥", dongle_soft_key, 20)

#define CHIP_PACKAGE_SELECT             CHIP_2020T_D             //芯片封装选择
#define GUI_SELECT                      GUI_TFT_172_320       //GUI Display Select
#define CTP_SELECT                      CTP_NO                  //CTP Select

/*****************************************************************************
 * Module    : GUI相关配置
 *****************************************************************************/
#define COMPO_BUF_SIZE                  512                     //组件BUF大小(2个BUF)
#define GUI_LINES_CNT                   5                       //单次推屏行数
#define GUI_FONT_W_SPACE                0                       //字的间距
#define GUI_FONT_H_SPACE                4                       //字的行间距
#define HEAP_FUNC_SIZE                  256                     //FUNC HEAP SIZE

/*****************************************************************************
 * Module    : 电子烟配置
 *****************************************************************************/
#define BSP_ECIGS_EN                    ECIG_CONTROL_V1         //烟控控制选择
#define ECIG_RES_ADC_EN                 1                       //是否有ADC2检阻电路

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_4M                        //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              CM_SIZE                         //FLASH保留区域大小
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序可使用的空间大小
#define FLASH_UI_BASE                   0x70000                         //UI资源起始地址
#define FLASH_UI_SIZE                   0x80000                         //UI资源大小
#define FLASH_ERASE_4K                  1                               //是否支持4K擦除
#define FLASH_DUAL_READ                 1                               //是否支持2线模式
#define FLASH_QUAD_READ                 0                               //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                               //SPI FLASH提速
#define FLASH_PROTECT_EN                0                               //FLASH写保护使能

#define CM_SIZE                         0x2000                          //参数区大小，至少8k(需4K对齐)
#define CM_START                        (FLASH_SIZE - CM_SIZE)          //参数区默认放在flash最后面

/*****************************************************************************
 * Module    : FOTA功能配置
 *****************************************************************************/
#define AB_FOT_EN                       0                       //FOTA升级使能
#define AB_FOT_TYPE                     AB_FOT_TYPE_PACK        //FOTA升级方式选择
#define LE_FOTA_EN                      AB_FOT_EN               //BLE FOTA升级使能

#define RES_USERBIN_EN                  0
#define USERRES_BASE_ADDR               0x380000
#define USERRES_SPACE_SIZE              0x7C000
#define RES_BIN_ADDR(offset_addr)       (USERRES_BASE_ADDR + offset_addr)


#if (FLASH_SIZE == FSIZE_256K)
#define FOT_PACK_START                  0x23000
#define FOT_PACK_SIZE                   0x19000
#elif (FLASH_SIZE == FSIZE_512K)
#define FOT_PACK_START                  0x4B000
#define FOT_PACK_SIZE                   0x32000
#elif (FLASH_SIZE == FSIZE_2M)
#define FOT_PACK_START                  0xFA000
#define FOT_PACK_SIZE                   0xF6000
#elif (FLASH_SIZE == FSIZE_4M)
#define FOT_PACK_START                  0x1FF000
#define FOT_PACK_SIZE                   0x1F0000
#elif (FLASH_SIZE == FSIZE_8M)
#define FOT_PACK_START                  0x3E8000
#define FOT_PACK_SIZE                   0x3E0000
#endif

/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#define OS_THREAD_MAIN_STACK            1024                                //main线程堆栈大小
#define OS_THREAD_TIMER_STACK           768                                 //timer线程堆栈大小
#define OS_THREAD_BT_STACK_EN           1                                   //蓝牙线程使能
#define OS_THREAD_BT_STACK              1536                                //蓝牙线程堆栈大小
#define OS_THREAD_AUPCM_STACK_EN        0                                   //AUPCM线程使能(有mic功能需使能)
#define OS_THREAD_AUPCM_STACK           1024                                //AUPCM线程堆栈大小
#define OS_THREAD_DRIVER_STACK_EN       1                                   //driver线程使能(有USB功能需使能)
#define OS_THREAD_DRIVER_STACK          1024                                //driver线程堆栈大小

/*****************************************************************************
 * Module    : 产测测试项配置
 *****************************************************************************/
#define PROD_TEST_KEY_EN                0            //按键测试
#define PROD_TEST_MIC_EN                0            //MIC测试

/*****************************************************************************
 * Module    : BLE FCC功能配置（与 FUNC_LE_FCC_EN 共用）
 *****************************************************************************/
#define LE_FCC_TEST_EN                  0           //上电直接进FCC
#define LE_FCC_TEST_HSUART_BAUD         1500000     //波特率
#define LE_FCC_TEST_HSUART_IO_SEL       GPIO_PB3    //IO口（默认单线串口）

/*****************************************************************************
 * Module    : BLE BQB功能配置（与 FUNC_LE_BQB_RF_EN 共用）
 *****************************************************************************/
#define LE_BQB_RF_EN                    0           //上电直接进BQB RF TEST
#define LE_BQB_RF_HSUART_BAUD           9600        //波特率
#define LE_BQB_RF_HSUART_RX_IO_SEL      GPIO_PA0    //IO口（默认双线串口）
#define LE_BQB_RF_HSUART_TX_IO_SEL      GPIO_PA1

/*****************************************************************************
 * Module    : BLE SM功能配置
 *****************************************************************************/
#define LE_SM_EN                        1           //BLE SM使能

/*****************************************************************************
 * Module    : 版本号管理
 *****************************************************************************/
#define SW_VERSION        "V1.0.8"        //只能使用数字0-9,ota需要转码

#include "config_extra.h"

#endif
