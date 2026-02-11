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
#define SYS_CLK_SEL                     SYS_24M                 //选择系统时钟
#define BUCK_MODE_EN                    0                       //是否BUCK MODE, 0:LDO MODE  1:BUCK MODE
#define WK0_10S_RESET                   1                       //WK pin 10秒复位功能
#define SYS_SLEEP_EN                    1                       //系统休眠使能
#define SYS_SLEEP_TIME                  5                       //自动浅休眠时间(秒), 0为不自动休眠
#define SYS_DEEP_SLEEP_TIME             50                      //自动深休眠时间(秒), 0为不自动休眠
#define SYS_PWROFF_MODE                 PWROFF_MODE2            //软关机模式选择，模式1 VDDIO掉电，模式2 VDDIO不掉电
#define SYS_OFF_TIME                    0                       //自动关机时间(秒), 0为不自动关机
#define SYS_PWRUP_TIME                  0                       //长按开机时间(毫秒), 0为立即开机
#define BSP_UART_DEBUG_EN               GPIO_PB9                //串口打印调试使能[Baud=1500000]
#define BSP_ADKEY_EN                    0                       //AD按键使能
#define BSP_IOKEY_EN                    0                       //IO按鍵使能[WK0:PP;]
#define BSP_KEYSCAN_EN                  1                       //keyscan按鍵使能
#define BSP_CHARGE_EN                   0                       //充电功能使能
#define BSP_IR_EN                       0                       //红外功能

/*****************************************************************************
 * Module    : 电池电量检测
 *****************************************************************************/
#define BSP_VBAT_DETECT_EN              1                       //电池电量检测
#define LP_PWROFF_EN			        1						//低电关机使能
#define LP_PWROFF_VAL			        2200				    //低电关机电量值（mv）

/*****************************************************************************
 * Module    : VDDIO配置
 *****************************************************************************/
#define PMU_VDDIO_LEVEL                 PMU_VDDIO_2V4           //VDDIO电压，电压越低语音底噪越小

/*****************************************************************************
 * Module    : MIC配置
 *****************************************************************************/
#define MIC_BIAS_EN                     1                       //MIC BIAS使能，PB1输出
#define MIC_BIAS_LEVEL                  MIC_BIAS_1V8            //MIC BIAS电压，电压越低语音底噪越小
#define MIC_BIAS_RES                    MIC_BIAS_RES_0K         //MIC BIAS电阻
#define MIC_ANA_GAIN                    MIC_ANA_GAIN_5          //MIC 模拟增益，6dB一挡

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_256K                      //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              CM_SIZE                         //FLASH保留区域大小
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序可使用的空间大小
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
#define AB_FOT_EN                       1                       //FOTA升级使能
#define AB_FOT_TYPE                     AB_FOT_TYPE_PACK        //FOTA升级方式选择
#define LE_FOTA_EN                      AB_FOT_EN               //BLE FOTA升级使能

#if (FLASH_SIZE == FSIZE_256K)
#define FOT_PACK_START                  0x23000
#define FOT_PACK_SIZE                   0x19000
#elif (FLASH_SIZE == FSIZE_512K)
#define FOT_PACK_START                  0x4B000
#define FOT_PACK_SIZE                   0x32000
#endif

/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#define OS_THREAD_MAIN_STACK            1024                                //main线程堆栈大小
#define OS_THREAD_TIMER_STACK           768                                 //timer线程堆栈大小
#define OS_THREAD_BT_STACK_EN           1                                   //蓝牙线程使能
#define OS_THREAD_BT_STACK              1536                                //蓝牙线程堆栈大小
#define OS_THREAD_AUPCM_STACK_EN        1                                   //AUPCM线程使能(有mic功能需使能)
#define OS_THREAD_AUPCM_STACK           1280                                //AUPCM线程堆栈大小
#define OS_THREAD_DRIVER_STACK_EN       0                                   //driver线程使能(有USB功能需使能)
#define OS_THREAD_DRIVER_STACK          1024                                //driver线程堆栈大小

