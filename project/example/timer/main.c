#include "include.h"
#include "timer_led.h"
#include "timer_pwm.h"
#include "timer_capture.h"
#include "bsp_uart_debug.h"


#define TIMER_FUNC_SEL      2           // 0: 溢出查询/中断测试
                                        // 1: PWM测试
                                        // 2: 捕获测试

int main(void)
{
    WDT_DIS();

    bsp_uart_debug_init();

#if TIMER_FUNC_SEL == 0
    printf("--->timer overflow interrupt example\n");
    timer3_led_init();
    while (1) {
        timer3_led_example();
    }

#elif TIMER_FUNC_SEL == 1
    printf("--->timer pwm example\n");
    timer3_pwm_init();
    while (1) {
        timer3_pwm_example();
    }

#elif TIMER_FUNC_SEL == 2
    printf("--->timer capture example\n");
    timer3_capture_init(-1L);
    while (1) {
        timer3_capture_example();
    }

#endif // TIMER_FUNC_SEL

    return 0;
}
