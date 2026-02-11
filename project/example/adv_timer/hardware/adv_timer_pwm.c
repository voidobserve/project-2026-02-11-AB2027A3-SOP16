#include "include.h"
#include "adv_timer_pwm.h"

#define             TIM_PEROID_VALUE               (2000 - 1)
#define             TIM_PRESCALER_VALUE            (48 - 1)

static uint16_t getcap_value = 0;
static uint16_t cap_value_temp = 0;
static uint16_t set_pluse_value =  100;
static uint16_t set_pluse_value1 = 250;
static uint16_t set_pluse_value2 = 500;
static uint16_t set_pluse_value3 = 750;
static uint16_t set_pluse_value4 = 1000;


AT(.com_text.isr)
static void adv_timer_isr(void)
{
    if(tim_getitstatus(TIM1, TIM_IT_CC1) != RESET)
    {
        tim_clearitpendingbit(TIM1,TIM_IT_CC1);
        getcap_value = TIM1->ccr1;
        cap_value_temp = getcap_value + set_pluse_value;
        if(cap_value_temp >= TIM_PEROID_VALUE){
            cap_value_temp = set_pluse_value1;
        }
        tim_setcompare1(TIM1, cap_value_temp);
    }
    else if(tim_getitstatus(TIM1, TIM_IT_CC2) != RESET)
    {
        tim_clearitpendingbit(TIM1,TIM_IT_CC2);
        getcap_value = TIM1->ccr1;
        cap_value_temp = getcap_value + set_pluse_value;
        if(cap_value_temp >= TIM_PEROID_VALUE){
            cap_value_temp = set_pluse_value2;
        }
        tim_setcompare2(TIM1, cap_value_temp);
    }
    else if(tim_getitstatus(TIM1, TIM_IT_CC3) != RESET)
    {
        tim_clearitpendingbit(TIM1,TIM_IT_CC3);
        getcap_value = TIM1->ccr1;
        cap_value_temp = getcap_value + set_pluse_value;
        if(cap_value_temp >= TIM_PEROID_VALUE){
            cap_value_temp = set_pluse_value3;
        }
        tim_setcompare3(TIM1, cap_value_temp);
    }
    else if(tim_getitstatus(TIM1, TIM_IT_CC4) != RESET)
    {
        tim_clearitpendingbit(TIM1,TIM_IT_CC4);
        getcap_value = TIM1->ccr1;
        cap_value_temp = getcap_value + set_pluse_value;
        if(cap_value_temp >= TIM_PEROID_VALUE){
            cap_value_temp = set_pluse_value4;
        }
        tim_setcompare4(TIM1, cap_value_temp);
    }
}
static void adv_timer_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_0 | GPIO_PIN_1 |
                                   GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5 |
                                   GPIO_PIN_6;
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
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_6, GPIO_CROSSBAR_OUT_TOC4);

}

static void adv_timer_init(void)
{

    tim_timebaseinittypedef adv_tim_timebasestructure;
    tim_ocinittypedef adv_tim_ocinitstructure;

    tim_timebasestructinit(&adv_tim_timebasestructure);

    adv_tim_timebasestructure.tim_clock_source   =   TIM_SYSTEM_CLOCK;
    adv_tim_timebasestructure.tim_clockdivision =    TIM_CKD_DIV1;
    adv_tim_timebasestructure.tim_countermode   =    TIM_COUNTERMODE_UP;
    adv_tim_timebasestructure.tim_period        =    TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler     =    TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode       =  TIM_OCMODE_PWM1;
    adv_tim_ocinitstructure.tim_outputstate  =  TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_outputnstate =  TIM_OUTPUTNSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_pulse        =  set_pluse_value1;
    adv_tim_ocinitstructure.tim_ocpolarity   =  TIM_OCPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocnpolarity  =  TIM_OCNPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocidlestate  =  TIM_OCNIDLESTATE_RESET;
    adv_tim_ocinitstructure.tim_ocnidlestate =  TIM_OCNIDLESTATE_RESET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value2;
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value3;
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value4;
    tim_oc4init(TIM1, &adv_tim_ocinitstructure);

    tim_itconfig(TIM1,TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,ENABLE);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc2preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc3preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc4preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);

    tim_cmd(TIM1, ENABLE);
    tim_ctrlpwmoutputs(TIM1,ENABLE);

}
void adv_timer_sevenchannelpwm_example(void)
{
    CLKGAT0 |= BIT(8);

    adv_timer_gpio_init();

    sys_irq_init(IRQn_TMR0,0,adv_timer_isr);

    adv_timer_init();
    while(1)
    {
        WDT_CLR();
    }
}
