#include "include.h"
#include "driver_saradc.h"
#include "bsp_ecigs.h"


#if (BSP_ECIGS_EN == ECIG_CONTROL_V1)

smoke_ctrl_t smk_ctrl={0};

AT(.com_text.init)
uint8_t mic_start_or_not(void)
{
    return gpio_read_bit(BSP_MIC_PORT, BSP_MIC_PIN);
}

// adc gnd输出低
AT(.com_text.init)
void bsp_adcgnd_gpio_init(void)
{
    if (BSP_ADCGND_PORT==GPIOA_REG) {
        GPIOAFEN &= ~BSP_ADCGND_PIN;
        GPIOADIR |=  BSP_ADCGND_PIN;
        GPIOADE  |=  BSP_ADCGND_PIN;
        GPIOACLR |=  BSP_ADCGND_PIN;
    } else {
        GPIOBFEN &= ~BSP_ADCGND_PIN;
        GPIOBDIR |=  BSP_ADCGND_PIN;
        GPIOBDE  |=  BSP_ADCGND_PIN;
        GPIOBCLR |=  BSP_ADCGND_PIN;
    }
}

// adc gnd设置模拟态
AT(.com_text.init)
void bsp_adcgnd_gpio_deinit(void)
{
    GPIOBDE &= ~BSP_ADCGND_PIN;
}

// 计算U²
AT(.com_text.cal.power)
u32 calculate_power(u32 hot_voltage)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    return hot_voltage * hot_voltage / sc->hot_res;
}

AT(.com_text.timer_flag)
void clear_timer_flag(void)
{
    tmr_clear_flag(TMR0, TMR_IT_UPDATE);
}

AT(.com_text.timer_flag)
uint8_t get_timer_flag(void)
{
   return tmr_get_flag(TMR0, TMR_IT_UPDATE);
}

// 开启加热
AT(.com_text.pwm)
void ECIG_PWM_ON_FUNC(void)
{
    gpio_set_bits(PWM1_OUT_PORT, PWM1_OUT_PIN);
}

// 关闭加热
AT(.com_text.pwm)
void ECIG_PWM_OFF_FUNC(void)
{
    gpio_reset_bits(PWM1_OUT_PORT, PWM1_OUT_PIN);
}

#if ECIG_RES_ADC_EN
// 开启检阻
AT(.com_text.res)
void ECIG_VEN_ON(void)
{
    gpio_set_bits(PWM2_OUT_PORT, PWM2_OUT_PIN);
}

// 关闭检阻
AT(.com_text.res)
void ECIG_VEN_OFF(void)
{
    gpio_reset_bits(PWM2_OUT_PORT, PWM2_OUT_PIN);
}
#endif

AT(.com_text.pwm_ctrl)
uint8_t get_smoke_sta(void)
{
    return smk_ctrl.smoke_sta;
}

AT(.com_text.hot_proc)
void bsp_hot_proc(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;

    uint32_t adc_hot;
    saradc_kick_start(ADC_HOT | VBAT_DETECT_ASSIST_CHANNEL);
    while(!saradc_get_flag(SARADC_IT_FINISHED)); //转换标志
    adc_hot = saradc_get_data(ADC_HOT);
    sc->adc_vbg = saradc_get_data(VBAT_DETECT_ASSIST_CHANNEL);
    sc->AD_hot_voltage_mv =  adc_hot * VBAT_DETECT_ASSIST_VOLTAGE / sc->adc_vbg * 4 / 3;
    sc->AD_hot_voltage = (sc->AD_hot_voltage_mv << 13) / 1000;

}

// 获取vbat电压mV
AT(.com_text.vbat_proc)
void e_cigs_vbat_proc(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    if (bsp_vbat_proc()){
        sc->AD_BAT_voltage_mv = bsp_get_vbat_level(1);
    }
}

