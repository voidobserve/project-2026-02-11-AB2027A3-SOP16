#include "include.h"
#include "bsp_adc12.h"
#include "bsp_ecigs.h"
#include "driver_hsuart.h"

#if (BSP_ECIGS_EN == ECIG_CONTROL_V2)


#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif





static adc12_cb_t  *adc12_cb=0 ;


AT(.com_text.adc12.hsuart_baud_config)
static void adc12_hsuart_baud_config(uint32_t baud)
{
    u32 baud_reg, sys_clk_val;
    sys_clk_val = 24000000;
    baud_reg = (sys_clk_val / baud) - 1;
    HSUART->baud = (uint32_t)((baud_reg << 16) | baud_reg);
}

AT(.com_text.adc12.send_buf)
static void adc12_uart_send_buf(u8 *buf, u16 len)
{
      adc12_cb_t  *adc12=adc12_cb;
      adc12->sta= 0x00;
      hsuart_dma_start(HSUT_TRANSMIT, (uint32_t)buf, len);
      while (hsuart_get_flag(HSUART_IT_TX) == RESET);
}


AT(.com_text.adc12_kick_toggle)
static void adc12_kick_gpio_toggle(void)
{
   gpio_toggle_bits(ADC12_KICK_PORT, ADC12_KICK_PIN);
}

AT(.com_text.adc12_kick_deinit)
static void adc12_kick_gpio_deinit(void)
{
   gpio_deinit(ADC12_KICK_PORT, ADC12_KICK_PIN);
}


AT(.com_text.adc12)
void master_uart_baud_set(u32 baud_rate)
{
    PICEN &= ~BIT(IRQn_HSUART);
    adc12_hsuart_baud_config(baud_rate);
    PICEN |= BIT(IRQn_HSUART);
}

AT(.com_text.adc12.checksum_calc)
u8 adc12_checksum_calc(u8 *buf, u8 len)
{
    u8 checksum = 0;
    while (len--) {
        checksum += *buf++;
    }
    return checksum;
}


AT(.com_text.adc12.get_value)
uint16_t  adc12_get_value(uint8_t chn)
{
        uint16_t tmp_val=0;
        if(adc12_cb->sta==ADC12_STA_FINISH){
        if(chn&BIT(0)){
            tmp_val=adc12_cb->adc_val[0];
            adc12_cb->adc_val[0]=0;
            return tmp_val ;
        }else if(chn&BIT(1)){
           tmp_val=adc12_cb->adc_val[1];
            adc12_cb->adc_val[1]=0;
            return tmp_val ;
        }else if(chn&BIT(2)){
           tmp_val=adc12_cb->adc_val[2];
            adc12_cb->adc_val[2]=0;
            return tmp_val ;;
        }else if(chn&BIT(3)){
            tmp_val=adc12_cb->adc_val[3];
            adc12_cb->adc_val[3]=0;
            return tmp_val ;
        }else if(chn&BIT(4)){
            tmp_val=adc12_cb->adc_val[4];
            adc12_cb->adc_val[4]=0;
            return tmp_val ;
        }else if(chn&BIT(5)){
            tmp_val=adc12_cb->adc_val[5];
            adc12_cb->adc_val[5]=0;
            return tmp_val ;
        }else {
            return 0x7FFF;
        }
    }
    else{
        return 0x7FFF;
    }
}

AT(.com_text.adc12.get_value_mv)
u16 adc12_get_value_mv(u8 channel)
{

    u16 adc_val = adc12_get_value(channel);
    //printf("adc_val = %d\n");

    if (adc12_cb->split4_en & channel) {
        return (adc_val * 2000 * 4) / 4096;
    } else {
        return (adc_val * 2000) / 4096;
    }
    return 0;

}



AT(.com_text.adc12.kick)
bool adc12_kick(void)
{
    memset(adc12_cb->adc_val,0,sizeof(adc12_cb->adc_val));
    adc12_cb->cmd=ADC12_CMD_KICK_ADC;
    adc12_cb->sta=ADC12_STA_IDLE;
    adc12_kick_gpio_toggle();
    return true;
}


bool adc12_enter_lowpwr(uint8_t chn)
{
   return adc12_into_sleep(chn);
}


bool adc12_write_reg(u8 addr, u8 data)
{
    if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }
     adc12_cb->cmd=ADC12_CMD_WRITE_REG;
     u8 data_buf[4] = {0};
     data_buf[0]= ADC12_CMD_WRITE_REG;
     data_buf[1]=addr;
     data_buf[2]= data;
     data_buf[3]= adc12_checksum_calc(data_buf, sizeof(data_buf)-1);
     for(uint8_t i=0;i<3;i++){
        adc12_uart_send_buf(data_buf, sizeof(data_buf));
        if(adc12_kick_wait_rsp(10000)){
            TRACE("adc12_write_reg ok! addr %02x --> data %02x\n",data_buf[1],data_buf[2]);
            return true;
        }
     }

    TRACE("adc12_write_reg error!\n");
    return false;
}

