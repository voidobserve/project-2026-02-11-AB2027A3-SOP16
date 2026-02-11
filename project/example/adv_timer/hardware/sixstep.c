#include "include.h"
#include "sixstep.h"
#include "driver_tmr.h"

#define             TIM_PEROID_VALUE               (2000 - 1)
#define             TIM_PRESCALER_VALUE            (48 - 1)

static uint16_t set_pluse_value = 0;
static uint8_t step = 1;

//AT(.com_text.isr)
//static void timer3_isr(void)
//{
//    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET)
//    {
//        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
//        TIM_GenerateEvent(TIM1,TIM_EventSource_COM);
//    }
//
//}

AT(.com_text.irq) FIQ
static void adv_timer_isr(void)
{
   tim_clearitpendingbit(TIM1, TIM_IT_COM);

  if (step == 1)
  {
    /* Next step: Step 2 Configuration ---------------------------- */
    /*  Channel3 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_DISABLE);

    /*  Channel1 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_1, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_ENABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_1, TIM_CCXN_DISABLE);

    /*  Channel2 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_2, TIM_OCMODE_PWM1 );
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_ENABLE);
    step++;
  }
  else if (step == 2)
  {
    /* Next step: Step 3 Configuration ---------------------------- */
    /*  Channel2 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_2, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_ENABLE);

    /*  Channel3 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_3, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_ENABLE);
     tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_DISABLE);

    /*  Channel1 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_1, TIM_CCXN_DISABLE);
    step++;
  }
  else if (step == 3)
  {
    /* Next step: Step 4 Configuration ---------------------------- */
    /*  Channel3 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_3, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_ENABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_DISABLE);

    /*  Channel2 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_DISABLE);

    /*  Channel1 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_1, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_1, TIM_CCXN_ENABLE);
    step++;
  }
  else if (step == 4)
  {
    /* Next step: Step 5 Configuration ---------------------------- */
    /*  Channel3 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_DISABLE);

    /*  Channel1 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_1, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_1, TIM_CCXN_ENABLE);

    /*  Channel2 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_2, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_ENABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_DISABLE);
    step++;
  }
  else if (step == 5)
  {
    /* Next step: Step 6 Configuration ---------------------------- */
    /*  Channel3 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_3, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_ENABLE);

    /*  Channel1 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_1, TIM_CCXN_DISABLE);

    /*  Channel2 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_2, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_ENABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_DISABLE);
    step++;
  }
  else
  {
    /* Next step: Step 1 Configuration ---------------------------- */
    /*  Channel1 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_1, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_1, TIM_CCX_ENABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_DISABLE);

    /*  Channel3 configuration */
    tim_selectocxm(TIM1, TIM_CHANNEL_3, TIM_OCMODE_PWM1);
    tim_ccxcmd(TIM1, TIM_CHANNEL_3, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_3, TIM_CCXN_ENABLE);

    /*  Channel2 configuration */
    tim_ccxcmd(TIM1, TIM_CHANNEL_2, TIM_CCX_DISABLE);
    tim_ccxncmd(TIM1, TIM_CHANNEL_2, TIM_CCXN_DISABLE);
    step = 1;
  }
}
static void tmr3_init(void)
{
    tmr_base_init_typedef tmr_base_init_struct;

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;       // 1MHz
    tmr_base_init_struct.prescale = 100;                        // 10KHz
    tmr_base_init_struct.period = 1000;                          // 100ms
    tmr_base_init(TMR3, &tmr_base_init_struct);

    tmr_pwm_duty_config(TMR3, TMR_PWM2, 50);
    tmr_pwm_cmd(TMR3, TMR_PWM2, ENABLE);
}
static void adv_timer_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_0 | GPIO_PIN_1 |
                                   GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_24MA;

    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_0, GPIO_CROSSBAR_OUT_TOC1);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_1, GPIO_CROSSBAR_OUT_TOC1N);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_2, GPIO_CROSSBAR_OUT_TOC2);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_3, GPIO_CROSSBAR_OUT_TOC2N);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_4, GPIO_CROSSBAR_OUT_TOC3);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_5, GPIO_CROSSBAR_OUT_TOC3N);
}
static void adv_timer_init(void)
{
    tim_timebaseinittypedef adv_tim_timebasestructure;
    tim_ocinittypedef adv_tim_ocinitstructure;
    tim_bdtrinittypedef adv_tim_bdtrinitstruct;

    tim_timebasestructinit(&adv_tim_timebasestructure);

    adv_tim_timebasestructure.tim_clock_source      = TIM_SYSTEM_CLOCK;
    adv_tim_timebasestructure.tim_clockdivision     = TIM_CKD_DIV1;
    adv_tim_timebasestructure.tim_countermode       = TIM_COUNTERMODE_UP;
    adv_tim_timebasestructure.tim_period            = TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler         = TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode       = TIM_OCMODE_TIMING;
    adv_tim_ocinitstructure.tim_outputstate  = TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_outputnstate = TIM_OUTPUTNSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_pulse        = set_pluse_value;
    adv_tim_ocinitstructure.tim_ocpolarity   = TIM_OCPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocnpolarity  = TIM_OCNPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocidlestate  = TIM_OCNIDLESTATE_RESET;
    adv_tim_ocinitstructure.tim_ocnidlestate = TIM_OCNIDLESTATE_RESET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);
    tim_oc4init(TIM1, &adv_tim_ocinitstructure);

    adv_tim_bdtrinitstruct.tim_automaticoutput = TIM_AUTOMATICOUTPUT_ENABLE;
    adv_tim_bdtrinitstruct.tim_break           = TIM_BREAK_ENABLE;
    adv_tim_bdtrinitstruct.tim_breakpolarity   = TIM_BREAKPOLARITY_HIGH;
    adv_tim_bdtrinitstruct.tim_deadtime        = 0X40;
    adv_tim_bdtrinitstruct.tim_locklevel       = TIM_LOCKLEVEL_OFF;
    adv_tim_bdtrinitstruct.tim_ossistate       = TIM_OSSISTATE_ENABLE;
    adv_tim_bdtrinitstruct.tim_ossrstate       = TIM_OSSRSTATE_ENABLE;

    tim_bdtrconfig(TIM1, &adv_tim_bdtrinitstruct);

    tim_itconfig(TIM1,TIM_IT_COM,ENABLE);

    tim_ccpreloadcontrol(TIM1, ENABLE);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc2preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc3preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc4preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);

    tim_cmd(TIM1, ENABLE);

    tim_ctrlpwmoutputs(TIM1,ENABLE);
}
void adv_timer_sixstep_example(void)
{
    WDT_DIS();
    /*tim clk*/
    CLKGAT0 |= BIT(8);
    /*tmr3 clk*/
    CLKGAT1 |= BIT(8);
    set_pluse_value = TIM_PEROID_VALUE/4;

    adv_timer_gpio_init();

    sys_irq_init(IRQn_TMR0,1,adv_timer_isr);

    adv_timer_init();

    tmr3_init();

    while(1)
    {
        if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET)
        {
            tmr_clear_flag(TMR3, TMR_IT_UPDATE);
            tim_generateevent(TIM1,TIM_EVENTSOURCE_COM);
//            printf("step = %d\n",step);
        }
    }
}

