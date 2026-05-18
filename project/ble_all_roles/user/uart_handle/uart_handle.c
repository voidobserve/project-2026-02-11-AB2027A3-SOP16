#include "uart_handle.h"
#include "user_config.h"
#include "driver_uart.h"

/**
 * @brief 发送控制命令给单片机
 *
 * @param motor_index 电机索引，
 *              0x01：电机1，
 *              0x02：电机2
 * @param cmd
 *          0x00：停止，
 *          0x01：正转，
 *          0x02：反转
 */
void uart_send_cmd(motor_index_t motor_index, motor_cmd_t cmd)
{
    u8 i;
    u8 j;
    u8 buf[5] = {
        UART_DATA_HANDLE_FORMAT_HEAD, // 格式头
        0x05,                         // 整一帧数据的长度
        motor_index,                  // 电机索引，
        cmd,                          // 命令
        0x00,                         // 校验和，后面会计算
    };

    buf[4] = (buf[0] + buf[1] + buf[2] + buf[3]) & 0xFF; // 计算校验和

    for (j = 0; j < 2; j++) // 控制重复的发送次数，例如 1：发送一次，2：重复发送两次
    {
        for (i = 0; i < ARRAY_SIZE(buf); i++) // 发送数据帧
        {
            while (uart_get_flag(UART, UART_IT_TX) != SET)
                ; // 等待发送完成
            uart_send_data(UART, (u16)buf[i]);

            WDT_CLR();
        }
    }
}

