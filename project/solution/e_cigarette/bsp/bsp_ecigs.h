#ifndef _BSP_ECIG_INIT_H
#define _BSP_ECIG_INIT_H

#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "bsp_adc12.h"

// ==================== 相关IO配置 ====================
#if (BSP_ECIGS_EN == ECIG_CONTROL_V2)

#define PWM1_OUT_PORT                       GPIOB_REG
#define PWM1_OUT_PIN                        GPIO_PIN_2

#define PWM2_OUT_PORT                       GPIOB_REG
#define PWM2_OUT_PIN                        GPIO_PIN_1

#define BSP_MIC_PORT                        GPIOB_REG
#define BSP_MIC_PIN                         GPIO_PIN_0

#define BSP_CHECK_RES_PORT                  GPIOA_REG
#define BSP_CHECK_RES_PIN                   GPIO_PIN_1

#define BSP_DET1_PORT                       GPIOA_REG
#define BSP_DET1_PIN                        GPIO_PIN_12

#define BSP_DET2_PORT                       GPIOA_REG
#define BSP_DET2_PIN                        GPIO_PIN_13

#define BSP_CCS_PORT                        GPIOA_REG
#define BSP_CCS_PIN                         GPIO_PIN_1

// ADC相关配置
#define HSUART_ADC12_DRIVER_BAUD            1000000
#define HSUART_ADC12_DRIVER_PORT_SEL        GPIOA_REG
#define HSUART_ADC12_DRIVER_PIN_SEL         GPIO_PIN_0
#define HSUART_ADC12_DRIVER_OV_CNT          20
#define ADC12_KICK_PORT                     GPIOB_REG
#define ADC12_KICK_PIN                      GPIO_PIN_5

#define BSP_ADC1_CHANEL                     ADC12_CHANEL_P04
#define BSP_ADC2_CHANEL                     ADC12_CHANEL_P03
#define BSP_ADC3_CHANEL                     ADC12_CHANEL_P07
#define BSP_ADC4_CHANEL                     ADC12_CHANEL_P10
#define VBAT_SAMPLE_CHANEL                  ADC12_CHANEL_P13

#elif (BSP_ECIGS_EN == ECIG_CONTROL_V1)

#define ADC_HOT                             ADC_CHANNEL_PB0     //加热adc
#define ADC2_RES                            ADC_CHANNEL_PB1     //检阻adc

#define PWM1_OUT_PORT                       GPIOB_REG
#define PWM1_OUT_PIN                        GPIO_PIN_2

#define PWM2_OUT_PORT                       GPIOB_REG
#define PWM2_OUT_PIN                        GPIO_PIN_4

#define BSP_MIC_PORT                        GPIOA_REG
#define BSP_MIC_PIN                         GPIO_PIN_0

#define BSP_ADCGND_PORT                     GPIOB_REG
#define BSP_ADCGND_PIN                      GPIO_PIN_6

#define ECIG_RES_WIRE                       0.6                 //无ADC2通路时电阻丝为固定阻值（Ω）
#define ECIG_RES_DIFF                       4.7                 //阻抗检测分压电阻值（Ω）
#define ECIG_SHORT_RES_MIN                  2                   //检阻时电阻丝小于(x/10)Ω判断为短路，0.2Ω
#define ECIG_OPEN_RES_MAX                   30                  //检阻时电阻丝大于(x/10)Ω判断为开路，3Ω
#define ECIG_SHORT_RES_PROP                 14                  //加热时判断短路
#define ECIG_OPEN_RES_PROP                  2000                //加热时判断开路

#define CAL_RES_TIME_CYCLE                  5                   //检阻次数
#endif

// 加热模式
typedef enum {
   MODE_RES1_HEATING = 0X01,
   MODE_RES2_HEATING = 0X02,
   MODE_RES1_RES2_SWITCH_HEATING = 0X03,
   MODE_RES1_RES2_ALL_HEATING = 0X04,
} HEAT_MODE;

// 烟控状态
typedef enum {
    IDLE = 0,
    SMOKING,
    SHORT_CIRCUIT,
    OPEN_CIRCUIT,
    LOW_POWER,
    SMOKE_TIMEOUT,
} SMOKE_STA;

// 线程驱动消息
enum {
    THREAD_DRIVER_MSG_MIC_START,
    THREAD_DRIVER_MSG_MIC_STOP,
    THREAD_DRIVER_MSG_MIC_TIMEOUT,
    THREAD_DRIVER_MSG_RES1_PULL_STA,
    THREAD_DRIVER_MSG_RES1_INSERT_STA,
    THREAD_DRIVER_MSG_RES2_PULL_STA,
    THREAD_DRIVER_MSG_RES2_INSERT_STA,
    THREAD_DRIVER_MSG_RES_CAL_START,
    THREAD_DRIVER_MSG_SET_HEAT_SAMPLE_CHANNEL,
    THREAD_DRIVER_MSG_REACH_TARGET_POWER,        // 达到目标功率
    THREAD_DRIVER_MSG_UNABLE_REACH_TARGET_POWER,
    THREAD_DRIVER_MSG_VBAT_LOW_STATUS,

