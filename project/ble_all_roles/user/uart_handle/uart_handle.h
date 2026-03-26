#ifndef __UART_HANDLE_H__
#define __UART_HANDLE_H__

#include "include.h"

#define UART_DATA_HANDLE_TIMEOUT ((u16)2000) // 接收数据的超时时间，单位：ms
#define UART_DATA_HANDLE_FORMAT_HEAD 0xA5    // 数据格式头

enum
{
    UART_DATA_HANDLE_STATUS_IDLE = 0,
    UART_DATA_HANDLE_STATUS_FORMAT_HEAD, // 格式头
    UART_DATA_HANDLE_STATUS_LEN,         // 数据长度
    UART_DATA_HANDLE_STATUS_END,
};

// 串口控制命令，蓝牙ic->单片机
enum
{
    MOTOR_CMD_STOP = 0x00,
    MOTOR_CMD_FORWARD = 0x01,
    MOTOR_CMD_REVERSE = 0x02,
};
typedef u8 motor_cmd_t;


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

enum
{
    MOTOR_INDEX_LEFT = 0x01,
    MOTOR_INDEX_RIGHT = 0x02,
};
typedef u8 motor_index_t;

void uart_send_cmd(motor_index_t motor_index, motor_cmd_t cmd);
void uart_data_handle(void);

#endif