AT(.com_text.isr)
bool timer_hot_mic_work(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    if(sc->hot_time_tick && tick_check_expire(sc->hot_time_tick, (sc->max_hot_time * 1000))){
        sc->smoke_sta = SMOKE_TIMEOUT;
        sc->hot_time_tick = 0;
    }

    return (mic_start_or_not()                 //mic工作状态
        && (sc->smoke_sta != SMOKE_TIMEOUT)    //吸烟超时
        && (sc->smoke_sta != LOW_POWER)        //低电量
        && (sc->smoke_sta != SHORT_CIRCUIT)    //短路
        && (sc->smoke_sta != OPEN_CIRCUIT)     //开路
    );
}

AT(.com_text.isr)
void timer_hot_free_handle(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;

    ECIG_PWM_OFF_FUNC();
#if ECIG_RES_ADC_EN
    ECIG_VEN_OFF();
#endif
    bsp_adcgnd_gpio_deinit();
    if(!mic_start_or_not()){
        sc->smoke_sta = IDLE;
        sc->mic_start = 0;
        sc->timer_100us_cnt = 0;
        sc->hot_time_tick = 0;
    }

}

AT(.com_text.isr)
void timer_hot_det_status(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    u32 div = abs(sc->AD_BAT_voltage_mv - sc->AD_hot_voltage_mv);
    u32 res_prop = div ? sc->AD_hot_voltage_mv * 10 / div : sc->open_res_prop;

    if (res_prop <= sc->short_res_prop) {   //短路判断
        ECIG_PWM_OFF_FUNC();
        sc->smoke_sta = SHORT_CIRCUIT;
    } else if (res_prop >= sc->open_res_prop) {   //开路判断
        ECIG_PWM_OFF_FUNC();
        sc->smoke_sta = OPEN_CIRCUIT;
    } else if (sc->smoke_sta != SMOKING) {
        sc->smoke_sta = SMOKING;
    }
}

#if ECIG_RES_ADC_EN

AT(.com_text.proc.res)
void bsp_res_proc(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    saradc_kick_start(ADC_HOT | ADC2_RES | VBAT_DETECT_ASSIST_CHANNEL);
    while(!saradc_get_flag(SARADC_IT_FINISHED)); //转换标志
    sc->adc1 = saradc_get_data(ADC_HOT);
    sc->adc2 = saradc_get_data(ADC2_RES);
    sc->adc_vbg = saradc_get_data(VBAT_DETECT_ASSIST_CHANNEL);

}

AT(.com_rodata.res)
char res_str[] = "res:%d\n";
AT(.com_rodata.res)
char vol_str[] = "adc:%d, %d\n";

AT(.com_text.cal.res)
void caculate_hot_res(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    u16 ubat = sc->adc2 * VBAT_DETECT_ASSIST_VOLTAGE / sc->adc_vbg;     //转换mV
    u16 uhot = sc->adc1 * VBAT_DETECT_ASSIST_VOLTAGE / sc->adc_vbg;
    sc->hot_res = 0;
    printf(vol_str, ubat, uhot);
    if (uhot == 0) {
        sc->smoke_sta = SHORT_CIRCUIT;
    } else if ((ubat * 10 / uhot) < sc->open_res_proportion || ubat == uhot) {
        sc->smoke_sta = OPEN_CIRCUIT;
    } else if ((ubat * 10 / uhot) > sc->short_res_proportion) {
        sc->smoke_sta = SHORT_CIRCUIT;
    } else {
        sc->hot_res = (uhot * sc->res_diff) / (ubat - uhot);
        printf(res_str, (sc->hot_res * 1000) >> 13);
    }
}

