#include "include.h"
#include "driver_lowpwr.h"
#include "driver_charge.h"
#include "driver_gpio.h"
#include "bsp_io_key.h"
#include "ble_user_service.h"

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
#if BSP_UART_DEBUG_EN
    sfr_user_backup.funcomcon[0] = FUNCOMCON0;
	sfr_user_backup.funcomcon[1] = FUNCOMCON1;
	sfr_user_backup.funcomcon[2] = FUNCOMCON4;
	sfr_user_backup.funcomcon[3] = FUNCOMCON5;
	sfr_user_backup.funcomcon[4] = FUNCOMCON6;
	sfr_user_backup.funcomcon[5] = FUNCIMCON0;

	sfr_user_backup.uart0baud = UART0BAUD;
	sfr_user_backup.uart0con = UART0CON;
#endif
}

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_resume(void)
{
#if BSP_UART_DEBUG_EN
    FUNCOMCON0 = sfr_user_backup.funcomcon[0];
    FUNCOMCON1 = sfr_user_backup.funcomcon[1];
	FUNCOMCON4 = sfr_user_backup.funcomcon[2];
	FUNCOMCON5 = sfr_user_backup.funcomcon[3];
	FUNCOMCON6 = sfr_user_backup.funcomcon[4];
    FUNCIMCON0 = sfr_user_backup.funcomcon[5];

    UART0BAUD = sfr_user_backup.uart0baud;
    UART0CON = sfr_user_backup.uart0con;
    UART0CPND = BIT(17) | BIT(16);                  //CLR Pending
#endif
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

#if BSP_IOKEY_EN
AT(.com_text.gpio.isr)
void gpio_isr_callback(void)
{
    WKUPCPND = 0xff << 16;                      //clear wakeup pending

#if SYS_SLEEP_EN
    if(sys_cb.sleep_sta != LOWPWR_SLEEP_EXIT){
        sys_cb.sleep_prevent = true;
    }
#endif

    lowpwr_sleep_delay_reset();
}

void bsp_gpio_wakeup_config(uint8_t lp_type)
{
    lowpwr_wakeup_typedef config;

    config.source = WK_LP_GPIO ;
    config.edge = WK_EDGE_FALLING;
    config.lp_type = lp_type;
    config.gpiox = IOKEY_KEY1_PORT;
    config.gpio_pupd = GPIO_PUPD_PU10K;
    /* An exception may be raised if the tow PORT are different */
    config.gpio_pin = IOKEY_KEY1_PIN | IOKEY_KEY2_PIN | IOKEY_KEY3_PIN;
    lowpwr_gpio_wakeup_config(&config);

    #if IO_KEY_WK0_EN
    config.source |= WK_LP_WK0;
    lowpwr_wk0_wakeup_config(&config);
    #endif

	gpio_edge_pic_config(gpio_isr_callback, 0);
}

#endif

#if SYS_SLEEP_EN

AT(.text.app.proc.sleep)
static bool lowpwr_can_enter_sleep(void)
{
    bool can_sleep = true;

    u16 msg = msg_dequeue();

    if(msg){
        switch(msg){
            case MSG_SYS_1S:
            case MSG_SYS_500MS:
                break;

            default:
                msg_enqueue(msg);
                can_sleep = false;
                break;
        }
    }

    return can_sleep;
}

static bool lowpwr_need_exit_sleep(void)
{
    if (lowpwr_is_wakeup_pending(WK_LP_BT)) {
        return true;
    }

    /*防止蓝牙与RTC同时到来，且蓝牙唤醒稍早于RTC，唤醒源只有BT，导致无法退出休眠while*/
    uint16_t pending = lowpwr_get_wakeup_source() >> 16;
    if ((pending & WK_LP_ALARM) || (pending & WK_LP_RTC_1S)) {
        return true;
    }

    if (ble_proc_pending()) {
        return true;
    }

    return false;
}

AT(.com_sleep.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用com_sleep或com_text函数

    //此处关掉影响功耗的模块

    sys_cb.sleep_sta = LOWPWR_SLEEP_ENTER;
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
            func_cb.sta = FUNC_PWROFF;
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

    config.lp_type = LP_TYPE_SLEEP;
    config.source = WK_LP_BT;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE) {
        config.source |=  WK_LP_INBOX;
    } else {
        config.source |=  WK_LP_VUSB;
    }
#endif

    lowpwr_wakeup_config_2(&config);

#if BSP_IOKEY_EN
    bsp_gpio_wakeup_config(LP_TYPE_SLEEP);
#endif
}

AT(.com_sleep.lowpwr.sleep)
void sfunc_sleep_do(void)
{
    uint32_t status, ret;

    while(bt_is_sleep() && !sys_cb.sleep_prevent) {
        WDT_CLR();

        if(lowpwr_need_exit_sleep()){
            break;
        }

        status = bt_sleep_proc();

        if(status == 1) {
            ret = sleep_timer();
            if(ret) {
                break;
            }
        }
    }
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    u8 sysclk;
    u8 pg_de;
    u16 pa_de, pb_de;

    printf("%s\n", __func__);

    sys_cb.sleep_sta = LOWPWR_SLEEP_PREPARE;

    ble_enter_sleep_proc();
    delay_5ms(1);
    bt_enter_sleep();

    sys_set_tmr_enable(0, 0);

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        lowpwr_wakeup_source_disable(WK_LP_VUSB);
        lowpwr_wakeup_source_enable(WK_LP_INBOX);
    } else {
        lowpwr_wakeup_source_disable(WK_LP_INBOX);
        lowpwr_wakeup_source_enable(WK_LP_VUSB);
    }
#endif

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

    sfunc_sleep_do();

    sys_clk_set(sysclk);

    //Reinitialize all peripherals, because all register configurations are lost after low power
    if(sys_cb.sleep_sta == LOWPWR_SLEEP_ENTER){
        bsp_periph_init();
    }

    printf("wakeup\n");

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_clr_wakeup_pending();

    sys_set_tmr_enable(1, 1);

    bt_exit_sleep();
    ble_exit_sleep_proc();

    sys_cb.sleep_sta = LOWPWR_SLEEP_EXIT;
    sys_cb.sleep_prevent = false;

    printf("sleep_exit\n");
}

AT(.text.lowpwr.deep_sleep_wk)
void lowpwr_deep_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.lp_type = LP_TYPE_SLEEP;
    config.source = 0;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE) {
        config.source |=  WK_LP_INBOX;
    } else {
        config.source |=  WK_LP_VUSB;
    }
#endif

    lowpwr_wakeup_config_2(&config);

#if BSP_IOKEY_EN
    bsp_gpio_wakeup_config(LP_TYPE_SLEEP);
#endif
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

    lowpwr_deep_sleep_wakeup_config();

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
    if (sys_cb.sleep_delay == -1L) {
        return false;
    }

    if (is_sleep()) {
        if((sys_cb.sleep_delay == 0) && lowpwr_can_enter_sleep()) {
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

    config.source = 0;

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE) {
        config.source |=  WK_LP_INBOX;
    } else {
        config.source |=  WK_LP_VUSB;
    }
#endif

    config.edge = WK_EDGE_FALLING;
    config.lp_type = LP_TYPE_POWOFF;

    lowpwr_wakeup_config_2(&config);

#if BSP_IOKEY_EN
    bsp_gpio_wakeup_config(LP_TYPE_POWOFF);
#endif
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
