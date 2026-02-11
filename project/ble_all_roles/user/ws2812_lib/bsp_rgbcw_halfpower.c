#include "include.h"
#include "driver_tmr.h"
#include "bsp_rgbcw_halfpower.h"

#if BSP_RGBCW_EN

#define TIM_FREQ                        (1000)//(24000)

/* isr_hz = sys_clk[def=24MHz] / (TIM_PEROID_VALUE + 1) / (TIM_PRESCALER_VALUE + 1) */
/* isr_hz = 24000000 / 2000 / 48 = 250hz(4ms) */

#if TIM_FREQ == 24000
#define TIM_LEVEL_MAX                   (1000)
#define TIM_PEROID_VALUE                (TIM_LEVEL_MAX - 1)
#define TIM_PRESCALER_VALUE             (1 - 1)
#elif TIM_FREQ == 1000
#define TIM_LEVEL_MAX                   (1000)
#define TIM_PEROID_VALUE                (TIM_LEVEL_MAX - 1)
#define TIM_PRESCALER_VALUE             (16 - 1)
#else
#error "plz set tim freq"
#endif

#define PWM_PORT_R                      GPIOA_REG
#define PWM_PIN_R                       GPIO_PIN_0
#define PWM_PORT_G                      GPIOA_REG
#define PWM_PIN_G                       GPIO_PIN_1
#define PWM_PORT_B                      GPIOA_REG
#define PWM_PIN_B                       GPIO_PIN_2
#define PWM_PORT_W                      GPIOA_REG
#define PWM_PIN_W                       GPIO_PIN_3
#define PWM_PORT_Y                      GPIOA_REG
#define PWM_PIN_Y                       GPIO_PIN_4

#define TIM3_PWM_DUTY(duty)             ((duty) == 0 ? 0xFFFF : (TIM_LEVEL_MAX - duty))

static void adv_timer_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init_structure.gpio_pin = PWM_PIN_R;
    gpio_init(PWM_PORT_R, &gpio_init_structure);

    gpio_init_structure.gpio_pin = PWM_PIN_G;
    gpio_init(PWM_PORT_G, &gpio_init_structure);

    gpio_init_structure.gpio_pin = PWM_PIN_B;
    gpio_init(PWM_PORT_B, &gpio_init_structure);

    gpio_init_structure.gpio_pin = PWM_PIN_W;
    gpio_init(PWM_PORT_W, &gpio_init_structure);

    gpio_init_structure.gpio_pin = PWM_PIN_Y;
    gpio_init(PWM_PORT_Y, &gpio_init_structure);

    gpio_func_mapping_config(PWM_PORT_W, PWM_PIN_W, GPIO_CROSSBAR_OUT_TOC1);
#if BSP_RGBCW_HALFPOWER_EN
    gpio_func_mapping_config(PWM_PORT_Y, PWM_PIN_Y, GPIO_CROSSBAR_OUT_TOC1N);
#elif BSP_RGBCW_FULLPOWER_EN
    gpio_func_mapping_config(PWM_PORT_Y, PWM_PIN_Y, GPIO_CROSSBAR_OUT_T3PWM0);
#endif
    gpio_func_mapping_config(PWM_PORT_R, PWM_PIN_R, GPIO_CROSSBAR_OUT_TOC2);
    gpio_func_mapping_config(PWM_PORT_G, PWM_PIN_G, GPIO_CROSSBAR_OUT_TOC3);
    gpio_func_mapping_config(PWM_PORT_B, PWM_PIN_B, GPIO_CROSSBAR_OUT_TOC4);
}

static void adv_timer_init(void)
{
#if BSP_RGBCW_FULLPOWER_EN
    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);
#endif

    tim_timebaseinittypedef adv_tim_timebasestructure;
    tim_ocinittypedef adv_tim_ocinitstructure;

    tim_timebasestructinit(&adv_tim_timebasestructure);

    adv_tim_timebasestructure.tim_clockdivision =    TIM_CKD_DIV1;
    adv_tim_timebasestructure.tim_countermode   =    TIM_COUNTERMODE_UP;
    adv_tim_timebasestructure.tim_period        =    TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler     =    TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode       =  TIM_OCMODE_PWM1;
    adv_tim_ocinitstructure.tim_outputstate  =  TIM_OUTPUTSTATE_ENABLE;
#if BSP_RGBCW_HALFPOWER_EN
    adv_tim_ocinitstructure.tim_outputnstate =  TIM_OUTPUTNSTATE_ENABLE;
#elif BSP_RGBCW_FULLPOWER_EN
    adv_tim_ocinitstructure.tim_outputnstate =  TIM_OUTPUTNSTATE_DISABLE;
