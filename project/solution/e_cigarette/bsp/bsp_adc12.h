#ifndef _BSP_ADC12_H
#define _BSP_ADC12_H

#define ADC12_DEFAULT_BAUD          100000  //波特率
#define ADC12_UART_BAUD             1000000 //波特率
#define ADC12_IOKICK_BAUD           2000000 //iokick响应波特率

#define REG_ADDR_P0PORT             0x80
#define REG_ADDR_P0DIR              0x88 //0输入 1输出
#define REG_ADDR_P0PU               0xA0 //上拉20k
#define REG_ADDR_P0PD               0x98 //下拉20k
#define REG_ADDR_P0DE               0x90 //0数字 1模拟
#define REG_ADDR_P1PORT             0xB0
#define REG_ADDR_P1DIR              0xC0
#define REG_ADDR_P1PU               0xD8
#define REG_ADDR_P1PD               0xE8
#define REG_ADDR_P1DE               0xB8





#define HSUART_ADC12_DRIVER_BAUD                1000000
//#define HSUART_ADC12_DRIVER_PORT_SEL            GPIOA_REG
//#define HSUART_ADC12_DRIVER_PIN_SEL             GPIO_PIN_0
#define HSUART_ADC12_DRIVER_OV_CNT              20
//#define ADC12_KICK_PORT                         GPIOB_REG
//#define ADC12_KICK_PIN                          GPIO_PIN_5






enum {
    ADC12_STA_IDLE=0x00,
    ADC12_STA_FINISH=0x01,
    ADC12_STA_ERROR,
};

enum {
    ADC12_CMD_KICK_ADC = 0x5C,
    ADC12_CMD_SET_ADCCHN = 0xA1,
    ADC12_CMD_WRITE_REG = 0xA3,
    ADC12_CMD_READ_REG= 0xA4,
    ADC12_CMD_IN_SLEEP= 0xA5,
    ADC12_CMD_SET_BAUD= 0xA6,
    ADC12_CMD_SET_IOKICK= 0xA7,
    ADC12_CMD_SLEEP_READY= 0xA8,
};

enum {
    ADC12_CHANEL_P13=0X01,//通道1
    ADC12_CHANEL_P04=0X02,//通道2
    ADC12_CHANEL_P07=0X04,//通道3
    ADC12_CHANEL_P10=0X08,//通道4
    ADC12_CHANEL_P03=0X10,//通道5
    ADC12_CHANEL_P12=0X20,//通道6
    ADC12_CHANEL_ALL=0X2f,
};


typedef struct {
  uint16_t value1[10];
  uint16_t value2[10];
  uint16_t vbat_val;
  uint16_t data_len;

} adc12_val_t;

typedef struct {
    uint8_t  sta;
    uint8_t  cmd;
    uint8_t  split4_en;
    uint32_t cur_baud;
    uint16_t adc_val[6];
    uint8_t  adc_buf[16];
    uint8_t  adc_reg_val;
    uint8_t  rx_buffer[32];
    uint8_t  rx_len;
} adc12_cb_t;



bool adc12_kick(void);

bool adc12_into_sleep(uint8_t chn);
bool adc12_wakeup_config(void);
bool adc12_write_reg(u8 addr, u8 data);
bool adc12_read_reg(u8 addr, u8 *buf);
void master_uart_baud_set(u32 baud_rate);

uint16_t adc12_get_value(uint8_t chn);
uint16_t adc12_get_value_mv(u8 channel);
bool adc12_kick_channel_set(u8 chn, u8 split4_en);
bool adc12_enter_lowpwr(u8 chn);
uint8_t adc12_kick_wait_rsp(uint32_t timeout_us);
bool adc12_set_ch_ana_in(u8 channel);
bool adc12_set_ch_de_out(u8 channel, bool level);
void adc12_huart_rx_handler(uint8_t *buf,uint16_t len);


#endif // _ADC12_H
