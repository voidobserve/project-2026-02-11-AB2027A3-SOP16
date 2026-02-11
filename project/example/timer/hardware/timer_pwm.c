#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "timer_pwm.h"

#define TIMER_PWM_PORT              GPIOA_REG
#define TIMER_PWM_PIN               GPIO_PIN_1

void timer3_pwm_init(void)
{
    gpio_init_typedef gpio_init_structure;
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    gpio_init_structure.gpio_pin = TIMER_PWM_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(TIMER_PWM_PORT, &gpio_init_structure);

    gpio_func_mapping_config(TIMER_PWM_PORT, TIMER_PWM_PIN, GPIO_CROSSBAR_OUT_T3PWM2);

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;       // 1MHz
    tmr_base_init_struct.prescale = 100 - 1;                    // 10KHz = 0.1ms
    tmr_base_init_struct.period = 102 - 1;                      // Period=101(10.2ms), means 0 <= duty <= 100
    tmr_base_init(TMR3, &tmr_base_init_struct);                 // Upper Count

    /* Duty(x) should be: 0 <= x < period */
    tmr_pwm_duty_config(TMR3, TMR_PWM2, 51 - 1);                // Valid Output: L, Defautl set PWM to 50%
    tmr_pwm_cmd(TMR3, TMR_PWM2, ENABLE);
}

void timer3_pwm_example(void)
{
    static u32 delay_cnt;
    static u16 duty = 0;
    static u8 dir = 1;
    u16 period = tmr_get_period(TMR3);
    u8 interval = 5;

    /* We set interval to 10ms, and duty will be increase from 0 to 100, than decrease from 100 to 0 */
    /* This means that a breath takes 1s */
    if (tick_check_expire(delay_cnt, interval)) {
        delay_cnt = tick_get();
        if (dir == 1) {
            duty += 1;
        } else {
            duty -= 1;
        }
        if (duty >= period) {
            dir = 0;
        } else if (duty <= 1) {
            dir = 1;
        }
        tmr_pwm_duty_config(TMR3, TMR_PWM2, duty - 1);
    }
}