#endif
    adv_tim_ocinitstructure.tim_pulse        =  0;
    adv_tim_ocinitstructure.tim_ocpolarity   =  TIM_OCPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocnpolarity  =  TIM_OCNPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocidlestate  =  TIM_OCNIDLESTATE_RESET;
    adv_tim_ocinitstructure.tim_ocnidlestate =  TIM_OCNIDLESTATE_RESET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = 0;
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = 0;
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = 0;
    tim_oc4init(TIM1, &adv_tim_ocinitstructure);

    // tim_itconfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc2preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc3preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc4preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);

    tim_cmd(TIM1, ENABLE);
    tim_ctrlpwmoutputs(TIM1, ENABLE);

#if BSP_RGBCW_FULLPOWER_EN
    tmr_base_init_typedef tmr_base_init_struct;
    tmr_base_init_struct.clock_source = TMR_SYSTEM_CLOCK;       // TMR_COUNTER_RISING:1MHz, TMR_SYSTEM_CLOCK:24Mhz
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;
    tmr_base_init_struct.prescale = TIM_PRESCALER_VALUE;        // 10KHz = 0.1ms
    tmr_base_init_struct.period = TIM_PEROID_VALUE + 2;         // Period=101(10.2ms), means 0 <= duty <= (TIM_PEROID_VALUE + 1)
    tmr_base_init(TMR3, &tmr_base_init_struct);                 // Upper Count

    /* Duty(x) should be: 0 <= x < period */
    tmr_pwm_duty_config(TMR3, TMR_PWM0, TIM3_PWM_DUTY(0));     // Valid Output: L, Defautl set PWM to 50%
    tmr_pwm_cmd(TMR3, TMR_PWM0, ENABLE);
#endif
}

void adv_timer_rgbcw_init(void)
{
    CLKGAT0 |= BIT(8);

    adv_timer_gpio_init();
    
    adv_timer_init();
}

void adv_timer_rgbcw_set_duty(u8 index, u16 duty)
{
    if (duty > TIM_LEVEL_MAX) duty = TIM_LEVEL_MAX;
    
    if (index == 0)
        tim_setcompare1(TIM1, duty);
    else if (index == 1)
#if BSP_RGBCW_HALFPOWER_EN
        tim_setcompare1(TIM1, TIM_LEVEL_MAX - duty);
#elif BSP_RGBCW_FULLPOWER_EN
    {
        tmr_pwm_duty_config(TMR3, TMR_PWM0, TIM3_PWM_DUTY(duty));
        /*因无法达到100%, 需要关闭映射直接输出高电平*/
        if (duty == TIM_LEVEL_MAX)
            PWM_PORT_Y->fen &= ~PWM_PIN_Y;
        else
            PWM_PORT_Y->fen |= PWM_PIN_Y;
        gpio_set_bits(PWM_PORT_Y, PWM_PIN_Y);
    }
#endif
    else if (index == 2)
        tim_setcompare2(TIM1, duty);
    else if (index == 3)
        tim_setcompare3(TIM1, duty);
    else if (index == 4)
        tim_setcompare4(TIM1, duty);
}

#if BSP_RGBCW_HALFPOWER_EN
void adv_timer_rgbcw_halfpower_cw_close(void)
{
    tim_ocinittypedef adv_tim_ocinitstructure;
    
    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode = TIM_OCMODE_PWM1;
    adv_tim_ocinitstructure.tim_outputstate = TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_outputnstate = TIM_OUTPUTNSTATE_DISABLE;
    adv_tim_ocinitstructure.tim_pulse = 0;
    adv_tim_ocinitstructure.tim_ocpolarity = TIM_OCPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocnpolarity = TIM_OCNPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocidlestate = TIM_OCNIDLESTATE_RESET;
    adv_tim_ocinitstructure.tim_ocnidlestate = TIM_OCNIDLESTATE_RESET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
}
#endif

void adv_timer_rgbcw_process(void)
{
    return;
    static u32 delay_cnt;
    static u32 duty = 0;

    if (tick_check_expire(delay_cnt, 10))
    {
        delay_cnt = tick_get();
        if (duty >= TIM_LEVEL_MAX)
        {
            if (duty == TIM_LEVEL_MAX)
            {
                duty++;
#if BSP_RGBCW_HALFPOWER_EN
                adv_timer_rgbcw_halfpower_cw_close();
#endif
            }
            return;
        }
        else
            duty += 5;

        adv_timer_rgbcw_set_duty(0, duty);
        adv_timer_rgbcw_set_duty(1, TIM_LEVEL_MAX - duty);
#if !WS2812_LIB_EN
        adv_timer_rgbcw_set_duty(2, TIM_LEVEL_MAX - duty);
        adv_timer_rgbcw_set_duty(3, duty);
        adv_timer_rgbcw_set_duty(4, duty);
#endif
    }
}

#if WS2812_LIB_EN
void adv_timer_rgbcw_update_rgb(u8 r, u8 g, u8 b)
{
    adv_timer_rgbcw_set_duty(2, r * TIM_LEVEL_MAX / 255);
    adv_timer_rgbcw_set_duty(3, g * TIM_LEVEL_MAX / 255);
    adv_timer_rgbcw_set_duty(4, b * TIM_LEVEL_MAX / 255);
}
#endif

#endif
