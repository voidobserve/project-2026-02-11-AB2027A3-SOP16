#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "timer_capture.h"

#define TIMER_CAPTURE_PORT              GPIOA_REG
#define TIMER_CAPTURE_PIN               GPIO_PIN_2

volatile u32 tmr_capture_overflow_cnt = 0;

AT(.com_rodata.capture)
const char capture_info[] = "capture! interval: %dms\n";
AT(.com_rodata.capture)
const char overflow_info[] = "overflow!\n";


AT(.com_text.isr)
void timer3_cap_handle(void)
{
    u32 interval;

    if (tmr_get_flag(TMR3, TMR_IT_CAPTURE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_CAPTURE);
        interval = tmr_capture_overflow_cnt * (tmr_get_period(TMR3) + 1) + tmr_get_counter(TMR3);
        interval = (interval + 500) / 1000;
        tmr_capture_overflow_cnt = 0;
        tmr_set_counter(TMR3, 0);
        printf(capture_info, interval);
    }

    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        tmr_capture_overflow_cnt++;
        printf(overflow_info);
    }
}

void timer3_capture_init(u32 period)
{
    gpio_init_typedef gpio_init_structure;
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    gpio_init_structure.gpio_pin = TIMER_CAPTURE_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PD10K;
    gpio_init(TIMER_CAPTURE_PORT, &gpio_init_structure);

    gpio_func_mapping_config(TIMER_CAPTURE_PORT, TIMER_CAPTURE_PIN, GPIO_CROSSBAR_IN_T3CAP);

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;
    tmr_base_init_struct.period = period;
    tmr_base_init_struct.prescale = 0;
    tmr_base_init(TMR3, &tmr_base_init_struct);

    tmr_capture_config(TMR3, TMR_CAP_EDGE);
    tmr_pic_config(TMR3, timer3_cap_handle, 0, TMR_IT_CAPTURE | TMR_IT_UPDATE, ENABLE);

    tmr_cmd(TMR3, ENABLE);
    tmr_capture_cmd(TMR3, ENABLE);
}

void timer3_capture_example(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, 1000)) {
        printf("---> system is running.\n");
        delay_cnt = tick_get();
    }
}
