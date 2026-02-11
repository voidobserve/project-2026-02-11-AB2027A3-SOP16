#include "include.h"
#include "driver_lowpwr.h"
#include "driver_gpio.h"
#include "driver_charge.h"
#include "bsp_io_key.h"

/*
    Users save the registers that need to be backed up.
    Registers that are not backed up will lose their values after they enter deep sleep
*/
#define LOWPWR_SFR_USER_SAVE

#ifdef LOWPWR_SFR_USER_SAVE
typedef struct {
    u32 funcomcon[6];

    u32 uart0baud;
    u16 uart0con;
} lowpwr_sfr_user_backup_t;

lowpwr_sfr_user_backup_t sfr_user_backup;

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_save(void)
{
    sfr_user_backup.funcomcon[0] = FUNCOMCON0;
    sfr_user_backup.funcomcon[1] = FUNCOMCON1;
    sfr_user_backup.funcomcon[2] = FUNCOMCON4;
    sfr_user_backup.funcomcon[3] = FUNCOMCON5;
    sfr_user_backup.funcomcon[4] = FUNCOMCON6;
    sfr_user_backup.funcomcon[5] = FUNCIMCON0;

    sfr_user_backup.uart0baud = UART0BAUD;
    sfr_user_backup.uart0con = UART0CON;
}

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_resume(void)
{
    FUNCOMCON0 = sfr_user_backup.funcomcon[0];
    FUNCOMCON1 = sfr_user_backup.funcomcon[1];
    FUNCOMCON4 = sfr_user_backup.funcomcon[2];
    FUNCOMCON5 = sfr_user_backup.funcomcon[3];
    FUNCOMCON6 = sfr_user_backup.funcomcon[4];
    FUNCIMCON0 = sfr_user_backup.funcomcon[5];

    UART0BAUD = sfr_user_backup.uart0baud;
    UART0CON = sfr_user_backup.uart0con;
    UART0CPND = BIT(17) | BIT(16);                  //CLR Pending
}
#endif

AT(.com_text.sleep_tick)
void lowpwr_tout_ticks(void)
{
#if SYS_SLEEP_EN
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0) {
        sys_cb.sleep_delay--;
    }
#endif

#if SYS_OFF_TIME
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0) {
        sys_cb.pwroff_delay--;
    }
#endif
}

#if SYS_SLEEP_EN

AT(.com_sleep.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用com_sleep或com_text函数

    //此处关掉影响功耗的模块

    lowpwr_sleep_proc(lpclk_type);                  //enter sleep

    //唤醒后，恢复模块功能
}

//休眠定时器，500ms进一次
AT(.com_sleep.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;

#if SYS_OFF_TIME
    if(sys_cb.pwroff_delay != -1L) {
        if(sys_cb.pwroff_delay > 5) {
            sys_cb.pwroff_delay -= 5;
        } else {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }
#endif

    return ret;
}

void lowpwr_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = WK_LP_WK0 | WK_LP_BT;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        config.source |=  WK_LP_INBOX;
    } else{
        config.source |=  WK_LP_VUSB;
    }
#endif

#if BSP_TOUCH_KEY_EN
    config.source |= WK_LP_TOUCH;
    config.source &= ~WK_LP_WK0;
#endif

    config.edge = WK_EDGE_FALLING;
    config.lp_type = LP_TYPE_SLEEP;

    lowpwr_wakeup_config_2(&config);
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t status, ret, sysclk;
    u8 pg_de;
    u16 pa_de, pb_de;

    printf("%s\n", __func__);

    bt_enter_sleep();

    sys_set_tmr_enable(0, 0);
    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pg_de = GPIOGDE;
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    lowpwr_wakeup_disable();
    lowpwr_sleep_wakeup_config();

    while(bt_is_sleep()) {
        WDT_CLR();

        status = bt_sleep_proc();

        if(status == 1) {
            ret = sleep_timer();
            if(ret) {
                if (ret == 1) {
                    func_cb.sta = FUNC_PWROFF;
                }
                break;
            }
        }

        if (lowpwr_is_wakeup_pending(WK_LP_BT)) {
            break;
        }
    }

    sys_clk_set(sysclk);

    printf("wakeup\n");

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_wakeup_disable();

    sys_set_tmr_enable(1, 1);

    bt_exit_sleep();
    printf("sleep_exit\n");
}

