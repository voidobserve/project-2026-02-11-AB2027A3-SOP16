#include "lpwm_led.h"

#define LPWM_LED_PORT               GPIOA_REG
#define LPWM_LED_PIN                GPIO_PIN_0

void lpwm_led_init(void)
{
    gpio_init_typedef gpio_init_structure;
    lpwm_stage_init_typedef lpwm_stage_init_struct;
    lpwm_base_init_typedef lpwm_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_LPWM, CLK_EN);

    gpio_init_structure.gpio_pin = LPWM_LED_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init(LPWM_LED_PORT, &gpio_init_structure);

    gpio_func_mapping_config(LPWM_LED_PORT, LPWM_LED_PIN, GPIO_CROSSBAR_OUT_LPWM);

    // lpwm ctrl init.
    lpwm_base_init_struct.mode = LPWM_MODE_BREATH;
    lpwm_base_init_struct.breath_start_dir = LPWM_DIR_INCREASE;
    lpwm_base_init_struct.period = 500;         // Max 511 (9bits), and the duty of each stage should less than period.
    lpwm_base_init_struct.first_duty = 100;
    lpwm_init(&lpwm_base_init_struct);

    // Stage A, must enable at breath mode.
    lpwm_stage_init_struct.repeat = 1;
    lpwm_stage_init_struct.step = 2;
    lpwm_stage_init_struct.start_duty = 50;
    lpwm_stage_config(LPWM_STAGE_A, &lpwm_stage_init_struct, ENABLE);

    // Stage B.
    lpwm_stage_init_struct.repeat = 1;
    lpwm_stage_init_struct.step = 10;
    lpwm_stage_init_struct.start_duty = 250;
    lpwm_stage_config(LPWM_STAGE_B, &lpwm_stage_init_struct, ENABLE);

    // Stage D, must enable at breath mode.
    lpwm_stage_init_struct.repeat = 1;
    lpwm_stage_init_struct.step = 8;
    lpwm_stage_init_struct.start_duty = 450;
    lpwm_stage_config(LPWM_STAGE_D, &lpwm_stage_init_struct, ENABLE);

    // Stage E.
    lpwm_stage_init_struct.repeat = 3;
    lpwm_stage_init_struct.step = 8;
    lpwm_stage_init_struct.start_duty = 300;
    lpwm_stage_config(LPWM_STAGE_E, &lpwm_stage_init_struct, ENABLE);

    // Max brightness hold time and min brightness hold time.
    lpwm_stage_init_struct.repeat = 100;
    lpwm_stage_config(LPWM_STAGE_L | LPWM_STAGE_H, &lpwm_stage_init_struct, ENABLE);

    lpwm_cmd(ENABLE);
}

void lpwm_led_example(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, 1000)) {
        printf("system is running.\n");
        delay_cnt = tick_get();
    }

    if (lpwm_get_flag(LPWM_FLAG_L_STAGE)) {
        printf("L Stage~\n");
        lpwm_clear_flag(LPWM_FLAG_L_STAGE);
    }
}
