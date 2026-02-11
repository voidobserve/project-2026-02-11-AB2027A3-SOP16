#include "bsp_ecigs.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "driver_hsuart.h"
#include "bsp_io_key.h"

#if (BSP_ECIGS_EN == ECIG_CONTROL_V2)

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

static smoke_ctrl_t smk_ctrl AT(.buf.smk_data);
static adc12_cb_t  adc12_cb_var  AT(.buf.adc12);

///HSUART DMA 中断服务函数
AT(.com_text.hsuart_adc12_driver.dma_rx_isr) FIQ
void adc12_hsuart_driver_dma_rx_isr(void)
{
    adc12_cb_t  *adc12=&adc12_cb_var;
    if (hsuart_get_flag(HSUART_IT_RX) != RESET) {
        uint32_t len = hsuart_get_fifo_counter();
	 	hsuart_receive_data_from_fifo(adc12->rx_buffer, len);
        //print_r(adc12->rx_buffer,len);
	 	adc12_huart_rx_handler(adc12->rx_buffer,len);
        hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
        hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(adc12->rx_buffer), 32);
    }

}


/*************************strong start***********************************/
void adc12_hsuart_driver_init(void)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

/************************************ GPIO Init ************************************/
    gpio_init_structure.gpio_pin = HSUART_ADC12_DRIVER_PIN_SEL;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_ADC12_DRIVER_PORT_SEL, &gpio_init_structure);

    /* NOTE: make sure printf_io isn't using PB9(VUSB) */
    gpio_func_mapping_clear(HSUART_ADC12_DRIVER_PORT_SEL, HSUART_ADC12_DRIVER_PIN_SEL);
    gpio_func_mapping_config(HSUART_ADC12_DRIVER_PORT_SEL, HSUART_ADC12_DRIVER_PIN_SEL, GPIO_CROSSBAR_IN_HURRX);
    gpio_func_mapping_config(HSUART_ADC12_DRIVER_PORT_SEL, HSUART_ADC12_DRIVER_PIN_SEL, GPIO_CROSSBAR_OUT_HURTX);

/*********************************** HSUART Init ***********************************/
    hsuart_init_struct.baud = HSUART_ADC12_DRIVER_BAUD;
    hsuart_init_struct.one_line_en = HSUT_ONELINE_EN;
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_2BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_24M_CLK;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_SINGLE;
    hsuart_init(&hsuart_init_struct);
    hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
    hsuart_rx_idle_config(HSUART_ADC12_DRIVER_OV_CNT, ENABLE);
    hsuart_pic_config(adc12_hsuart_driver_dma_rx_isr, 1, HSUART_IT_RX, ENABLE);
    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);
    hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(adc12_cb_var.rx_buffer), 32);

}







void adc12_kick_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = ADC12_KICK_PIN ;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(ADC12_KICK_PORT, &gpio_init_structure);
    gpio_toggle_bits(ADC12_KICK_PORT, ADC12_KICK_PIN);

}


/*************************strong end***********************************/


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


void bsp_check_res_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init_structure.gpio_pin = BSP_CHECK_RES_PIN  ;
    gpio_init(BSP_CHECK_RES_PORT, &gpio_init_structure);
    gpio_reset_bits(BSP_CHECK_RES_PORT, BSP_CHECK_RES_PIN);

}

void bsp_det_gpio_init(void)
{

    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init_structure.gpio_pin = BSP_DET1_PIN;
    gpio_init(BSP_DET1_PORT, &gpio_init_structure);
    gpio_init_structure.gpio_pin = BSP_DET2_PIN;
    gpio_init(BSP_DET2_PORT, &gpio_init_structure);

}


void bsp_pwm_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init_structure.gpio_pin = PWM1_OUT_PIN  ;
    gpio_init(PWM1_OUT_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = PWM2_OUT_PIN  ;
    gpio_init(PWM1_OUT_PORT, &gpio_init_structure);

    gpio_reset_bits(PWM1_OUT_PORT, PWM1_OUT_PIN);
    gpio_reset_bits(PWM2_OUT_PORT, PWM2_OUT_PIN);

}


