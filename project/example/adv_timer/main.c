#include "include.h"
#include "adv_timer_octoggle.h"
#include "adv_timer_pwm.h"
#include "cenatct_sgl.h"
#include "ocative.h"
#include "ocinactive.h"
#include "oneplus.h"
#include "sixstep.h"
#include "bsp_uart_debug.h"


#define ADV_TIMER_FUNC_SEL      1           // 0: 翻转输出
                                            // 1: 7路PWM测试
                                            // 2: 中心对齐\自动PWM输出置位\互补输出
                                            // 3. 输出有效极性
                                            // 4. 输出无效极性
                                            // 5. 输出脉冲
                                            // 6. 输出六步PWM

int main(void)
{


    bsp_uart_debug_init();

    printf("hello main handle %d\n",ADV_TIMER_FUNC_SEL);
    #if ADV_TIMER_FUNC_SEL == 0
    adv_timer_octoggle_example();
    #elif ADV_TIMER_FUNC_SEL == 1
    adv_timer_sevenchannelpwm_example();
    #elif ADV_TIMER_FUNC_SEL == 2
    adv_timer_cenatcesgl_example();
    #elif ADV_TIMER_FUNC_SEL == 3
    adv_timer_ocative_example();
    #elif ADV_TIMER_FUNC_SEL == 4
    adv_timer_ocinactive_example();
    #elif ADV_TIMER_FUNC_SEL == 5
    adv_timer_oneplus_example();
    #elif ADV_TIMER_FUNC_SEL == 6
    adv_timer_sixstep_example();
    #endif // ADV_TIMER_FUNC_SEL


    return 0;
}
