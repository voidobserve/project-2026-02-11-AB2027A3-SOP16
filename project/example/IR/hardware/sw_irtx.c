
#include "driver_adv_timer.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "sw_irtx.h"



#define SW_IRTX_IO_PORT        GPIOA_REG
#define SW_IRTX_IO_PIN         GPIO_PIN_4

#define  TIM_PEROID_VALUE     (630 - 1)
#define  TIM_PRESCALER_VALUE  (1 - 1)



static uint16_t set_pluse_value1 =(TIM_PEROID_VALUE/2);
static uint16_t sw_ir_tx_buf[256];
static uint16_t sw_ir_tx_len=0;
static uint8_t sw_ir_repeat_code=0;
static uint8_t sw_ir_tx_end=0;

AT(.com_text.timer_isq)
void sw_ir_tx_data_timer_isq(void)
{
    static uint16_t send_cnt=0;
    static uint16_t data_index=0;
    if (tmr_get_flag(TMR1, TMR_IT_UPDATE) != RESET){
        tmr_clear_flag(TMR1, TMR_IT_UPDATE);
        send_cnt=send_cnt+1;
        if(send_cnt>1&&sw_ir_tx_len>0){

              uint16_t ir_time=sw_ir_tx_buf[data_index]&(~0x8000);
              tmr_cmd(TMR1,DISABLE);
              tmr_set_period(TMR1,ir_time);
              tmr_cmd(TMR1,ENABLE);
              if(send_cnt==2){
                   tim_ctrlpwmoutputs(TIM1,ENABLE);
              }

              if((sw_ir_tx_buf[data_index]&0x8000)){
                     tim_cmd(TIM1, ENABLE);
                     tim_setcompare1(TIM1,set_pluse_value1 );
              }
              else {
                    tim_cmd(TIM1, DISABLE);
                    tim_setcompare1(TIM1,0 );
                    tim_cmd(TIM1, ENABLE);
              }

              data_index=data_index+1;
              if(data_index>=sw_ir_tx_len){

                    if (sw_ir_repeat_code){
                        data_index =sw_ir_tx_len-8;
                        send_cnt= sw_ir_tx_len;
                    }
                    else{
                        data_index=0;
                        send_cnt=0;
                        sw_ir_tx_len=0;
                        tim_setcompare1(TIM1,0 );
                        tim_cmd(TIM1, DISABLE);
                        tim_ctrlpwmoutputs(TIM1,DISABLE);
                        tmr_cmd(TMR1,DISABLE);
                        tim_cmd(TIM1, DISABLE);
                        sw_ir_tx_end=0;
                    }

              }
        }

        if(sw_ir_tx_len==0){
            data_index=0;
            send_cnt=0;
            sw_ir_tx_end=0;
            tmr_cmd(TMR1,DISABLE);
            tim_cmd(TIM1, DISABLE);
        }
    }
}



void sw_ir_tx_data(uint16_t*irdata,uint16_t irlen)
{
      if(irlen>sizeof(sw_ir_tx_buf)/2){
         return ;
      }

      if(sw_ir_tx_end){
          return;
      }
      sw_ir_tx_len=irlen;
      sw_ir_tx_end=1;
      memcpy(sw_ir_tx_buf,irdata,irlen*sizeof(sw_ir_tx_buf[0]));
      tmr_cmd(TMR1, ENABLE);
}


void sw_ir_repeat_code_enable(void)
{
    sw_ir_repeat_code=1;
}

void sw_ir_repeat_code_disable(void)
{
    sw_ir_repeat_code=0;
}



void nec_encode_format_send_data(uint8_t *buf,uint8_t len,uint8_t repeat_status)
{
    uint16_t ir_data[128]={0};
    ir_data[0]=(0x8000|9000);//起始位 有载波
    ir_data[1]=4500;
    uint16_t *p_data=&ir_data[2];
    uint16_t data_index=0;
    for(uint8_t i=0;i<len;i++){
        for(uint8_t j=0;j<8;j++){
            if((buf[i]>>j)&0x01){
                p_data[data_index+0]=(0x8000|562);
                p_data[data_index+1]=(0x0000|1687);
            }
            else{
                p_data[data_index+0]=(0x8000|562);
                p_data[data_index+1]=(0x0000|562);
            }
            data_index=data_index+2;
        }
    }
     p_data[data_index+0]=(0x8000|562);
     p_data[data_index+1]=(0x0000|562);

     //重复码
     p_data[data_index+2]=30000;
     p_data[data_index+3]=10000;
     p_data[data_index+4]=(0x8000|9000);
     p_data[data_index+5]=2250;
     p_data[data_index+6]=(0x8000|562);
     p_data[data_index+7]=(562);


     if(repeat_status){
         p_data[data_index+8]=30000;
         p_data[data_index+9]=30000;
         p_data[data_index+10]=30000;
         p_data[data_index+11]=6000;
         p_data[data_index+12]=(0x8000|9000);
         p_data[data_index+13]=2250;
         p_data[data_index+14]=(0x8000|562);
         p_data[data_index+15]=(562);
         sw_ir_repeat_code_enable();
        sw_ir_tx_data(ir_data,data_index+18);
     }
     else {
        sw_ir_repeat_code_disable();
        sw_ir_tx_data(ir_data,data_index+10);
     }
}


