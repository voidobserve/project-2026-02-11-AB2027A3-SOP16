#ifndef __USER_DELAY_CTX_H__
#define __USER_DELAY_CTX_H__

#include "include.h"

// 定义用户自定义的延时控制结构
typedef struct
{
    u32 trigger_tick; // 通过调用 tick_get() 来更新
    u32 delay_ms;     // 目标延时时间
    // u8 pending;       // 延时是否完成（由于可以借助官方提供的接口检测到任务是否延时到了指定时间 tick_check_expire()，这里不添加这个变量）
    u8 is_enable; // 该任务是否使能
    // u8 is_handle_once; // 该任务是否只执行一次（默认只执行一次，所以不加这个功能）
    void (*callback)(void); // 延时完成时，执行的回调函数
} user_delay_ctx_t;         // control timer x，执行延时操作的控制块

// 需要延时一段时间再执行的任务id
typedef enum
{
    USER_DELAY_CTX_SAVE_DATA, // 保存数据
    // USER_DELAY_CTX_LED_LEFT_ON,
    // USER_DELAY_CTX_LED_LEFT_OFF,
    
    // USER_DELAY_CTX_LED_RIGHT_ON,
    // USER_DELAY_CTX_LED_RIGHT_OFF,

    // USER_DELAY_CTX_MOTOR_OFF,

    // USER_DELAY_CTX_MOTOR_LEFT_OFF,
    // USER_DELAY_CTX_MOTOR_RIGHT_OFF,

    USER_DELAY_CTX_ID_MAX,
} user_delay_ctx_id_t;

void user_delay_ctx_init(void);
void user_delay_ctx_set(user_delay_ctx_id_t id, u32 delay_ms);
void user_delay_ctx_cancel(user_delay_ctx_id_t id);
void user_delay_ctx_handle(void);

#endif