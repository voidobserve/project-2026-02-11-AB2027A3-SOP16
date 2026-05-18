#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "include.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define USER_DEBUG_ENABLE 0

#define USER_DATA_VALID_VAL 0xC5        // 校验，用于验证是不是第一次上电，之前写入的数据是否有效
#define USER_DATA_SAVE_INTERVAL_MS 2000 // 每次保存数据的间隔时间

// 需要掉电保存的数据
typedef struct __attribute__((packed))
{
    u8 valid; // 校验，用于验证是不是第一次上电，之前写入的数据是否有效
    u32 color;
} user_data_t;

// 定义电机状态（由单片机发送给蓝牙ic，单片机->蓝牙ic）
enum
{
    MOTOR_STATUS_NONE = 0x00, // 无状态，设备刚上电
    MOTOR_STATUS_FORWARD,
    MOTOR_STATUS_FORWARD_STOP, // 正转，但是电机停了下来
    MOTOR_STATUS_REVERSE,
    MOTOR_STATUS_REVERSE_STOP, // 反转，但是电机停了下来
};
typedef u8 motor_status_t;

extern volatile motor_status_t motor_0_status;
extern volatile motor_status_t motor_1_status;
extern volatile user_data_t user_data;

void user_data_write(void);
void user_data_read(void);

void user_init(void);
void user_main(void);

void user_ws2812_service(void);

void ble_user_server_message_deal(u8 *buffer, u16 len);

void led_left_pwr_on(void);
void led_left_pwr_off(void);
void led_right_pwr_on(void);
void led_right_pwr_off(void);
// void uart_send_motor_reverse(void);
void uart_send_motor_left_reverse(void);
void uart_send_motor_right_reverse(void);

#if USER_DEBUG_ENABLE
#include "user_debug_io.h"
#endif

#include "colorful_light_ctl.h"
#include "user_delay_ctx.h"

#endif