void bsp_ccs_gpio_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;


    gpio_init_structure.gpio_pin = BSP_CCS_PIN  ;
    gpio_init(BSP_CCS_PORT, &gpio_init_structure);
    gpio_reset_bits(BSP_CCS_PORT, BSP_CCS_PIN);
}

AT(.com_text.ccs_gpio_set)
void bsp_ccs_gpio_set(void)
{
   gpio_set_bits(BSP_CCS_PORT, BSP_CCS_PIN);
}

AT(.com_text.ccs_gpio_reset)
void bsp_ccs_gpio_reset(void)
{
   gpio_reset_bits(BSP_CCS_PORT, BSP_CCS_PIN);
}

AT(.com_text.pwm1_on)
void bsp_pwm1_on(void)
{
   gpio_set_bits(PWM1_OUT_PORT, PWM1_OUT_PIN);
}

AT(.com_text.pwm1_off)
void bsp_pwm1_off(void)
{
   gpio_reset_bits(PWM1_OUT_PORT, PWM1_OUT_PIN);
}

AT(.com_text.pwm2_on)
void bsp_pwm2_on(void)
{
   gpio_set_bits(PWM2_OUT_PORT, PWM2_OUT_PIN);
}

AT(.com_text.pwm2_off)
void bsp_pwm2_off(void)
{
   gpio_reset_bits(PWM2_OUT_PORT, PWM2_OUT_PIN);
}

AT(.com_text.e_cig)
uint8_t mic_start_or_not(void)
{
   return gpio_read_bit(BSP_MIC_PORT,BSP_MIC_PIN);
}


AT(.com_text.res1.short_circuit)
uint8_t res1_short_circuit_check(void)
{

  return gpio_read_bit(BSP_DET1_PORT,BSP_DET1_PIN);
}

AT(.com_text.res2.short_circuit)
uint8_t res2_short_circuit_check(void)
{
   return gpio_read_bit(BSP_DET2_PORT,BSP_DET2_PIN);
}


AT(.com_text.res1.det)
uint16_t res1_status_det(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    static uint8_t res1_insert_status=0;
    static uint8_t res1_pull_status=0;//上一个状态
    static uint32_t insert_debounce_cnt=0;
    static uint32_t pull_debounce_cnt=0;
    uint16_t res_sta=0;
    smoke_ctrl_t *sc_res = &smk_ctrl;
    if(gpio_read_bit(BSP_DET1_PORT,BSP_DET1_PIN)==RESET){

         if(res1_insert_status==0){
            insert_debounce_cnt=insert_debounce_cnt+1;
            if(insert_debounce_cnt==10*500)//500ms
            {
                res1_insert_status=1;
                res1_pull_status=0;
                insert_debounce_cnt=0;
                res_sta=THREAD_DRIVER_MSG_RES1_INSERT_STA;
                sc_res->res1_det_flag=1;
                return  res_sta;
            }
         }
    }
    else{
        if(res1_pull_status==0){
            pull_debounce_cnt=pull_debounce_cnt+1;
            if(pull_debounce_cnt==10*500)//500ms
            {
                res1_insert_status=0;
                res1_pull_status=1;
                pull_debounce_cnt=0;
                res_sta=THREAD_DRIVER_MSG_RES1_PULL_STA;
                sc_res->res1_det_flag=0;
                sc->smoke_sta = OPEN_CIRCUIT;
                return  res_sta;
            }
         }
    }
    return 0;
}


AT(.com_text.res2.det)
uint16_t res2_status_det(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    static uint8_t res2_insert_status=0;
    static uint8_t res2_pull_status=0;//上一个状态
    static uint32_t insert_debounce_cnt=0;
    static uint32_t pull_debounce_cnt=0;
    uint16_t res_sta=0;
    smoke_ctrl_t *sc_res = &smk_ctrl;

    if(gpio_read_bit(BSP_DET2_PORT,BSP_DET2_PIN)==RESET){

         if(res2_insert_status==0){
            insert_debounce_cnt=insert_debounce_cnt+1;
            if(insert_debounce_cnt==10*500)//500ms
            {
                res2_insert_status=1;
                res2_pull_status=0;
                insert_debounce_cnt=0;
                res_sta=THREAD_DRIVER_MSG_RES2_INSERT_STA;
                sc_res->res2_det_flag=1;
                 return  res_sta;
            }
         }
    }
    else{
        if(res2_pull_status==0){
            pull_debounce_cnt=pull_debounce_cnt+1;
            if(pull_debounce_cnt==10*500)//500ms
            {
                res2_insert_status=0;
                res2_pull_status=1;
                pull_debounce_cnt=0;
                sc_res->res2_det_flag=0;
                sc->smoke_sta = OPEN_CIRCUIT;
                res_sta=THREAD_DRIVER_MSG_RES2_PULL_STA;
                return  res_sta;
            }
         }
    }


    return 0;


}



