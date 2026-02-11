#include "cenatct_sgl.h"
#define             TIM_PEROID_VALUE               (2000 - 1)
#define             TIM_PRESCALER_VALUE            (48 - 1)

static uint16_t set_pluse_value = 300;

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
    adv_tim_timebasestructure.tim_clockdivision     = TIM_CKD_DIV2;
    adv_tim_timebasestructure.tim_countermode       = TIM_COUNTERMODE_CENTERALIGNED1;
    adv_tim_timebasestructure.tim_period            = TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler         = TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode       = TIM_OCMODE_PWM1;
    adv_tim_ocinitstructure.tim_outputstate  = TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_outputnstate = TIM_OUTPUTNSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_pulse        = set_pluse_value;
    adv_tim_ocinitstructure.tim_ocpolarity   = TIM_OCPOLARITY_LOW;
    adv_tim_ocinitstructure.tim_ocnpolarity  = TIM_OCNPOLARITY_LOW;
    adv_tim_ocinitstructure.tim_ocidlestate  = TIM_OCIDLESTATE_SET;
    adv_tim_ocinitstructure.tim_ocnidlestate = TIM_OCNIDLESTATE_SET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_ocmode = TIM_OCMODE_PWM2;
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_ocmode = TIM_OCMODE_PWM1;
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);

    adv_tim_bdtrinitstruct.tim_automaticoutput = TIM_AUTOMATICOUTPUT_ENABLE;
    adv_tim_bdtrinitstruct.tim_break           = TIM_BREAK_ENABLE;
    adv_tim_bdtrinitstruct.tim_breakpolarity   = TIM_BREAKPOLARITY_HIGH;
    adv_tim_bdtrinitstruct.tim_deadtime        = 0X80;
    adv_tim_bdtrinitstruct.tim_locklevel       = TIM_LOCKLEVEL_OFF;
    adv_tim_bdtrinitstruct.tim_ossistate       = TIM_OSSISTATE_ENABLE;
    adv_tim_bdtrinitstruct.tim_ossrstate       = TIM_OSSRSTATE_ENABLE;

    tim_bdtrconfig(TIM1, &adv_tim_bdtrinitstruct);

    tim_ccpreloadcontrol(TIM1, ENABLE);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc2preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc3preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_oc4preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);

    tim_cmd(TIM1, ENABLE);

}
void adv_timer_cenatcesgl_example(void)
{
    WDT_DIS();

    CLKGAT0 |= BIT(8);

    adv_timer_gpio_init();

    adv_timer_init();

    while(1);
}
