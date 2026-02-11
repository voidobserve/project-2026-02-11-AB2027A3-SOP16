#include "include.h"
#include "bsp_uart_debug.h"
#include "func_lowpwr.h"
#include "bsp_io_key.h"
#include "driver_lowpwr.h"

static bool user_sleep_stat = 0;
static uint8_t pwron_cnt = 0;
static uint8_t pwroff_cnt = 0;

void bsp_periph_init(void)
{
    bsp_uart_debug_init();
    bsp_io_key_init();
}

void user_set_sleep_stat(bool stat)
{
    user_sleep_stat = stat;
}

bool user_get_sleep_stat(void)
{
    return user_sleep_stat;
}

void user_poweron_check(void)
{
    while (1) {
        if (bsp_get_io_key_id() == KEY_ID_PP) {
            delay_5ms(100);
            pwron_cnt++;
            printf("pwron_cnt:%d\n", pwron_cnt);
            if (pwron_cnt >= 4) {
                printf("power on!\n");
                break;
            }
        } else {
            pwron_cnt = 0;
            func_pwroff();
        }
    }
}

void user_poweroff_check(void)
{
    if ((bsp_get_io_key_id() == KEY_ID_PP) && (pwron_cnt == 0)) {
        delay_5ms(100);
        pwroff_cnt++;
        printf("pwroff_cnt:%d\n", pwroff_cnt);
    } else if (bsp_get_io_key_id() == KEY_ID_NO) {  //按键抬起
        pwron_cnt = 0;
        pwroff_cnt = 0;
    }

    if (pwroff_cnt == 6) {
        func_pwroff();
    }
}

void user_sleep_check(void)
{
    if (bsp_get_io_key_id() == KEY_ID_K1) {
        printf("KEY_ID_K1\n");
        delay_5ms(50);
        user_set_sleep_stat(1);
    }
}

int main(void)
{
    bsp_periph_init();

    printf("--->lowpower example\n");

    lowpwr_get_wakeup_source();

    printf("!!!!!!!!!!!!!!!!!\n");
    printf("--->WK0:long press to power on/power off\n");
    printf("--->PA5:short press to enter sleep\n");
    printf("--->PA6:short press to exit sleep\n");
    printf("!!!!!!!!!!!!!!!!!\n");

    user_poweron_check();   //长按开机

    pmu_init(BUCK_MODE_EN);  // power init
    xosc_init();

    while (1) {
        WDT_CLR();  //退出sleep会自动打开WDT，该例程需持续清狗

        user_poweroff_check();  //长按关机

        user_sleep_check();  //模拟进入sleep

        sleep_process(user_get_sleep_stat);  //sleep处理
    }

    return 0;
}