void sw_ir_tx_timer_init(void)
{
    tmr_base_init_typedef tmr_base_init_struct={0};
    clk_gate0_cmd(CLK_GATE0_TMR1, CLK_EN);
    tmr_base_init_struct.clock_source = TMR_COUNTER_RISING;
    tmr_base_init_struct.counter_source = TMR_INC_SOURCE;
    tmr_base_init_struct.prescale = 1-1;
    tmr_base_init_struct.period = 100- 1;                      // us
    tmr_base_init(TMR1, &tmr_base_init_struct);
    tmr_pic_config(TMR1, sw_ir_tx_data_timer_isq, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR1, DISABLE);
}

 void sw_ir_tx_carrier_init(void)
{
    CLKGAT0 |= BIT(8);
    gpio_init_typedef gpio_init_structure={0};
    gpio_init_structure.gpio_pin = SW_IRTX_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_24MA;
    gpio_init(SW_IRTX_IO_PORT, &gpio_init_structure);
    gpio_func_mapping_config(SW_IRTX_IO_PORT, SW_IRTX_IO_PIN, GPIO_CROSSBAR_OUT_TOC1);



    tim_timebaseinittypedef adv_tim_timebasestructure={0};
    tim_ocinittypedef adv_tim_ocinitstructure;

    tim_timebasestructinit(&adv_tim_timebasestructure);

    adv_tim_timebasestructure.tim_clockdivision =    TIM_CKD_DIV1;
    adv_tim_timebasestructure.tim_countermode   =    TIM_COUNTERMODE_UP;
    adv_tim_timebasestructure.tim_period        =    TIM_PEROID_VALUE;
    adv_tim_timebasestructure.tim_prescaler     =    TIM_PRESCALER_VALUE;
    adv_tim_timebasestructure.tim_repetitioncounter = 0;

    tim_timebaseinit(TIM1, &adv_tim_timebasestructure);

    tim_ocstructinit(&adv_tim_ocinitstructure);

    adv_tim_ocinitstructure.tim_ocmode       =  TIM_OCMODE_PWM1;
    adv_tim_ocinitstructure.tim_outputstate  =  TIM_OUTPUTSTATE_ENABLE;
    adv_tim_ocinitstructure.tim_outputnstate =  TIM_OUTPUTNSTATE_DISABLE;
    adv_tim_ocinitstructure.tim_pulse        =  set_pluse_value1;
    adv_tim_ocinitstructure.tim_ocpolarity   =  TIM_OCPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocnpolarity  =  TIM_OCNPOLARITY_HIGH;
    adv_tim_ocinitstructure.tim_ocidlestate  =  TIM_OCNIDLESTATE_RESET;
    adv_tim_ocinitstructure.tim_ocnidlestate =  TIM_OCNIDLESTATE_RESET;

    tim_oc1init(TIM1, &adv_tim_ocinitstructure);

    tim_oc1preloadconfig(TIM1, TIM_OCPRELOAD_ENABLE);
    tim_ctrlpwmoutputs(TIM1,DISABLE);
    tim_cmd(TIM1, DISABLE);

}


void sw_ir_tx_data_init(void)
{
    printf("%s\n", __func__);
    sw_ir_tx_timer_init();
    sw_ir_tx_carrier_init();
}



void sw_ir_tx_data_example(void)
{
    static uint32_t irtx_tick = 0;

    uint8_t ir_data[]={0x11,0x12,0x13,0x14};
    if (tick_check_expire(irtx_tick, 500)){
        printf("%s\n", __func__);
        irtx_tick = tick_get();
        nec_encode_format_send_data(ir_data,sizeof(ir_data),1);
    }
}

