bool adc12_read_reg(u8 addr, u8 *buf)
{
     if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }
     adc12_cb->cmd=ADC12_CMD_READ_REG;
     u8 data_buf[4] = {0};
     data_buf[0]= ADC12_CMD_READ_REG;
     data_buf[1]=addr;
     data_buf[2]= 0X00;
     data_buf[3]= adc12_checksum_calc(data_buf, sizeof(data_buf)-1);

    adc12_uart_send_buf(data_buf, sizeof(data_buf));
    if(adc12_kick_wait_rsp(10000)){
        TRACE("adc12_read_reg ok!\n");
        *buf= adc12_cb->adc_reg_val;
        return true;
    }
    TRACE("adc12_read_reg error!\n");
    return false;

}




bool adc12_set_ch_de_out(u8 channel, bool level)
{

    uint8_t pval=0;
    uint8_t dirval=0;
    uint8_t deval=0;

    if(ADC12_CHANEL_P12==channel||ADC12_CHANEL_P10==channel||ADC12_CHANEL_P13==channel)
    {
        if (!adc12_read_reg(REG_ADDR_P1PORT, &pval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P1DIR, &dirval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P1DE, &deval)) {
            return false;
        }

      //printf("[P1]pval = %02x\n",pval);
      //printf("[P1]dirval = %02x\n",dirval);
      //printf("[P1]deval = %02x\n",deval);
      //printf("============================\n");

       if(ADC12_CHANEL_P12==channel){
              if(level){
                 pval=pval|BIT(2);
              }
              else{
                  pval= pval&(~BIT(2));
              }
              dirval=dirval|BIT(2);
              deval=deval&(~BIT(2));
       }
       else if(ADC12_CHANEL_P10==channel){
              if(level){
                 pval=pval|BIT(0);
              }
              else{
                  pval= pval&(~BIT(0));
              }
             dirval=dirval|BIT(0);
             deval=deval&(~BIT(0));
       }else if(ADC12_CHANEL_P13==channel){
             if(level){
                 pval=pval|BIT(3);
              }
              else{
                  pval= pval&(~BIT(3));
              }
             dirval=dirval|BIT(3);
             deval=deval&(~BIT(3));
       }
        if (!adc12_write_reg(REG_ADDR_P1PORT, pval)) {
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P1DIR, dirval)) {
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P1DE, deval)) {
            return false;
        }

       //printf("[P1]pval = %02x\n",pval);
       //printf("[P1]dirval = %02x\n",dirval);
       //printf("[P1]deval = %02x\n",deval);
    }
    else if(ADC12_CHANEL_P07==channel||ADC12_CHANEL_P03==channel||ADC12_CHANEL_P04==channel)
    {
        if (!adc12_read_reg(REG_ADDR_P0PORT, &pval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P0DIR, &dirval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P0DE, &deval)) {
            return false;
        }

      //printf("[P0]pval = %02x\n",pval);
      //printf("[P0]dirval = %02x\n",dirval);
      //printf("[P0]deval = %02x\n",deval);
      //printf("============================\n");

       if(ADC12_CHANEL_P07==channel){
              if(level){
                 pval=pval|BIT(7);
              }
              else{
                  pval= pval&(~BIT(7));
              }

              dirval=dirval|BIT(7);
              deval=deval&(~BIT(7));
       }
       else if(ADC12_CHANEL_P03==channel){
             if(level){
                 pval=pval|BIT(3);
              }
              else{
                  pval= pval&(~BIT(3));
              }
             dirval=dirval|BIT(3);
             deval=deval&(~BIT(3));
       }else if(ADC12_CHANEL_P04==channel){
              if(level){
                 pval=pval|BIT(4);
              }
              else{
                  pval= pval&(~BIT(4));
              }
             dirval=dirval|BIT(4);
             deval=deval&(~BIT(4));
       }
        if (!adc12_write_reg(REG_ADDR_P0PORT, pval)){
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P0DIR, dirval)){
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P0DE, deval)) {
            return false;
        }

      //printf("[P0]pval = %02x\n",pval);
      //printf("[P0]dirval = %02x\n",dirval);
      //printf("[P0]deval = %02x\n",deval);

    }


    return true;
}

