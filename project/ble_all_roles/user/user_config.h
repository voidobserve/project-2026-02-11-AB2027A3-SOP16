#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "include.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define USER_DEBUG_ENABLE 1

// 需要掉电保存的数据
typedef struct __attribute__((packed))
{
    u32 color;
} user_data_t;

extern volatile user_data_t user_data;
 

void user_data_write(void);
void user_data_read(void);

void user_main(void);

void user_ws2812_service(void);

#if USER_DEBUG_ENABLE
#include "user_debug_io.h"
#endif

#endif