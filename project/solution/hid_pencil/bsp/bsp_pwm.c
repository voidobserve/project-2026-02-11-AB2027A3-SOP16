#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "driver_adv_timer.h"
#include "bsp_pwm.h"

#if BSP_PWM_EN

void bsp_tmr_pwm_init(void)
{
    gpio_init_typedef gpio_init_structure;
    tmr_base_init_typedef tmr_base_init_struct;
    tim_timebaseinittypedef tim_base_init_struct;
    tim_ocinittypedef tim_oc_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);
    clk_gate0_cmd(CLK_GATE0_TMR0, CLK_EN);

    gpio_init_structure.gpio_pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_6, GPIO_CROSSBAR_OUT_T3PWM0);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_7, GPIO_CROSSBAR_OUT_TOC1);

    /* 484.8KHz 初始化 */

    tmr_base_init_struct.clock_source = TMR_SYSTEM_CLOCK;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;
    tmr_base_init_struct.prescale = 0;
    tmr_base_init_struct.period = 33 - 1;
    tmr_base_init(TMR3, &tmr_base_init_struct);

    tmr_pwm_duty_config(TMR3, TMR_PWM0, 17 - 1);
    tmr_pwm_cmd(TMR3, TMR_PWM0, ENABLE);

    /* 470.6KHz 初始化 */

    tim_timebasestructinit(&tim_base_init_struct);
    tim_base_init_struct.tim_clockdivision = TIM_CKD_DIV1;
    tim_base_init_struct.tim_countermode = TIM_COUNTERMODE_UP;
    tim_base_init_struct.tim_period = 34 - 1;
    tim_base_init_struct.tim_prescaler = 1 - 1;
    tim_base_init_struct.tim_repetitioncounter = 0;
    tim_timebaseinit(TIM1, &tim_base_init_struct);

    tim_ocstructinit(&tim_oc_init_struct);
    tim_oc_init_struct.tim_ocmode = TIM_OCMODE_PWM1;
    tim_oc_init_struct.tim_outputstate = TIM_OUTPUTSTATE_ENABLE;
    tim_oc_init_struct.tim_outputnstate = TIM_OUTPUTNSTATE_DISABLE;
    tim_oc_init_struct.tim_pulse = 17 - 1;
    tim_oc_init_struct.tim_ocpolarity = TIM_OCPOLARITY_HIGH;
    tim_oc_init_struct.tim_ocidlestate = TIM_OCNIDLESTATE_RESET;
    tim_oc1init(TIM1, &tim_oc_init_struct);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);

    tim_cmd(TIM1, ENABLE);
    tim_ctrlpwmoutputs(TIM1, ENABLE);
}

#endif
