#include "user_delay_ctx.h"
#include "user_config.h"

static volatile user_delay_ctx_t user_delay_ctx[USER_DELAY_CTX_ID_MAX] = {0};

void user_delay_ctx_init(void)
{
    u8 i;
    for (i = 0; i < ARRAY_SIZE(user_delay_ctx); i++)
    {
        user_delay_ctx[i].trigger_tick = 0;
        user_delay_ctx[i].delay_ms = 0;
        user_delay_ctx[i].is_enable = 0;
        user_delay_ctx[i].callback = NULL;
    }

    // 注册延时任务：
    user_delay_ctx[USER_DELAY_CTX_SAVE_DATA].callback = user_data_write;
    // user_delay_ctx[USER_DELAY_CTX_LED_LEFT_ON].callback = led_left_pwr_on;
    // user_delay_ctx[USER_DELAY_CTX_LED_LEFT_OFF].callback = led_left_pwr_off;
    // user_delay_ctx[USER_DELAY_CTX_LED_RIGHT_ON].callback = led_right_pwr_on;
    // user_delay_ctx[USER_DELAY_CTX_LED_RIGHT_OFF].callback = led_right_pwr_off;

    // user_delay_ctx[USER_DELAY_CTX_MOTOR_OFF].callback = uart_send_motor_reverse;

    // user_delay_ctx[USER_DELAY_CTX_MOTOR_LEFT_OFF].callback = uart_send_motor_left_reverse;
    // user_delay_ctx[USER_DELAY_CTX_MOTOR_RIGHT_OFF].callback = uart_send_motor_right_reverse;
}

// 设置延时任务，延时一段时间后执行
void user_delay_ctx_set(user_delay_ctx_id_t id, u32 delay_ms)
{
    if (user_delay_ctx[id].is_enable == 1)
    {
        // 由于单片机是一直发送电机的状态，蓝牙ic的串口接收中会重复收到这些状态，可能会重复进入该函数，会导致对应的任务延时时间一直无法到来
        return;
    }

    user_delay_ctx[id].is_enable = 0; // 先屏蔽，最后再使能
    user_delay_ctx[id].trigger_tick = tick_get();
    user_delay_ctx[id].delay_ms = delay_ms; 
    user_delay_ctx[id].is_enable = 1;
}

// 取消当前正在跑延时的任务
void user_delay_ctx_cancel(user_delay_ctx_id_t id)
{
    user_delay_ctx[id].is_enable = 0;
}

// 由主循环调用
void user_delay_ctx_handle(void)
{
    u8 i;
    for (i = 0; i < ARRAY_SIZE(user_delay_ctx); i++)
    {
        if (user_delay_ctx[i].is_enable == 0)
        {
            // 当前轮询到的任务没有使能，没有则继续轮询下一个任务
            continue;
        }

        if (tick_check_expire(user_delay_ctx[i].trigger_tick, user_delay_ctx[i].delay_ms))
        {
            // 延时时间到，执行操作
            // my_printf("user delay ctx %d delayed execution\n", i);
            user_delay_ctx[i].is_enable = 0; // 任务只执行一次

            if (user_delay_ctx[i].callback != NULL)
            {
                user_delay_ctx[i].callback();
            }
        }
    }
}
