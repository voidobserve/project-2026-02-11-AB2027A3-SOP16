#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if BSP_ADKEY_EN || BSP_IOKEY_EN || BSP_TOUCH_KEY_EN
    #define BSP_KEY_EN              1
#endif

#if BSP_ADKEY_EN || BSP_VBAT_DETECT_EN || BSP_TSEN_EN
    #define BSP_SARADC_EN           1
#endif

#if !SYS_SLEEP_EN
    #undef SYS_SLEEP_TIME
    #define SYS_SLEEP_TIME          0
#endif

#if (FLASH_RESERVE_SIZE % 4096) != 0
    #error "FLASH_RESERVE_SIZE is not a multiple of 4096!"
#endif

#if (AB_FOT_EN && (AB_FOT_TYPE == AB_FOT_TYPE_PACK))
    #if (FOT_PACK_START % 4096) != 0
        #error "FOT_PACK_START is not a multiple of 4096!"
    #endif
#endif


/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#if TONE_PLAY_THREAD_EN
    #undef OS_THREAD_AUPCM_STACK_EN
    #define OS_THREAD_AUPCM_STACK_EN    1
#endif

#define OS_THREAD_SIZE_CAL(stack, msg, en)  (en ? (stack + 108 + msg * 112) : 0)
#define OS_HEAP_SIZE    768 +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_BT_STACK, 1, OS_THREAD_BT_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_AUPCM_STACK, 1, OS_THREAD_AUPCM_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_DRIVER_STACK, 1, OS_THREAD_DRIVER_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_TIMER_STACK, 0, 1) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_MAIN_STACK, 0, 1)


/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/

#if (FUNC_LE_BQB_RF_EN || FUNC_LE_FCC_EN || LE_FCC_TEST_EN || LE_BQB_RF_EN)
    #define LE_DUT_UART_EN          1
#else
    #define LE_DUT_UART_EN          0
#endif

#if LE_FCC_TEST_EN || LE_BQB_RF_EN
    #undef SYS_SLEEP_TIME
    #define SYS_SLEEP_TIME          0

    #undef SYS_OFF_TIME
    #define SYS_OFF_TIME            0

    #undef SYS_PWRUP_TIME
    #define SYS_PWRUP_TIME          0

    #undef WK0_10S_RESET
    #define WK0_10S_RESET           0
#endif

#if LE_FCC_TEST_EN || FUNC_LE_FCC_EN
    #if (LE_FCC_TEST_HSUART_IO_SEL == BSP_UART_DEBUG_EN)
    #error "LE FCC TEST: LE_FCC_TEST_HSUART_IO_SEL conflicts with BSP_UART_DEBUG_EN!\n"
    #endif  //(LE_FCC_TEST_HSUART_IO_SEL == BSP_UART_DEBUG_EN)
#endif

#if (FUNC_LE_FCC_EN && LE_FCC_TEST_EN)
#error "LE FCC TEST: The FUNC_LE_FCC_EN and LE_FCC_TEST_EN cannot be enabled at the same time!\n"
#endif

#if (FUNC_LE_BQB_RF_EN && LE_BQB_RF_EN)
#error "LE BQB RF TEST: The FUNC_LE_BQB_RF_EN and LE_BQB_RF_EN cannot be enabled at the same time!\n"
#endif


/*****************************************************************************
 * Module    : UART打印功能配置
 *****************************************************************************/
#if !BSP_UART_DEBUG_EN
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

/*****************************************************************************
 * Module    : GUI功能配置
 *****************************************************************************/
#define UI_ADDR_BASE(n)                     (FLASH_UI_BASE + n)

#if (GUI_SELECT == GUI_TFT_80_ST7735)
    #define GUI_SCREEN_WIDTH                80
    #define GUI_SCREEN_HEIGHT               160
    #define GUI_SCREEN_OFS_X                24
    #define GUI_SCREEN_OFS_Y                0
#elif (GUI_SELECT == GUI_TFT_172_320)
    #define GUI_SCREEN_WIDTH                172
    #define GUI_SCREEN_HEIGHT               320
    #define GUI_SCREEN_OFS_X                34
    #define GUI_SCREEN_OFS_Y                0
#else
    #define GUI_SCREEN_WIDTH                0
    #define GUI_SCREEN_HEIGHT               0
    #define GUI_SCREEN_OFS_X                0
    #define GUI_SCREEN_OFS_Y                0
#endif

#define GUI_SCREEN_CENTER_X                 (GUI_SCREEN_WIDTH / 2)
#define GUI_SCREEN_CENTER_Y                 (GUI_SCREEN_HEIGHT / 2)

#if (CTP_SELECT == CTP_CST8X)
    #define TP_IIC_ADDR                     0x15
    #define TP_IIC_UPDATE_ADDR              0x6A
#else
    #define TP_IIC_ADDR                     0x00
#endif

/*****************************************************************************
 * Module    : 电子烟功能配置
 *****************************************************************************/
#if ((CHIP_PACKAGE_SELECT == CHIP_2020T_D) && BSP_ECIGS_EN)
#undef BSP_ECIGS_EN
#define BSP_ECIGS_EN                        ECIG_CONTROL_V2
#endif

#endif // __CONFIG_EXTRA_H__