// 处理串口接收到的数据
void uart_data_handle(void)
{
    // USER_DEBUG_IO_TOGGLE(); // 目前最长是 194 ms 调用一次

    static volatile u8 cmd_buff[10] = {0};    // 存放接收到的一条指令
    static volatile u8 cur_cmd_buff_len = 0;  // 指示当前接收到的指令的索引（之后会在程序中更新，不用清零）
    static volatile u8 dest_cmd_buff_len = 0; // 存放最终要接收的指令长度（之后会在程序中更新，不用清零）

    static volatile u8 timeout_enable = 0; // 超时计数使能
    static volatile u32 timeout_cnt = 0;   // 超时计数（基于系统时基，运行时值不为0）

    static volatile u8 uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 状态机

    u8 recv_byte;
    volatile u8 check_sum = 0;        // 存放计算之后的校验和
    volatile u8 i;                    // 循环计数值
    volatile u8 is_recv_complete = 0; // 是否接收到完整的一帧指令

    // 存放上一次的电机状态
    // static volatile motor_status_t last_motor_0_status = MOTOR_STATUS_NONE;
    // static volatile motor_status_t last_motor_1_status = MOTOR_STATUS_NONE;
    // 存放最终得到的电机状态（这里如果不加static修饰，会有问题）

    // 接收超时处理：
    if (0 == uart_rxbuffer_get_count())
    {
        if (timeout_enable &&
            tick_check_expire(timeout_cnt, UART_DATA_HANDLE_TIMEOUT))
        {
            // 接收超时
            // timeout_cnt = 0;
            // timeout_cnt = tick_get();
            timeout_enable = 0;                                     // 不使能超时计数
            uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 重新开始接收

#if USER_DEBUG_ENABLE
            // 超时之后，打印缓冲区内的数据
            // my_printf("=================================>\n");
            // my_printf("uart recv timeout\n");
            // for (i = 0; i < ARRAY_SIZE(cmd_buff); i++)
            // {
            //     my_printf("%02x ", (u16)cmd_buff[i]);
            // }
            // my_printf("=================================^\n");
#endif
        }

        return; // 串口缓冲区的数据为空，直接返回
    }

    while (1) // 有时候该函数会100~200ms才调用一次，这里一次性把缓冲区中的数据读出来
    {
        if (uart_rxbuffer_get_count() == 0 || is_recv_complete) // 退出条件
        {
            if (is_recv_complete)
            {
                is_recv_complete = 0;
            }

            break;
        }

        timeout_enable = 1;       // 使能超时计数
        timeout_cnt = tick_get(); // 更新超时计数的时基
        recv_byte = uart_rxbuffer_get_byte();

        switch (uart_data_handle_status)
        {
        case UART_DATA_HANDLE_STATUS_IDLE:
            if (UART_DATA_HANDLE_FORMAT_HEAD == recv_byte)
            {
                cmd_buff[0] = recv_byte;
                cur_cmd_buff_len = 1;
                uart_data_handle_status = UART_DATA_HANDLE_STATUS_FORMAT_HEAD;
            }
            else
            {
                // 不是格式头，重新开始接收，关掉超时计数
                timeout_enable = 0;
            }
            break;
            // ===============================================================
        case UART_DATA_HANDLE_STATUS_FORMAT_HEAD:
            cmd_buff[cur_cmd_buff_len++] = recv_byte;
            dest_cmd_buff_len = recv_byte;                         // 存放要接收的数据长度
            uart_data_handle_status = UART_DATA_HANDLE_STATUS_LEN; // 表示接收到了数据帧长度
            // my_printf("len == %bu\n", dest_cmd_buff_len);
            break;
            // ===============================================================
        case UART_DATA_HANDLE_STATUS_LEN:
            cmd_buff[cur_cmd_buff_len++] = recv_byte;
            if (cur_cmd_buff_len >= dest_cmd_buff_len) // 如果接收完所有的数据
            {
                for (i = 0; i < dest_cmd_buff_len - 1; i++)
                {
                    check_sum += cmd_buff[i];
                }

                if (check_sum != cmd_buff[dest_cmd_buff_len - 1])
                {
                    // 校验和错误
#if USER_DEBUG_ENABLE

                    // my_printf("=================================>\n");
                    // my_printf("check sum error\n");
                    // for (i = 0; i < ARRAY_SIZE(cmd_buff); i++)
                    // {
                    //     my_printf("%02x ", (u16)cmd_buff[i]);
                    // }
                    // my_printf("=================================^\n");
#endif

                    // timeout_cnt = 0;
                    timeout_enable = 0;                                     // 不使能超时计数
                    uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 重新接收数据
                }
                else
                {
// 校验和正确
#if USER_DEBUG_ENABLE

                    // my_printf("check sum ok\n");
#endif
                    uart_data_handle_status = UART_DATA_HANDLE_STATUS_END;
                    is_recv_complete = 1;
                }
            }
            break;
        // ===============================================================
        default:

            break;
        }
    }

    if (UART_DATA_HANDLE_STATUS_END != uart_data_handle_status)
    {
        return; // 未接收完数据，不进入下面的处理操作，函数直接返回
    }

    // 打印接收到的一帧数据
    // for (i = 0; i < dest_cmd_buff_len; i++)
    // {
    //     my_printf("0x%02x ", (u16)cmd_buff[i]);
    // }
    // my_printf("\n");

    if (cmd_buff[2] == 0x01)
    {
        // 如果是第一个电机
        motor_0_status = cmd_buff[3];
    }
    else
    {
        // 如果是第二个电机
        motor_1_status = cmd_buff[3];
    }

    /*
        如果左灯点亮，但是收到了电机反转或者反转停止的信号，需要关掉灯
    */
    if (colorful_light_ctl.left_light_enable &&
        (motor_0_status == MOTOR_STATUS_REVERSE ||
         motor_0_status == MOTOR_STATUS_REVERSE_STOP))
    {
        colorful_light_ctl.left_light_enable = 0;
        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_LEFT_ON);
        led_left_pwr_off(); // 立即关闭led
    }

    if (colorful_light_ctl.right_light_enable &&
        (motor_1_status == MOTOR_STATUS_REVERSE ||
         motor_1_status == MOTOR_STATUS_REVERSE_STOP))
    {
        colorful_light_ctl.right_light_enable = 0;
        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_RIGHT_ON);
        led_right_pwr_off(); // 立即关闭led
    }

    // 灯光没有开，但是电机已经在正转停止
    if (colorful_light_ctl.left_light_enable == 0 &&
        motor_0_status == MOTOR_STATUS_FORWARD_STOP)
    {
        // 电机正转，停下来时，再打开灯光，防止电机运转时造成灯光闪烁
        colorful_light_ctl.left_light_enable = 1;
        led_left_pwr_on();
    }

    // 灯光没有开，但是电机已经在正转停止
    if (colorful_light_ctl.right_light_enable == 0 &&
         motor_1_status == MOTOR_STATUS_FORWARD_STOP)
    {
        // 电机正转，停下来时，再打开灯光，防止电机运转时造成灯光闪烁
        colorful_light_ctl.right_light_enable = 1;
        led_right_pwr_on();
    }

    // if (last_motor_0_status != motor_0_status)
    // {
    //     last_motor_0_status = motor_0_status;
    // }

    // if (last_motor_1_status != motor_1_status)
    // {
    //     last_motor_1_status = motor_1_status;
    // }

    // 处理完成后，重新接收数据
    // timeout_cnt = 0;
    timeout_enable = 0; // 不使能超时计数
    uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE;
}