AT(.com_text.res.value)
uint32_t caculate_res_value(uint16_t adc_value1,uint16_t adc_value2)
{

    uint32_t tmp_value1=0;
    uint32_t tmp_value2=0;

    if(adc_value1==0||adc_value2==0){
       return 0;
    }

    if(adc_value1>=adc_value2){
        tmp_value1=adc_value1*1000000;
        tmp_value2=adc_value2;
    }
    else{
        tmp_value1=adc_value2*1000000;
        tmp_value2=adc_value1;
    }

   uint32_t  res_val=(tmp_value1/tmp_value2 -1000000)/4700;
   return  res_val;
}

AT(.com_text.ecig.power)
u32 calculate_power(u32 hot_voltage, u16 hot_res)
{
    if (hot_res == 0) {
        return 0;
    }
    return (hot_voltage * hot_voltage / hot_res);
}

AT(.com_text.res1.val)
uint32_t get_res1_val(void )
{

       uint32_t tmp_val1=0;
       uint32_t tmp_val2=0;
       smoke_ctrl_t *sc_res = &smk_ctrl;
       bsp_ccs_gpio_set();//打开计算加热电阻电源

       //printf("sc_res->heat1_chnl|sc_res->reg1_chnl = %02x\n",sc_res->heat1_chnl|sc_res->reg1_chnl);
       adc12_kick_channel_set(sc_res->heat1_chnl|sc_res->reg1_chnl,sc_res->heat1_chnl|sc_res->reg1_chnl);//设置ADC采样通道
       delay_us(50);

       //printf("================get_res1_val===========================\n");
       for(uint8_t i=0;i<10;i++){
           adc12_kick();//开启adc12 第一次采样
           if(adc12_kick_wait_rsp(10000)){
              sc_res->adc12_val1[i]= adc12_get_value(sc_res->heat1_chnl);//获取上次采样值
              sc_res->adc12_val2[i]=adc12_get_value(sc_res->reg1_chnl);//获取上次采样值
           }
           else {
               printf("[res1]adc12_kick ERROR\n");
           }
       }
       bsp_ccs_gpio_reset();
       for(uint16_t i=0;i<10;i++){
            tmp_val1=tmp_val1+ sc_res->adc12_val1[i];
            tmp_val2=tmp_val2+ sc_res->adc12_val2[i];
       }
       sc_res->res1_val=  caculate_res_value(tmp_val1/10,tmp_val2/10);
       printf("sc_res->res1_val= %d\n",sc_res->res1_val);

       return sc_res->res1_val;
}



AT(.com_text.res2.val)
uint32_t get_res2_val(void )
{

       uint32_t tmp_val1=0;
       uint32_t tmp_val2=0;
       smoke_ctrl_t *sc_res = &smk_ctrl;

       bsp_ccs_gpio_set();//打开计算加热电阻电源
       //printf("sc_res->heat2_chnl|sc_res->reg2_chnl = %02x\n",sc_res->heat2_chnl|sc_res->reg2_chnl);
       adc12_kick_channel_set(sc_res->heat2_chnl|sc_res->reg2_chnl,sc_res->heat2_chnl|sc_res->reg2_chnl);//设置ADC采样通道
      // printf("================get_res2_val===========================\n");
       delay_us(50);
       for(uint8_t i=0;i<10;i++){
           adc12_kick();//开启adc12 第一次采样
           if(adc12_kick_wait_rsp(10000)){
              sc_res->adc12_val3[i]= adc12_get_value(sc_res->heat2_chnl);//获取上次采样值
              sc_res->adc12_val4[i]= adc12_get_value(sc_res->reg2_chnl);//获取上次采样值

              //printf("sc_res->adc12_val3[%d] = %d\n",i,sc_res->adc12_val3[i]);
              //printf("sc_res->adc12_val4[%d] = %d\n",i,sc_res->adc12_val4[i]);
           }
           else {
               printf("[res1]adc12_kick ERROR\n");
           }
       }
       bsp_ccs_gpio_reset();
       for(uint16_t i=0;i<10;i++){
            tmp_val1=tmp_val1+ sc_res->adc12_val3[i];
            tmp_val2=tmp_val2+ sc_res->adc12_val4[i];
       }
       sc_res->res2_val=  caculate_res_value(tmp_val1/10,tmp_val2/10);
       printf("sc_res->res2_val= %d\n",sc_res->res2_val);

       return sc_res->res2_val;
}


