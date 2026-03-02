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

void uart_data_handle(void);

#endif