AT(.com_text.isr)
void timer_hot_dual_isr(void)//100us
{
    smoke_ctrl_t *sc = &smk_ctrl;

    if (timer_hot_mic_work()) {
        sc->timer_100us_cnt++;
        if (mic_start_or_not() != sc->mic_start) {
            bsp_adcgnd_gpio_init();
            ECIG_PWM_OFF_FUNC();
            if (sc->timer_100us_cnt < CAL_RES_TIME_CYCLE) {
                ECIG_VEN_ON();
                bsp_res_proc();
                caculate_hot_res();
                
            } else if (sc->timer_100us_cnt == CAL_RES_TIME_CYCLE) {
                ECIG_VEN_OFF();
                if (sc->hot_res) {
                    sc->mic_start = 1;
                    sc->timer_100us_cnt = 0;
                    sc->smoke_sta = SMOKING;
                }
            }
        } else {
            //第一个周期不开pwm，检验VBAT是否欠压
            if(sc->timer_100us_cnt == 1){
                ECIG_PWM_OFF_FUNC();
                e_cigs_vbat_proc();
                if (sc->AD_BAT_voltage_mv < 3300) {//欠压保护
                    sc->smoke_sta = LOW_POWER;
                    sc->power_on_flag = 0;
                } else {
                    sc->power_on_flag = 1;
                }

            //第2-4三个t打开pwm1，让vat稳定，再进行开路、短路检测
            } else if (sc->timer_100us_cnt < 5) {
                ECIG_PWM_ON_FUNC();
                bsp_hot_proc();
                sc->p_current = calculate_power(sc->AD_hot_voltage) + sc->p_prev;
                sc->p_prev = sc->p_current;

            //第6-100个t，正常加热流程为拉高pwm1，判断短路，计算功率
            } else {
                if(sc->power_on_flag){
                    bsp_hot_proc();
                    timer_hot_det_status();
                    sc->p_current = calculate_power(sc->AD_hot_voltage) + sc->p_prev;
                    sc->p_prev = sc->p_current;
                    if(sc->p_current >= (sc->final_power * 100) << 13){
                        ECIG_PWM_OFF_FUNC();
                        sc->power_on_flag = 0;
                    }
                } else { //不加热的时候采集vbat
                    e_cigs_vbat_proc();
                }

                //第100个t，清零
                if (sc->timer_100us_cnt == 100) {
                    sc->timer_100us_cnt = 0;
                    ECIG_PWM_OFF_FUNC();
                    sc->p_current = 0;
                    sc->p_prev = 0;
                }
            }
        }
    } else {
        timer_hot_free_handle();
    }

}
#else

AT(.com_text.isr)
void timer_hot_single_isr(void)//100us
{
    smoke_ctrl_t *sc = &smk_ctrl;
    if (timer_hot_mic_work()) {
        sc->timer_100us_cnt++;
        bsp_adcgnd_gpio_init();
        //第一个周期不开pwm，检验VBAT是否欠压
        if (sc->timer_100us_cnt == 1) {
            ECIG_PWM_OFF_FUNC();
            e_cigs_vbat_proc();
            if (sc->AD_BAT_voltage_mv < 3300) {//欠压保护
                sc->smoke_sta = LOW_POWER;
                sc->power_on_flag = 0;
            } else {
                sc->power_on_flag = 1;
                if (mic_start_or_not() != sc->mic_start) {
                    sc->mic_start = 1;
                    sc->hot_time_tick = tick_get();
                    sc->smoke_sta = SMOKING;
                }
            }

        //第2-4三个t打开pwm1，让vat稳定，再进行开路、短路检测
        } else if (sc->timer_100us_cnt < 5) {
            ECIG_PWM_ON_FUNC();
            bsp_hot_proc();
            sc->p_current = calculate_power(sc->AD_hot_voltage) + sc->p_prev;
            sc->p_prev = sc->p_current;

        //第6-100个t，正常加热流程为拉高pwm1，判断短路，计算功率
        } else {
            if(sc->power_on_flag){
                bsp_hot_proc();
                timer_hot_det_status();
                sc->p_current = calculate_power(sc->AD_hot_voltage) + sc->p_prev;
                sc->p_prev = sc->p_current;
                if(sc->p_current >= (sc->final_power * 100) << 13){
                    ECIG_PWM_OFF_FUNC();
                    sc->power_on_flag = 0;
                }
            } else { //不加热的时候采集vbat
                e_cigs_vbat_proc();
            }

            //第100个t，清零
            if (sc->timer_100us_cnt == 100) {
                sc->timer_100us_cnt = 0;
                ECIG_PWM_OFF_FUNC();
                sc->p_current = 0;
                sc->p_prev = 0;
            }
        }
    } else {
        timer_hot_free_handle();
    }

}