AT(.com_text.res1.handler)
void res1_heating_handler(void)
{
    smoke_ctrl_t *sc = &smk_ctrl;       //sc->
     if(sc->heat_chn_set_flag)
     {
          if(sc->res1_det_flag==1)
          {
              if(sc->timer_100us_cnt==102){
                   sc->timer_100us_cnt=0;
              }

              sc->timer_100us_cnt=sc->timer_100us_cnt+1;

              if(sc->timer_100us_cnt==1){
                  bsp_pwm2_off();
                  bsp_pwm1_off();
                  adc12_kick();
                  sc->power_flag=0;
                  sc->current_power=0;
              }

              if(sc->power_flag){
                 return ;
              }

              if(sc->timer_100us_cnt==2){

                  sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                  if(sc->vbat_val<3600){//空载电池低压保护
                      bsp_pwm1_off();
                      bsp_pwm2_off();
                      sc->timer_100us_cnt=0;
                      if(sc->vbat_low_flag==0){
                        sc->vbat_low_flag=1;
                        sc->smoke_sta = LOW_POWER;
                        thread_driver_msg_post(THREAD_DRIVER_MSG_VBAT_LOW_STATUS);
                      }
                      return ;
                  }
                  bsp_pwm1_on();
                  adc12_kick();
              }

              if( sc->timer_100us_cnt>=3&&sc->timer_100us_cnt<102){

                    sc->res1_heat_vol=  adc12_get_value_mv(sc->heat1_chnl);
                    sc->res2_heat_vol=  adc12_get_value_mv(sc->heat2_chnl);
                    sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                    sc->current_power=sc->current_power+calculate_power(sc->res1_heat_vol,sc->res1_val);
                    if(sc->current_power>=sc->target_power){

                       bsp_pwm1_off();
                       bsp_pwm2_off();
                       thread_driver_msg_post(THREAD_DRIVER_MSG_REACH_TARGET_POWER);
                       sc->power_flag=1;
                       return ;
                    }

                    if(sc->timer_100us_cnt==101)
                    {
                       thread_driver_msg_post(THREAD_DRIVER_MSG_UNABLE_REACH_TARGET_POWER);
                    }

                      bsp_pwm1_on();
                      adc12_kick();

                      if(res1_short_circuit_check()==RESET){//短路
                        bsp_pwm1_off();
                        bsp_pwm2_off();
                        sc->short_circuit_flag=1;
                        sc->smoke_sta = SHORT_CIRCUIT;
                          return ;
                      }
              }
          }
     }

}