bool adc12_set_ch_ana_in(u8 channel)
{
    uint8_t dirval=0;
    uint8_t deval=0;
    if(ADC12_CHANEL_P12==channel||ADC12_CHANEL_P10==channel||ADC12_CHANEL_P13==channel)
    {
        if (!adc12_read_reg(REG_ADDR_P1DIR, &dirval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P1DE, &deval)) {
            return false;
        }

       if(ADC12_CHANEL_P12==channel){
            dirval=dirval&(~BIT(2));
            deval=deval|BIT(2);
       }
       else if(ADC12_CHANEL_P10==channel){
            dirval=dirval&(~BIT(0));
            deval=deval|BIT(0);
       }
       else if(ADC12_CHANEL_P13==channel){
            dirval=dirval&(~BIT(3));
            deval=deval|BIT(3);
       }


        if (!adc12_write_reg(REG_ADDR_P1DIR, dirval)) {
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P1DE, deval)) {
            return false;
        }

    }
    else if(ADC12_CHANEL_P07==channel||ADC12_CHANEL_P03==channel||ADC12_CHANEL_P04==channel)
    {

        if (!adc12_read_reg(REG_ADDR_P0DIR, &dirval)) {
            return false;
        }
        if (!adc12_read_reg(REG_ADDR_P0DE, &deval)) {
            return false;
        }

       if(ADC12_CHANEL_P07==channel){
           dirval=dirval&(~BIT(7));
           deval=deval|BIT(7);
       }
       else if(ADC12_CHANEL_P03==channel){
            dirval=dirval&(~BIT(3));
            deval=deval|BIT(3);
       }
       else if(ADC12_CHANEL_P04==channel){
            dirval=dirval&(~BIT(4));
            deval=deval|BIT(4);
       }

        if (!adc12_write_reg(REG_ADDR_P0DIR, dirval)) {
            return false;
        }
        if (!adc12_write_reg(REG_ADDR_P0DE, deval)) {
            return false;
        }

    }
    return true;
}






AT(.com_text.adc12_uart.rx_handler)
void adc12_huart_rx_handler(uint8_t *buf,uint16_t len)
{
     //print_r(hsuart_adc12_driver_rx_buffer,len);
     if(len<3){
        adc12_cb->sta=ADC12_STA_ERROR;
        return ;
     }

     if(buf[len-1]!=adc12_checksum_calc(buf, len-1)){
         adc12_cb->sta=ADC12_STA_ERROR;
         return ;
     }

     if(adc12_cb->cmd==ADC12_CMD_SET_ADCCHN||adc12_cb->cmd==ADC12_CMD_KICK_ADC){
          uint8_t adc_chanel=0;
          for(uint8_t i=0;i<len-1;i=i+2){
               adc_chanel=(buf[i+1] & 0xf0)>>4;
               uint16_t  adc_val =0;
               adc_val = (uint16_t)buf[i] << 4;
               adc_val = adc_val+(buf[i+1] & 0x0f);
               adc12_cb->adc_val[adc_chanel-1]=adc_val;
          }
     }
     else if(adc12_cb->cmd==ADC12_CMD_READ_REG){
        adc12_cb->adc_reg_val=buf[1];
     }

      adc12_cb->sta= ADC12_STA_FINISH;
}


bool adc12_sleep_before_ready(u8 p0_pupdsta, u8 p1_pupdsta)
{
     if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }
     adc12_cb->cmd=ADC12_CMD_SLEEP_READY;
     u8 data_buf[4] = {0};
     data_buf[0]= ADC12_CMD_SLEEP_READY;
     data_buf[1]=p0_pupdsta;
     data_buf[2]= p1_pupdsta;
     data_buf[3]= adc12_checksum_calc(data_buf, sizeof(data_buf)-1);
     adc12_uart_send_buf(data_buf, sizeof(data_buf));
    if(adc12_kick_wait_rsp(10000)){
        TRACE("adc12_sleep_before_ready ok!\n");
        return true;
    }

    TRACE("adc12_sleep_before_ready error!\n");
    return false;

}

bool adc12_wake_up_config(u8 p0_wakeup, u8 p1_wakeup)
{
     if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }
     adc12_cb->cmd=ADC12_CMD_IN_SLEEP;
     u8 data_buf[4] = {0};
     data_buf[0]= ADC12_CMD_IN_SLEEP;
     data_buf[1]=p0_wakeup;
     data_buf[2]= p1_wakeup;
     data_buf[3]= adc12_checksum_calc(data_buf, sizeof(data_buf)-1);
     adc12_uart_send_buf(data_buf, sizeof(data_buf));
    if(adc12_kick_wait_rsp(10000)){
        TRACE("adc12_wake_up_config ok!\n");
        return true;
    }
    TRACE("adc12_wake_up_config error!\n");
    return false;

}



