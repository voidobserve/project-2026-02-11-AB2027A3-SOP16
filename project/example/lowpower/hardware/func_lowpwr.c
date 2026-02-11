#include "include.h"
#include "driver_lowpwr.h"
#include "driver_gpio.h"
#include "func_lowpwr.h"
#if RTC_WK_EXAMPLE_EN
#include "driver_clk.h"
#include "driver_rtc.h"
#endif

extern void bsp_periph_init(void);
extern void user_set_sleep_stat(bool stat);

#if RTC_WK_EXAMPLE_EN
AT(.com_rodata.isr)
const char alarm_tip[] = "\n----------------->> alarm <<--------------------\n\n";

AT(.com_text.isr)
static void rtc_isr(void)
{
    if ((rtc_get_flag(RTC_IT_ALM) != RESET) || (rtc_get_overflow_flag(RTC_IT_ALM) != RESET)) {
        rtc_clear_flag(RTC_IT_ALM);
        printf(alarm_tip);
    }
}
#endif

#if SLEEP_SFR_SAVE
/*
    Users save the registers that need to be backed up.
    Registers that are not backed up will lose their values after they enter deep sleep
*/
typedef struct {
    u32 uart0baud;
    u16 uart0con;
} lowpwr_sfr_user_backup_t;

lowpwr_sfr_user_backup_t sfr_user_backup;

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_save(void)
{
	sfr_user_backup.uart0baud = UART0BAUD;
	sfr_user_backup.uart0con = UART0CON;
}

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_resume(void)
{
    UART0BAUD = sfr_user_backup.uart0baud;
    UART0CON = sfr_user_backup.uart0con;
    UART0CPND = BIT(17) | BIT(16);                  //CLR Pending
}
#endif

void lowpwr_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;
#if RTC_WK_EXAMPLE_EN
    config.source = WK_LP_GPIO | WK_LP_ALARM;
#else
    config.source = WK_LP_GPIO;
#endif
    config.edge = WK_EDGE_FALLING;
    config.lp_type = LP_TYPE_SLEEP;
    config.gpiox = GPIOA_REG;
    config.gpio_pin = GPIO_PIN_6;       //PA6 wake up sleep
    config.gpio_pupd = GPIO_PUPD_PU10K;
    lowpwr_wakeup_config(&config);

#if RTC_WK_EXAMPLE_EN
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_RC2M_RTC); //enable rtc clock
    u32 rc2m_clk = rc2m_clk_nhz_get(1); //get rc2m actual frequency
    rtc_prescale_set((rc2m_clk/64)-1); //Config predivision factor for 1Hz counter 
    rtc_pic_config(rtc_isr, 0, RTC_IT_ALM, ENABLE);  //interrupt service routine register
    rtc_set_alarm(rtc_get_counter()+4); //Set the alarm to wake up after 5s
#endif
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t sysclk;
    u8 pg_de;
    u16 pa_de, pb_de;

    printf("sleep enter\n");

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

#if SYS_SLEEP_LEVEL == SLEEP_LEVEL_LV1
    lowpwr_sleep_proc(CLK_31K25);    //!!go to sleep, the program stops on this line!!!
#else
    lowpwr_sleep_proc(CLK_RCDIV);    //!!go to sleep, the program stops on this line!!!
#endif

    //Reinitialize all peripherals, because all register configurations are lost after low power
    bsp_periph_init();

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_get_wakeup_source();
    lowpwr_wakeup_disable();

    sys_clk_set(sysclk);

    user_set_sleep_stat(0); //example, update sleep status

#if RTC_WK_EXAMPLE_EN
    //fixed the bug with no alarm pending
    if (rtc_get_overflow_flag(RTC_IT_ALM)) {
        rtc_isr();
    }
#endif //RTC_WK_EXAMPLE_EN

    printf("sleep exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (is_sleep()) {
        sfunc_sleep();
        return true;
    } else {
        return false;
    }
}

void lowpwr_pwroff_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;
#if RTC_WK_EXAMPLE_EN
    config.source = WK_LP_WK0 | WK_LP_ALARM;
#else
    config.source = WK_LP_WK0;
#endif
    config.edge = WK_EDGE_FALLING;
    config.lp_type = LP_TYPE_POWOFF;

#if RTC_WK_EXAMPLE_EN
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_RC2M_RTC); //enable rtc clock
    u32 rc2m_clk = rc2m_clk_nhz_get(1); //get rc2m actual frequency
    rtc_prescale_set((rc2m_clk/64)-1); //Config predivision factor for 1Hz counter 
    rtc_set_alarm(rtc_get_counter()+4); //Set the alarm to wake up after 5s
#endif

    lowpwr_wakeup_config(&config);
}

void func_pwroff(void)
{
    printf("%s\n", __func__);

    GPIOADE = 0;
    GPIOBDE = 0;

    lowpwr_pwroff_wakeup_config();

#if (SYS_PWROFF_MODE  == POWEROFF_MODE1)
    lowpwr_pwroff_mode1_do();
#else
    lowpwr_pwroff_mode2_do();
#endif
}