AT(.com_text.res2.handler)
void res2_heating_handler(void)
{
     smoke_ctrl_t *sc = &smk_ctrl;
     if(sc->heat_chn_set_flag)
     {
          if(sc->res2_det_flag==1)
          {
              if(sc->timer_100us_cnt==102){
                   sc->timer_100us_cnt=0;
              }

              sc->timer_100us_cnt=sc->timer_100us_cnt+1;

              if(sc->timer_100us_cnt==1){
                  bsp_pwm2_off();
                  bsp_pwm1_off();
                  adc12_kick();
                  sc->power_flag=0;
                  sc->current_power=0;
              }

              if(sc->power_flag){
                 return ;
              }

              if(sc->timer_100us_cnt==2){

                  sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                  if(sc->vbat_val<3600){//空载电池低压保护
                      bsp_pwm1_off();
                      bsp_pwm2_off();
                      sc->timer_100us_cnt=0;
                      if(sc->vbat_low_flag==0){
                        sc->vbat_low_flag=1;
                        sc->smoke_sta = LOW_POWER;
                        thread_driver_msg_post(THREAD_DRIVER_MSG_VBAT_LOW_STATUS);
                      }
                      return ;
                  }
                  bsp_pwm2_on();
                  adc12_kick();

              }

              if(sc->timer_100us_cnt>=3&&sc->timer_100us_cnt<102){

                    sc->res1_heat_vol=  adc12_get_value_mv(sc->heat1_chnl);
                    sc->res2_heat_vol=  adc12_get_value_mv(sc->heat2_chnl);
                    sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                    sc->current_power=sc->current_power+calculate_power(sc->res2_heat_vol,sc->res2_val);

                    if(sc->current_power>=sc->target_power){

                       bsp_pwm1_off();
                       bsp_pwm2_off();
                       thread_driver_msg_post(THREAD_DRIVER_MSG_REACH_TARGET_POWER);
                       sc->power_flag=1;
                       return ;

                    }

                    if(sc->timer_100us_cnt==101)
                    {
                       thread_driver_msg_post(THREAD_DRIVER_MSG_UNABLE_REACH_TARGET_POWER);
                    }
                    bsp_pwm2_on();
                    adc12_kick();
                    if(res2_short_circuit_check()==RESET){//短路
                        bsp_pwm1_off();
                        bsp_pwm2_off();
                        sc->short_circuit_flag=1;
                        sc->smoke_sta = SHORT_CIRCUIT;
                        return ;
                    }
              }
          }
     }
}



AT(.com_text.res1_res2.handler)
void res1_res2_heating_handler(void)
{
     smoke_ctrl_t *sc = &smk_ctrl;
     if(sc->heat_chn_set_flag)
     {
          if(sc->res1_det_flag==1&&sc->res2_det_flag==1)
          {
              if(sc->timer_100us_cnt==102){
                   sc->timer_100us_cnt=0;
              }

              sc->timer_100us_cnt=sc->timer_100us_cnt+1;

              if(sc->timer_100us_cnt==1){
                  bsp_pwm2_off();
                  bsp_pwm1_off();
                  adc12_kick();
                  sc->power_flag=0;
                  sc->current_power=0;
              }

              if(sc->power_flag){
                 return ;
              }

              if(sc->timer_100us_cnt==2){

                  sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                  if(sc->vbat_val<3600){//空载电池低压保护
                      bsp_pwm1_off();
                      bsp_pwm2_off();
                      sc->timer_100us_cnt=0;
                      if(sc->vbat_low_flag==0){
                        sc->vbat_low_flag=1;
                        sc->smoke_sta = LOW_POWER;
                        thread_driver_msg_post(THREAD_DRIVER_MSG_VBAT_LOW_STATUS);
                      }
                      return ;
                  }
                  bsp_pwm1_on();
                  bsp_pwm2_on();
                  adc12_kick();

              }

              if(sc->timer_100us_cnt>=3&&sc->timer_100us_cnt<102){

                    sc->res1_heat_vol=  adc12_get_value_mv(sc->heat1_chnl);
                    sc->res2_heat_vol=  adc12_get_value_mv(sc->heat2_chnl);
                    sc->vbat_val= adc12_get_value_mv(sc->vbat_chnl);
                    sc->current_power=sc->current_power+calculate_power(sc->res1_heat_vol,sc->res1_val);
                    sc->current_power=sc->current_power+calculate_power(sc->res2_heat_vol,sc->res2_val);
                    if(sc->current_power>=sc->target_power){

                       bsp_pwm1_off();
                       bsp_pwm2_off();
                       thread_driver_msg_post(THREAD_DRIVER_MSG_REACH_TARGET_POWER);
                       sc->power_flag=1;
                       return ;

                    }

                    if(sc->timer_100us_cnt==101)
                    {
                       thread_driver_msg_post(THREAD_DRIVER_MSG_UNABLE_REACH_TARGET_POWER);
                    }

                    bsp_pwm1_on();
                    bsp_pwm2_on();
                    adc12_kick();

                    if(res1_short_circuit_check()==RESET||res2_short_circuit_check()==RESET){//短路
                        bsp_pwm1_off();
                        bsp_pwm2_off();
                        sc->short_circuit_flag=1;
                        sc->smoke_sta = SHORT_CIRCUIT;
                      return ;
                    }
              }
          }
     }
}


