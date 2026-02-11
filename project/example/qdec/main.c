#include "include.h"
#include "bsp_uart_debug.h"
#include "driver_qdec.h"
#include "driver_gpio.h"

#define QDEC_PORT0_SEL                  GPIOA_REG
#define QDEC_PIN0_SEL                   GPIO_PIN_0

#define QDEC_PORT1_SEL                  GPIOA_REG
#define QDEC_PIN1_SEL                   GPIO_PIN_1

volatile int step_count = 0;
static int16_t last_cnt = 0;

AT(.com_periph.qdec.isr)
void qdec_isr(void)
{
    if(qdec_get_flag(QDECX, QDEC_FORWARD_DIR_PND | QDEC_REVERSE_DIR_PND)) {
        qdec_clear_flag(QDECX, QDEC_REVERSE_DIR_PND | QDEC_FORWARD_DIR_PND);
        step_count = qdec_get_data(QDECX);
        step_count <<= 20;
        step_count >>= 20;
    } else if(qdec_get_flag(QDECX, QDEC_TERR_PND)) {
        qdec_clear_flag(QDECX, QDEC_TERR_PND);
        step_count = qdec_get_data(QDECX);
    } else if(qdec_get_flag(QDECX, QDEC_OV_PND)) {
        qdec_clear_flag(QDECX, QDEC_OV_PND);
    }
}

int main(void)
{
    WDT_DIS();
    bsp_uart_debug_init();

    printf("bsp qdec example\n");

    gpio_init_typedef gpio_init_structure;
    qdec_init_typedef qdec_init_struct;

    /************************* GPIO初始化 *************************/
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pin = QDEC_PIN0_SEL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init(QDEC_PORT0_SEL, &gpio_init_structure);

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pin = QDEC_PIN1_SEL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init(QDEC_PORT1_SEL, &gpio_init_structure);

    /*********************** Crossbar映射 ************************/
    gpio_func_mapping_config(QDEC_PORT0_SEL, QDEC_PIN0_SEL, GPIO_CROSSBAR_IN_QDECXDI0);
    gpio_func_mapping_config(QDEC_PORT1_SEL, QDEC_PIN1_SEL, GPIO_CROSSBAR_IN_QDECXDI1);

    /************************** QDEC初始化 **************************/
    qdec_init_struct.clk_source = QDEC_X24M_32K_CLK;
    qdec_init_struct.smp_num_sel = QDEC_SMP_NUM_64;
    qdec_init_struct.cm_sel = QDEC_CM_X4;
    qdec_init_struct.wakup_en = QDEC_WAKEUP_ENABLE;
    qdec_init_struct.tm_en = QDEC_TM_DISABLE;
    qdec_init_struct.fil_len = 40;

    qdec_init(QDECX, &qdec_init_struct);

    qdec_pic_config(QDECX, qdec_isr, 0, QDEC_IT_REVERSE_DIR | QDEC_IT_FORWARD_DIR, ENABLE);

    qdec_cmd(QDECX, ENABLE);

    while(1) {
        WDT_CLR();
        if(last_cnt != step_count){
            printf("last_cnt:%d, cur_cnt:%d\n", last_cnt, step_count);
            last_cnt = step_count;
        }
    }

    return 0;
}
