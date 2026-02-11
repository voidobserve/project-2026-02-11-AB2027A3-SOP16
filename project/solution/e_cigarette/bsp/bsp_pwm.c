#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"

void bsp_timer3_pwm_init(void)
{
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;       // 1MHz
    tmr_base_init_struct.prescale = 0;                      
    tmr_base_init_struct.period = 100 - 1;                      // pwm 10KHz : 1MHz/100
    tmr_base_init(TMR3, &tmr_base_init_struct);                 // Upper Count
    
}

void bsp_pwm_duty_set(u8 duty)
{
    printf("%s\n", __func__);
    if (duty > 100) {
        duty = 100;   
    }
    
    tmr_pwm_duty_config(TMR3, TMR_PWM0, 100 - duty);                
    tmr_pwm_cmd(TMR3, TMR_PWM0, ENABLE);
}

void bsp_timer3_pwm_deinit(void)
{
    tmr_deinit(TMR3);
}