    THREAD_DRIVER_MSG_SMOKE_MAX = 0x80,
};

// ==================== 通用结构体定义 ====================

// 烟控主结构体
typedef struct {
#if (BSP_ECIGS_EN == ECIG_CONTROL_V2)
    u32 target_power;           // 加热目标功率
    u32 current_power;          // 加热当前功率
    u16 vbat_val;               // 电池电压
    u8 smoke_sta;               // 烟控状态
    u8 short_circuit_flag;      // 短路标志位
    u8 heat_mode;               // 加热模式
    u16 adc12_val1[10];
    u16 adc12_val2[10];
    u16 adc12_val3[10];
    u16 adc12_val4[10];
    u32 mic_debounce_time;      // 吸烟消除抖动时间
    u32 heat_time_cnt;
    u32 res1_val;
    u32 res2_val;
    u8 res1_det_flag;           // 加热电阻1检测在线
    u8 res2_det_flag;           // 加热电阻2检测在线
    uint8_t reg1_chnl;          // 加热电阻1检测ADC通道
    uint8_t heat1_chnl;         // 加热电压1检测ADC通道
    uint8_t reg2_chnl;          // 加热电阻2检测ADC通道
    uint8_t heat2_chnl;         // 加热电压2检测ADC通道
    uint8_t vbat_chnl;          // 电池检测ADC通道
    u8 res_cal_flag;            // 加热电阻计算完成标志
    u8 heat_chn_set_flag;       // 设置完成标志
    u32 timer_100us_cnt;
    u8 power_flag;              // 达到目标功率标志
    u16 res1_heat_vol;          // 加热电阻1的加热电压
    u16 res2_heat_vol;          // 加热电阻2的加热电压
    u16 vbat_low_flag;          // 电池电压标志
    u8 heat_switch;             // 加热切换
    
#elif (BSP_ECIGS_EN == ECIG_CONTROL_V1)
    u8 smoke_sta;               // 烟控状态
    u32 AD_hot_voltage;         // 定点VAT
    u32 AD_hot_voltage_mv;      // 真实VAT
    u32 AD_BAT_voltage_mv;      // VBAT
    u32 p_current;              // 当前功率
    u32 p_prev;                 // 前一次功率
    u8 power_on_flag;           // PWM开启标志位
    u32 hot_time_tick;          // 加热计时
    u8 timer_100us_cnt;         // 100us计时
    u8 mic_start;               // mic start信号
    volatile u32 adc1;
    volatile u32 adc2;
    volatile u32 adc_vbg;
    u8 final_power;             // 目标功率
    u8 max_hot_time;            // 最长加热时间
    u8 short_res_prop;          // 短路保护，电热丝阻值和MOS内阻（100mΩ）的最小比例，阻值相近乘10提高精度，10--0.1Ω视作短路
    u16 open_res_prop;          // 开路保护，电热丝阻值和MOS内阻（100mΩ）的最大比例，阻值相近乘10提高精度，200--2Ω视作开路（无ADC2通路时有效）
    u8 short_res_proportion;    // 电阻短路时adc比例（10倍）
    u8 open_res_proportion;     // 电阻断路时adc比例（10倍）
    u16 res_diff;               // 阻抗检测分压电阻值（Ω，8192倍）
    u16 hot_res;                // 电热丝阻值（Ω，8192倍）
#endif
} smoke_ctrl_t;


// ==================== 通用函数声明 ====================
void e_cigs_init(void);
void e_cigs_event_handler(void);
void bsp_hot_proc(void);
uint8_t mic_start_or_not(void);
uint8_t get_smoke_sta(void);
void e_cigs_vbat_proc(void);
void thread_driver_smoke_event_callback(uint8_t msg);

#if (BSP_ECIGS_EN == ECIG_CONTROL_V2)
void e_cigs_sleep_config(void);
void e_cigs_wakeup_config(void);
void adc12_kick_init(adc12_cb_t *adc12);
bool adc12_kick(void);
bool adc12_wakeup_config(void);
bool adc12_write_reg(u8 addr, u8 data);
bool adc12_read_reg(u8 addr, u8 *buf);
uint16_t adc12_get_value(uint8_t chn);
uint16_t adc12_get_value_mv(u8 channel);
bool adc12_kick_channel_set(u8 chn, u8 split4_en);
bool adc12_enter_lowpwr(u8 chn);
uint8_t adc12_kick_wait_rsp(uint32_t timeout_us);
bool adc12_set_ch_ana_in(u8 channel);
bool adc12_set_ch_de_out(u8 channel, bool level);
void adc12_kick_gpio_init(void);
void adc12_huart_rx_handler(uint8_t *buf, uint16_t len);
#endif

#endif 