/*****************************************************************************
 * Module    : 产测测试项配置
 *****************************************************************************/
#define PROD_TEST_KEY_EN                0            //按键测试
#define PROD_TEST_MIC_EN                0            //MIC测试

/*****************************************************************************
 * Module    : 语音降噪功能配置
 *****************************************************************************/
#define AINS3_EN                        0           //AINS3降噪算法使能 ram:5K, flash:8K, clock:120M以上
#define AINS3_PART_LEN                  60          // !!!!!! 只支持 60 和 64 个样点处理一次, msbc 60, adpcm 64
#define AINS3_NT                        0           //0~20，降噪等级，越大降噪越强
#define AINS3_PRIOR_OPT_IDX             6           //0~20，先验信噪比门限，越小人声越多

#define AINS4_EN                        0           //AINS4降噪算法使能 ram:11K， flash:20K, clock:120M以上
#define AINS4_PART_LEN                  60          // !!!!!! 只支持 60 和 64 个样点处理一次, msbc 60, adpcm 64
#define AINS4_NR_SUPPRESS               10          //0~20，降噪等级，越大降噪越强
#define AINS4_PRIOR_OPT_IDX             3           //0~20，先验信噪比门限，越小人声越多

/*****************************************************************************
 * Module    : BLE 功能配置
 *****************************************************************************/
#define LE_SM_EN                        1           //SM使能
#define LE_RPA_EN                       0           //RPA使能，libbtstack.a需改为libbtstack_privacy.a
#define LE_WLIST_EN                     0           //白名单使能，libbtstack.a需改为libbtstack_privacy.a
#define LE_DIR_ADV_EN                   0           //定向广播使能
#define LE_MTU_EXCHANGE_EN              1           //连接后主动发起MTU使能
#define LE_SM_SEND_SECURITY_REQ_TIME    500         //从机发起加密的延时，unit: ms
#define LE_SM_ALWAYS_ACCEPT_LTK_REQ_EN  0           //SM总是接受LTK请求使能
#define LE_DISCONN_RESTART_ADV_EN       1           //断连自动开广播使能
#define LE_RX_EXIT_SLEEP_EN             0           //RX到数据自动退出睡眠使能
#define LE_PHY_2M_EN                    1           //支持2M包使能
#define LE_PHY_UPDATE_EN                0           //连接后主动发起PHY使能
#define LE_FEATURE_EXCHANGE_EN          0           //连接后主动发起FEATURE使能
#define LE_LENGTH_REQ_EN                0           //连接后主动发起length request使能

#define LE_FCC_TEST_EN                  0           //上电直接进FCC，与 FUNC_LE_FCC_EN 共用串口配置
#define LE_FCC_TEST_HSUART_BAUD         1500000     //波特率
#define LE_FCC_TEST_HSUART_IO_SEL       GPIO_PB3    //IO口（默认单线串口）

#define LE_BQB_RF_EN                    0           //上电直接进BQB RF TEST， 与 FUNC_LE_BQB_RF_EN 共用串口配置
#define LE_BQB_RF_HSUART_BAUD           9600        //波特率
#define LE_BQB_RF_HSUART_RX_IO_SEL      GPIO_PA0    //IO口（默认双线串口）
#define LE_BQB_RF_HSUART_TX_IO_SEL      GPIO_PA1

/*****************************************************************************
 * Module    : 遥控器方案相关配置
 *****************************************************************************/
#define LE_REOMTE_PRODUCT_ID            LE_REMOTE_DEFAULT   //MSBC、ADPCM方案
#define LE_REMOTE_LATENCY_EN_DLY        5000                //连接后延时一段时间再开latency，unit:ms
#define LE_REMOTE_HOLD_KEY_LOWPOWER_EN  0                   //连接后长按低功耗使能，降低长按按键的平均功耗

/*****************************************************************************
 * Module    : 版本号管理
 *****************************************************************************/
#define SW_VERSION		"V1.0.8"		//只能使用数字0-9,ota需要转码

#include "config_extra.h"

#endif