AT(.com_text.timer0_isq)
void e_cigs_isr(void)
{

    smoke_ctrl_t *sc = &smk_ctrl;

    if (tmr_get_flag(TMR0, TMR_IT_UPDATE) != RESET){

          tmr_clear_flag(TMR0, TMR_IT_UPDATE);

          if(mic_start_or_not()==SET)//吸烟
          {
              sc->mic_debounce_time= sc->mic_debounce_time+1;
              //20ms 消除抖动
              if(sc->mic_debounce_time==10*20) {
                    //计算加热电阻
                 thread_driver_msg_post(THREAD_DRIVER_MSG_RES_CAL_START);
                 sc->heat_switch=!sc->heat_switch;
              }

              if (sc->res_cal_flag){
                  sc->res_cal_flag=0;
                  thread_driver_msg_post(THREAD_DRIVER_MSG_SET_HEAT_SAMPLE_CHANNEL);
              }

              if(sc->heat_chn_set_flag){
                  if(sc->short_circuit_flag==0){
                        sc->smoke_sta = SMOKING;
                        if(sc->heat_mode==MODE_RES1_HEATING){
                            res1_heating_handler();//加热电阻1，加热输出
                        }
                        else if (sc->heat_mode==MODE_RES2_HEATING){
                            res2_heating_handler();//加热电阻2，加热输出
                        }
                        else if (sc->heat_mode==MODE_RES1_RES2_SWITCH_HEATING){
                            if(sc->heat_switch){ //双发交替输出
                                res1_heating_handler();
                            }
                            else{
                                res2_heating_handler();
                            }
                        }
                        else if (sc->heat_mode==MODE_RES1_RES2_ALL_HEATING){
                            res1_res2_heating_handler();//双发全输出
                        }
                  }
              }
          }
          else{
                sc->smoke_sta = IDLE;
                sc->timer_100us_cnt=0;
                sc->mic_debounce_time=0;
                sc->res_cal_flag=0;
                sc->short_circuit_flag=0;
                sc->heat_chn_set_flag=0;
                sc->vbat_low_flag=0;
                bsp_pwm1_off();
                bsp_pwm2_off();

               uint16_t res_status=0;
               res_status=  res1_status_det();
               if(res_status) {
                   thread_driver_msg_post(res_status);
               }
               res_status=  res2_status_det();
               if(res_status) {
                   thread_driver_msg_post(res_status);
               }
          }
       }

}


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



