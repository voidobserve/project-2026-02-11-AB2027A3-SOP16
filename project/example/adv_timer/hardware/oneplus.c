#include "oneplus.h"

#define             TIM_PEROID_VALUE               (2000 - 1)
#define             TIM_PRESCALER_VALUE            (48 - 1)

static uint16_t set_pluse_value1 = 250;
static uint16_t set_pluse_value2 = 500;
static uint16_t set_pluse_value3 = 1000;

AT(.com_text.irq) FIQ
static void timer3_fiq(void)
{
    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET)
    {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        GPIOA ^= 1 << 4;

    }
}

static void tmrclk_diswdt(void)
{
    WDT_DIS();

    clk_gate0_cmd(CLK_GATE0_TMR0,CLK_EN);
    clk_gate1_cmd(CLK_GATE1_TMR3,CLK_EN);

}
static void adv_timer_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_0 |
                                   GPIO_PIN_1 |
                                   GPIO_PIN_2 ;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_24MA;

    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_init_structure.gpio_pin = GPIO_PIN_3;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_init_structure.gpio_pin = GPIO_PIN_4;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_0, GPIO_CROSSBAR_OUT_TOC1);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_1, GPIO_CROSSBAR_OUT_TOC2);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_2, GPIO_CROSSBAR_OUT_TOC3);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_3, GPIO_CROSSBAR_IN_TETR);

}
static void timer3_pwm_init(void)
{
    tmr_base_init_typedef tmr_base_init_struct;

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;       // 1MHz
    tmr_base_init_struct.prescale = 100;                        // 10KHz
    tmr_base_init_struct.period = 100;                          // 10ms
    tmr_base_init(TMR3, &tmr_base_init_struct);

    tmr_pic_config(TMR3, timer3_fiq, 1, TMR_IT_UPDATE, ENABLE);

    tmr_cmd(TMR3, ENABLE);
}


static void adv_timer_init(void)
{
    tim_timebaseinittypedef adv_tim_timebasestructure;
    tim_ocinittypedef adv_tim_ocinitstructure;

    tim_timebasestructinit(&adv_tim_timebasestructure);

    adv_tim_timebasestructure.tim_clock_source      = TIM_SYSTEM_CLOCK;
    adv_tim_timebasestructure.tim_clockdivision     = TIM_CKD_DIV1;
    adv_tim_timebasestructure.tim_countermode       = TIM_COUNTERMODE_UP;
    adv_tim_timebasestructure.tim_period            = TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler         = TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode      = TIM_OCMODE_PWM2;
    adv_tim_ocinitstructure.tim_outputstate = TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_pulse       = set_pluse_value1;
    adv_tim_ocinitstructure.tim_ocpolarity  = TIM_OCPOLARITY_HIGH;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value2;
    adv_tim_ocinitstructure.tim_ocpolarity = TIM_OCPOLARITY_LOW;
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value3;
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);

    tim_selectonepulsemode(TIM1,TIM_OPMODE_SINGLE);

    tim_selectinputtrigger(TIM1,TIM_TS_ETRF);

    tim_selectslavemode(TIM1, TIM_SLAVEMODE_TRIGGER);

    tim_ctrlpwmoutputs(TIM1,ENABLE);
}
void adv_timer_oneplus_example(void)
{
    tmrclk_diswdt();

    adv_timer_gpio_init();

    timer3_pwm_init();

    adv_timer_init();
//    printf("TIM_SMCR = %x\n",TIM_SMCR);
//    printf("a = %d\n",TIM1->ARR);
//    printf("B = %d\n",TIM1->ccr1);
//    printf("C = %d\n",TIM1->ccr2);
//    printf("d = %d\n",TIM1->ccr3);
    while(1)
    {
        /*xbox trigger is not normal*/
        if(tim_getflagstatus(TIM1,TIM_FLAG_TRIGGER) != RESET)
        {
            GPIOA ^= 1 << 9;
            tim_clearflag(TIM1,TIM_FLAG_TRIGGER);
        }
    }

}