#endif //ECIG_RES_ADC_EN



AT(.com_text.timer0_isq)
void e_cigs_isr(void)
{
    if (get_timer_flag() != RESET) {
        clear_timer_flag();
    } else {
        return;
    }

#if ECIG_RES_ADC_EN
    timer_hot_dual_isr();
#else
    timer_hot_single_isr();
#endif
}

AT(.com_text.thread.driver)
void thread_driver_smoke_event_callback(uint8_t msg)
{

}

// saradc 通道初始化
void bsp_saradc_channel_init(void)
{
    saradc_channel_init_typedef saradc_channel_init_struct;
    uint32_t channel = ADC_HOT;
#if ECIG_RES_ADC_EN
    channel |= ADC2_RES;
#endif
    saradc_channel_init_struct.channel =  channel;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_0_CLK;
    saradc_channel_init_struct.interrupt_en = DISABLE;

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_kick_start(channel);
}

// mic io初始化
void bsp_mic_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = BSP_MIC_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(BSP_MIC_PORT, &gpio_init_structure);
}

// PWM io初始化
void bsp_pwm_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = PWM1_OUT_PIN;        //PWM1
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(PWM1_OUT_PORT, &gpio_init_structure);

#if ECIG_RES_ADC_EN
    gpio_init_structure.gpio_pin = PWM2_OUT_PIN;      //PWM2
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(PWM2_OUT_PORT, &gpio_init_structure);
#endif
}

// 定时器0初始化
void e_cigs_hot_timer0_init(void)
{
    printf("%s\n",__func__);
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate0_cmd(CLK_GATE0_TMR0, CLK_EN);

    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;       // 1MHz
    tmr_base_init_struct.prescale = 1;
    tmr_base_init_struct.period = 100 - 1;                      //100us翻1次
    tmr_base_init(TMR0, &tmr_base_init_struct);
    tmr_pic_config(TMR0, e_cigs_isr, 0, TMR_IT_UPDATE, ENABLE);

    tmr_cmd(TMR0, ENABLE);
}


void e_cigs_init(void)
{
    memset(&smk_ctrl,0,sizeof(smk_ctrl));
    bsp_pwm_gpio_init();
    bsp_mic_gpio_init();
    bsp_saradc_channel_init();
    e_cigs_vbat_proc();
    e_cigs_hot_timer0_init();

    smk_ctrl.power_on_flag          = 1;
    smk_ctrl.timer_100us_cnt        = 0;
    smk_ctrl.final_power            = 5;                        //目标功率
    smk_ctrl.max_hot_time           = 8;                        //最长加热时间
    smk_ctrl.short_res_proportion   = ECIG_RES_DIFF * 100 / ECIG_SHORT_RES_MIN + 10;      //(res_diff / res_min + 1) * 10
    smk_ctrl.open_res_proportion    = ECIG_RES_DIFF * 100 / ECIG_OPEN_RES_MAX + 10;
    smk_ctrl.short_res_prop         = ECIG_SHORT_RES_PROP;
    smk_ctrl.open_res_prop          = ECIG_OPEN_RES_PROP;
    smk_ctrl.res_diff               = ECIG_RES_DIFF * 8192;     //阻抗检测分压电阻值（Ω，8192倍）
#if !ECIG_RES_ADC_EN
    smk_ctrl.hot_res                = ECIG_RES_WIRE * 8192;     //无ADC2通路时电阻丝为固定阻值（Ω，8192倍）
#endif
}

#endif




















