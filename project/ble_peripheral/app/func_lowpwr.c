#include "include.h"
#include "driver_lowpwr.h"
#include "driver_charge.h"
#include "driver_gpio.h"
#include "ble_user_service.h"
#include "bsp_io_key.h"

#if SYS_SLEEP_EN
typedef struct {
    u8  sysclk;
    u8  pg_de;
    u16 pa_de;
    u16 pb_de;
} sleep_backup_cb_t;

/*
    Users save the registers that need to be backed up.
    Registers that are not backed up will lose their values after they enter sleep
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

#if IO_KEY_SCAN_MODE
void bsp_gpio_scan_mode_de_save(void)
{
    uint32_t iokeyscan_gpioade = 0;
    uint32_t iokeyscan_gpiobde = 0;

    for (u8 i = 0; i < IO_KEY_ROW_TABLE_SIZE; i++) {
        if (GPIOA_REG == key_io_table_row[i].gpiox) {
            iokeyscan_gpioade |= key_io_table_row[i].gpio_pin;
        }
        if (GPIOB_REG == key_io_table_row[i].gpiox) {
            iokeyscan_gpiobde |= key_io_table_row[i].gpio_pin;
        }
    }
    //Retain the wakeup port configuration
    GPIOADE |= iokeyscan_gpioade & 0xffff;
    GPIOBDE |= iokeyscan_gpiobde & 0x3ff;
}
#endif

void bsp_gpio_wakeup_config(uint8_t lp_type)
{
    lowpwr_wakeup_typedef config;

    config.lp_type = lp_type;
    config.source = 0 ;
    config.edge = WK_EDGE_FALLING;

    #if IO_KEY_WK0_EN
    config.source |= WK_LP_WK0;
    lowpwr_wk0_wakeup_config(&config);
    #endif

    #if IO_KEY_SCAN_MODE
    config.source = WK_LP_GPIO ;
    config.edge = WK_EDGE_RISING;

    for (u8 i = 0; i < IO_KEY_COL_TABLE_SIZE; i++) {
        config.gpiox = key_io_table_column[i].gpiox;
        config.gpio_pin = key_io_table_column[i].gpio_pin;
        config.gpio_pupd = GPIO_PUPD_PD10K;
        lowpwr_gpio_wakeup_config(&config);
    }

    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;

    for (u8 i = 0; i < IO_KEY_ROW_TABLE_SIZE; i++) {
        gpio_init_structure.gpio_pin = key_io_table_row[i].gpio_pin;
        gpio_init(key_io_table_row[i].gpiox, &gpio_init_structure);
        gpio_set_bits(key_io_table_row[i].gpiox, gpio_init_structure.gpio_pin);
    }

    if (sys_cb.sleep_sta == LOWPWR_SLEEP_PREPARE) {     //io scan模式进sleep流程才开中断，避免平时扫描过程一直进中断
        gpio_edge_pic_config(gpio_isr_callback, 0);
    }
    #else
        config.source = WK_LP_GPIO ;
        config.edge = WK_EDGE_FALLING;

        for (u8 i = 0; i < IO_KEY_COL_TABLE_SIZE; i++) {
            config.gpiox = key_io_table_column[i].gpiox;
            config.gpio_pin = key_io_table_column[i].gpio_pin;
            config.gpio_pupd = GPIO_PUPD_PU10K;
            lowpwr_gpio_wakeup_config(&config);
        }

        gpio_edge_pic_config(gpio_isr_callback, 0);
    #endif // IO_KEY_SCAN_MODE
}

#endif

#if SYS_SLEEP_EN

AT(.text.app.proc.sleep)
static bool lowpwr_can_enter_sleep(void)
{
    bool can_sleep = true;

    do {
        if (sys_cb.sleep_delay != 0) {
            can_sleep = false;
            break;
        }

    #if FUNC_BT_EN
        if (!bt_is_sleep()) {
            can_sleep = false;
            break;
        }
    #endif

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
    } while(0);

    return can_sleep;
}

AT(.com_sleep.lowpwr.sleep)
static bool lowpwr_need_exit_sleep(void)
{
    if (lowpwr_is_wakeup_pending(WK_LP_BT)) {
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
    sys_cb.sleep_sta = LOWPWR_SLEEP_ENTER;
    lowpwr_sleep_proc(lpclk_type);                  //enter sleep
}

//休眠定时器，500ms进一次,需将cfg_bt_hsec_wakeup_en设置为1
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

static void lowpwr_sleep_wakeup_config_do(bool deep_sleep)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

    if (!deep_sleep) {
        config.source |= WK_LP_BT;
    }

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        config.source |=  WK_LP_INBOX;
    } else {
        config.source |=  WK_LP_VUSB;
    }
#endif

#if BSP_TOUCH_KEY_EN
    config.source |= WK_LP_TOUCH;
#endif

    config.lp_type = LP_TYPE_SLEEP;
    lowpwr_wakeup_config_2(&config);

#if BSP_IOKEY_EN
    bsp_gpio_wakeup_config(LP_TYPE_SLEEP);
#endif

}

void lowpwr_sleep_wakeup_config(void)
{
    lowpwr_sleep_wakeup_config_do(false);
}

void lowpwr_deep_sleep_wakeup_config(void)
{
    lowpwr_sleep_wakeup_config_do(true);
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep_prepare(sleep_backup_cb_t *sleep_backup_cb, bool deep_sleep)
{
    sys_cb.sleep_sta = LOWPWR_SLEEP_PREPARE;

    sys_set_tmr_enable(0, 0);

#if (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
    lowpwr_sleep_wakeup_config_do(deep_sleep);
#endif //(BSP_IOKEY_EN && IO_KEY_SCAN_MODE)

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE){
        lowpwr_wakeup_source_disable(WK_LP_VUSB);
        lowpwr_wakeup_source_enable(WK_LP_INBOX);
    } else {
        lowpwr_wakeup_source_disable(WK_LP_INBOX);
        lowpwr_wakeup_source_enable(WK_LP_VUSB);
    }
#endif

#if FUNC_BT_EN
    if (deep_sleep) {
        bt_off();
    } else {
        ble_enter_sleep_proc();
        delay_5ms(1);
        bt_enter_sleep();
    }
#endif

    //io analog input
    sleep_backup_cb->pa_de = GPIOADE;
    sleep_backup_cb->pb_de = GPIOBDE;
    sleep_backup_cb->pg_de = GPIOGDE;

    //Retain the wakeup port configuration
    GPIOADE &= PORTINTEN & 0xffff;
    GPIOBDE &= (PORTINTEN >> 16) & 0x3ff;
    //MCP FLASH
    GPIOGDE = 0x3F;

#if (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
    bsp_gpio_scan_mode_de_save();
#endif

    sleep_backup_cb->sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);
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
void sfunc_deep_sleep_do(void)
{
    if(sys_cb.sleep_prevent == false){
        sys_cb.sleep_sta = LOWPWR_SLEEP_ENTER;
        lowpwr_sleep_proc(CLK_RCDIV);
    }
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep_exit(sleep_backup_cb_t *sleep_backup_cb, bool deep_sleep)
{
#if (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
    gpio_edge_pic_disable();
#endif //(BSP_IOKEY_EN && IO_KEY_SCAN_MODE)

    sys_clk_set(sleep_backup_cb->sysclk);

    //Reinitialize all peripherals, because all register configurations are lost after low power
    if(sys_cb.sleep_sta == LOWPWR_SLEEP_ENTER){
        bsp_periph_init();
    }

    printf("wakeup\n");

    GPIOADE = sleep_backup_cb->pa_de;
    GPIOBDE = sleep_backup_cb->pb_de;
    GPIOGDE = sleep_backup_cb->pg_de;

    lowpwr_clr_wakeup_pending();

    sys_set_tmr_enable(1, 1);

#if FUNC_BT_EN
    if (deep_sleep) {
        func_bt_init();
    } else {
        bt_exit_sleep();
        ble_exit_sleep_proc();
    }
#endif

    sys_cb.sleep_sta = LOWPWR_SLEEP_EXIT;
    sys_cb.sleep_prevent = false;

    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    printf("%s\n", __func__);

    sleep_backup_cb_t sleep_backup_cb;

    sfunc_sleep_prepare(&sleep_backup_cb, false);

    sfunc_sleep_do();

    sfunc_sleep_exit(&sleep_backup_cb, false);
}

/*
    Enter deep sleep, close the ble module, exit sleep, restart the ble module
*/
AT(.text.lowpwr.deep_sleep)
void sfunc_deep_sleep(void)
{
    printf("%s\n", __func__);

    sleep_backup_cb_t sleep_backup_cb;

    sys_set_tmr_enable(0, 0);

    lowpwr_deep_sleep_wakeup_config();

    sfunc_sleep_prepare(&sleep_backup_cb, true);

    sfunc_deep_sleep_do();

    sfunc_sleep_exit(&sleep_backup_cb, true);

    lowpwr_sleep_wakeup_config();
}

AT(.text.app.proc.sleep)
bool sleep_process(void)
{
    if(lowpwr_can_enter_sleep()) {
        sfunc_sleep();
        lowpwr_sleep_delay_reset();
        lowpwr_pwroff_delay_reset();
        return true;
    }

    return false;
}
#endif

void lowpwr_pwroff_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

#if BSP_TOUCH_KEY_EN
    config.source |= WK_LP_TOUCH;
#endif

#if BSP_CHARGE_EN
    if (charge_detect_dc() == CHARGE_DC_STATUS_ONLINE) {
        config.source |=  WK_LP_INBOX;
    } else{
        config.source |=  WK_LP_VUSB;
    }
#endif

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

#if (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
    bsp_gpio_scan_mode_de_save();
#endif

#if SYS_PWROFF_MODE == PWROFF_MODE1
    lowpwr_pwroff_mode1_do();
#elif SYS_PWROFF_MODE == PWROFF_MODE2
    lowpwr_pwroff_mode2_do();
#endif
}