AT(.com_text.thread.driver)
void thread_driver_smoke_event_callback(uint8_t msg)
{
    smoke_ctrl_t *sc = &smk_ctrl;
    switch (msg) {
        case THREAD_DRIVER_MSG_RES1_PULL_STA:
          TRACE("-->res1 pulled status\n");
         break;
        case THREAD_DRIVER_MSG_RES1_INSERT_STA:
          TRACE("-->res1 inerted status\n");
         break;
        case THREAD_DRIVER_MSG_RES2_PULL_STA:
           TRACE("-->res2 pulled status\n");
         break;
        case  THREAD_DRIVER_MSG_RES2_INSERT_STA:
           TRACE("-->res2 inerted status\n");
        break;
        case THREAD_DRIVER_MSG_RES_CAL_START:

           if(sc->heat_mode==MODE_RES1_HEATING){
               if(sc->res1_det_flag){
                    get_res1_val();
                    sc->res_cal_flag=1;
               }
               else{
                    sc->res_cal_flag=0;
               }
           }
           else if(sc->heat_mode==MODE_RES2_HEATING){
               if(sc->res2_det_flag){
                    get_res2_val();
                    sc->res_cal_flag=1;
               }
               else{
                    sc->res_cal_flag=0;
               }
           }
           else if(sc->heat_mode==MODE_RES1_RES2_SWITCH_HEATING||sc->heat_mode==MODE_RES1_RES2_ALL_HEATING){
               if(sc->res2_det_flag&&sc->res1_det_flag){
                    get_res1_val();
                    get_res2_val();
                    sc->res_cal_flag=1;
               }
               else{
                    sc->res_cal_flag=0;
               }
           }

           break;
        case THREAD_DRIVER_MSG_SET_HEAT_SAMPLE_CHANNEL:
            TRACE("sc->mic_debounce_time=%d\n",sc->mic_debounce_time);
            bool ret=adc12_kick_channel_set(sc->heat1_chnl|sc->heat2_chnl|sc->vbat_chnl,sc->heat1_chnl|sc->heat2_chnl|sc->vbat_chnl);
            if(ret==true){
               sc->heat_chn_set_flag=1;
               TRACE("adc12_kick_channel_set success!\n");
            }
            else{
               sc->heat_chn_set_flag=0;
               TRACE("adc12_kick_channel_set fail!\n");
            }
            break;

        case   THREAD_DRIVER_MSG_REACH_TARGET_POWER:{
            TRACE("sc->res1_val=%d\n",sc->res1_val);
            TRACE("sc->res2_val=%d\n",sc->res2_val);
            TRACE("sc->res1_heat_vol=%d\n",sc->res1_heat_vol);
            TRACE("sc->res2_heat_vol=%d\n",sc->res2_heat_vol);
            TRACE("sc->vbat_val=%d\n",sc->vbat_val);
            TRACE("sc->current_power=%d\n",sc->current_power);
            break;
        }
        case THREAD_DRIVER_MSG_UNABLE_REACH_TARGET_POWER:{
            TRACE("maximum output power:\n");
            TRACE("sc->current_power=%d\n",sc->current_power);
            break;
        }
        case THREAD_DRIVER_MSG_VBAT_LOW_STATUS:{
            TRACE("vbat low:\n");
            TRACE("sc->vbat_val=%d\n",sc->vbat_val);
            break;
        }

        default:
         break;
    }
}

uint8_t get_smoke_sta(void)
{
    return smk_ctrl.smoke_sta;
}

void e_cigs_init(void)
{


    printf("%s\n", __func__);
    smoke_ctrl_t *sc = &smk_ctrl;
    memset(&smk_ctrl, 0, sizeof(smoke_ctrl_t));

    sc->heat1_chnl=BSP_ADC1_CHANEL;
    sc->reg1_chnl=BSP_ADC3_CHANEL;
    sc->heat2_chnl=BSP_ADC2_CHANEL;
    sc->reg2_chnl=BSP_ADC4_CHANEL;
    sc->vbat_chnl=VBAT_SAMPLE_CHANEL;
    sc->heat_mode=MODE_RES1_RES2_SWITCH_HEATING;
    sc->target_power=5*1000*100;//5w 功率设置

    memset(&adc12_cb_var,0,sizeof(adc12_cb_var));
    adc12_hsuart_driver_init();
    adc12_kick_gpio_init();
    adc12_kick_init(&adc12_cb_var);
    bsp_pwm_gpio_init();

     bsp_check_res_gpio_init();
     bsp_det_gpio_init();
     bsp_ccs_gpio_init();
     bsp_mic_gpio_init();
     e_cigs_hot_timer0_init();

     adc12_kick_channel_set(sc->vbat_chnl,sc->vbat_chnl);//设置ADC采样通道
     uint16_t vabt_val=adc12_get_value_mv(sc->vbat_chnl);
     printf("vabt_val = %d\n",vabt_val);
     adc12_set_ch_de_out(ADC12_CHANEL_P12,1);//LCD背光
}


void  e_cigs_sleep_config(void)
{
   smoke_ctrl_t *sc = &smk_ctrl;
   adc12_enter_lowpwr(sc->heat1_chnl|sc->reg1_chnl|sc->heat2_chnl|sc->reg2_chnl|sc->vbat_chnl);
}

void e_cigs_wakeup_config(void)
{
     memset(&adc12_cb_var,0,sizeof(adc12_cb_var));
     adc12_kick_init(&adc12_cb_var);
     bsp_pwm_gpio_init();
     bsp_mic_gpio_init();
     bsp_check_res_gpio_init();
     bsp_det_gpio_init();
     bsp_ccs_gpio_init();
     e_cigs_hot_timer0_init();

}

#endif // BSP_ECIGS_EN