AT(.text.lowpwr.deep_sleep_wk)
void lowpwr_deep_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = WK_LP_WK0;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        config.source |=  WK_LP_INBOX;
    } else{
        config.source |=  WK_LP_VUSB;
    }
#endif

#if BSP_TOUCH_KEY_EN
    config.source |= WK_LP_TOUCH;
    config.source &= ~WK_LP_WK0;
#endif

    config.lp_type = LP_TYPE_SLEEP;
    lowpwr_wakeup_config_2(&config);
}

/*
    Enter deep sleep, close the ble module, exit sleep, restart the ble module
*/
AT(.text.lowpwr.deep_sleep)
void sfunc_deep_sleep(void)
{
    uint32_t sysclk;
    u8 pg_de;
    u16 pa_de, pb_de;

    printf("%s\n", __func__);

    sys_cb.sleep_sta = LOWPWR_SLEEP_PREPARE;

    bt_off();
    sys_set_tmr_enable(0, 0);

     //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pg_de = GPIOGDE;
    //Retain the wakeup port configuration
    GPIOADE &= PORTINTEN & 0xffff;
    GPIOBDE &= (PORTINTEN >> 16) & 0x3ff;
    //MCP FLASH
    GPIOGDE = 0x3F;

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    if(sys_cb.sleep_prevent == false){
        sys_cb.sleep_sta = LOWPWR_SLEEP_ENTER;
        lowpwr_sleep_proc(CLK_RCDIV);
    }

    sys_clk_set(sysclk);

    //Reinitialize all peripherals, because all register configurations are lost after low power
    if(sys_cb.sleep_sta == LOWPWR_SLEEP_ENTER){
        bsp_periph_init();
    }

    printf("wakeup deep\n");

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_clr_wakeup_pending();
    sys_set_tmr_enable(1, 1);
    func_bt_init();

    sys_cb.sleep_sta = LOWPWR_SLEEP_EXIT;
    sys_cb.sleep_prevent = false;

    lowpwr_sleep_wakeup_config();

    printf("sleep_exit_deep\n");
}

AT(.text.app.proc.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (is_sleep()) {
        if (sys_cb.sleep_delay == -1L) {
            return false;
        }
        if(sys_cb.sleep_delay == 0) {
            sfunc_sleep();
            lowpwr_sleep_delay_reset();
            lowpwr_pwroff_delay_reset();
            return true;
        }
    } else {
        lowpwr_sleep_delay_reset();
    }
    return false;
}
#endif

void lowpwr_pwroff_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = WK_LP_WK0;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        config.source |=  WK_LP_INBOX;
    } else {
        config.source |=  WK_LP_VUSB;
    }
#endif

#if BSP_TOUCH_KEY_EN
    config.source |= WK_LP_TOUCH;
    config.source &= ~WK_LP_WK0;
#endif
    config.edge = WK_EDGE_FALLING;
    config.lp_type = LP_TYPE_POWOFF;

    lowpwr_wakeup_config_2(&config);
}


AT(.text.lowpwr.pwroff)
void func_pwroff(void)
{
    printf("%s\n", __func__);

    sys_set_tmr_enable(0, 0);

    GPIOADE = 0;
    GPIOBDE = 0;

    lowpwr_pwroff_wakeup_config();

#if SYS_PWROFF_MODE == PWROFF_MODE1
    lowpwr_pwroff_mode1_do();
#elif SYS_PWROFF_MODE == PWROFF_MODE2
    lowpwr_pwroff_mode2_do();
#endif
}
