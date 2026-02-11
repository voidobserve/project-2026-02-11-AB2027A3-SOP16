#include "ocative.h"
#define             TIM_PEROID_VALUE               (2000 - 1)
#define             TIM_PRESCALER_VALUE            (48 - 1)

static uint16_t set_pluse_value1 = 250;
static uint16_t set_pluse_value2 = 500;
static uint16_t set_pluse_value3 = 1000;

static void tmrclk_diswdt(void)
{
    WDT_DIS();

    clk_gate0_cmd(CLK_GATE0_TMR0,CLK_EN);
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
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init(GPIOA_REG, &gpio_init_structure);

    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_0, GPIO_CROSSBAR_OUT_TOC1);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_1, GPIO_CROSSBAR_OUT_TOC2);
    gpio_func_mapping_config(GPIOA_REG, GPIO_PIN_2, GPIO_CROSSBAR_OUT_TOC3);
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

    adv_tim_ocinitstructure.tim_ocmode      = TIM_OCMODE_ACTIVE;
    adv_tim_ocinitstructure.tim_outputstate = TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_pulse       = set_pluse_value1;
    adv_tim_ocinitstructure.tim_ocpolarity  = TIM_OCPOLARITY_HIGH;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value2;
    tim_oc2init(TIM1, &adv_tim_ocinitstructure);
    adv_tim_ocinitstructure.tim_pulse = set_pluse_value3;
    tim_oc3init(TIM1, &adv_tim_ocinitstructure);

    tim_arrpreloadconfig(TIM1, ENABLE);

    tim_cmd(TIM1, ENABLE);

    tim_ctrlpwmoutputs(TIM1,ENABLE);
}

void adv_timer_ocative_example(void)
{
    tmrclk_diswdt();

    adv_timer_gpio_init();

    adv_timer_init();

    while(1);
}