bool adc12_into_sleep(uint8_t chn)
{
   uint8_t p0=0;
   uint8_t p1=0;

   if(chn&BIT(0)){
     p1=p1|BIT(3);
   }

   if(chn&BIT(1)){
     p0=p0|BIT(4);
   }

   if(chn&BIT(2)){
     p0=p0|BIT(7);
   }

   if(chn&BIT(3)){
    p1=p1|BIT(0);
   }

   if(chn&BIT(4)){
      p0=p0|BIT(3);
   }

   if(chn&BIT(5)){
     p1=p1|BIT(2);
   }


   if(adc12_sleep_before_ready(p0,p1)==false){
      return false;
   }

   adc12_kick_gpio_deinit();
  // gpio_deinit(ADC12_KICK_PORT, ADC12_KICK_PIN);

   //P06(iokick)上拉唤醒
   if(adc12_wake_up_config(BIT(6),0)==false){
       return false;
   }

   TRACE("adc12_into_sleep OK\n");
   return true;
}

AT(.com_text.adc12)
uint8_t adc12_kick_wait_rsp(uint32_t timeout_us)
{
     for(uint32_t i=0;i<timeout_us;i++){
         delay_us(1);
         if(adc12_cb->sta==ADC12_STA_FINISH){
             return true;
         }
     }
     return false;
}

AT(.com_text.adc12)
void adc12_uart_baud_set(u32 baud_rate)
{
     if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }
     adc12_cb->cmd=ADC12_CMD_SET_BAUD;
     u8 data_buf[4] = {0};
     data_buf[0]= ADC12_CMD_SET_BAUD;
     data_buf[1]= (8000000 / baud_rate) - 1;
     data_buf[2]= 0;
     data_buf[3]= adc12_checksum_calc(data_buf, sizeof(data_buf)-1);
     adc12_uart_send_buf(data_buf, sizeof(data_buf));//设置 adc12 波特率
}



AT(.com_text.adc12)
bool adc12_kick_channel_set(u8 chn, u8 split4_en)
{

    adc12_cb->split4_en=split4_en;
    if (chn == 0) {
        TRACE("adc12 chanel setting error\n");
        return false;
    }

    if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
    }

    adc12_cb->cmd=ADC12_CMD_SET_ADCCHN;
    u8 tx_buf[4] = {0};
    tx_buf[0]=ADC12_CMD_SET_ADCCHN;
    tx_buf[1]=chn;
    tx_buf[2]=split4_en?chn:0;
    tx_buf[3]=adc12_checksum_calc(tx_buf, sizeof(tx_buf)-1);

    adc12_uart_send_buf(tx_buf, sizeof(tx_buf));
    if(adc12_kick_wait_rsp(10000)){

        adc12_cb->cur_baud=ADC12_IOKICK_BAUD;
        master_uart_baud_set(ADC12_IOKICK_BAUD);
        TRACE("adc12_kick_channel_set ok!\n");
        return true;
    }

    return false;

}





bool adc12_io_kick_set(uint32_t baud_rate)
{
     if(adc12_cb->cur_baud ==ADC12_IOKICK_BAUD){
        master_uart_baud_set(ADC12_UART_BAUD);
     }

    u8 tx_buf[4] = {0};
    adc12_cb->cmd=ADC12_CMD_SET_IOKICK;
    tx_buf[0]=ADC12_CMD_SET_IOKICK;
    tx_buf[1]=0x7e;
    tx_buf[2]=8000000 / baud_rate - 1;
    tx_buf[3]=adc12_checksum_calc(tx_buf, sizeof(tx_buf)-1);
    adc12_uart_send_buf(tx_buf, sizeof(tx_buf));
    if(adc12_kick_wait_rsp(30000)){
       adc12_cb->cur_baud=ADC12_IOKICK_BAUD;
       master_uart_baud_set(baud_rate);
       return true;
    }
     return false;

}




bool adc12_wakeup_config(void)
{
   if(adc12_io_kick_set(ADC12_IOKICK_BAUD)==false){
       return false;
   }
   return true;
}


bool adc12_uart_baud_init(uint32_t baud_rate)
{
       memset(adc12_cb, 0, sizeof(adc12_cb_t));
       adc12_cb->cur_baud = ADC12_UART_BAUD;
       master_uart_baud_set(ADC12_DEFAULT_BAUD);   //从机初始波特率100k
       adc12_uart_baud_set(ADC12_UART_BAUD);
       if(adc12_kick_wait_rsp(30000)){
              master_uart_baud_set(ADC12_UART_BAUD);
              TRACE("adc12_uart_baud_init true\n");
              return true;
       }
       else{
           master_uart_baud_set(ADC12_UART_BAUD);
           TRACE("adc12_uart_baud_init false\n");
           return false;
       }
}

void adc12_kick_init(adc12_cb_t *adc12)
{
   adc12_cb=adc12;
   adc12_uart_baud_init(ADC12_UART_BAUD);
   adc12_io_kick_set(ADC12_IOKICK_BAUD);

}

#endif


