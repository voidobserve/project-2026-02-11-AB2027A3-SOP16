#include "uart_handle.h"
#include "user_config.h"

// 处理串口接收到的数据
void uart_data_handle(void)
{
    // USER_DEBUG_IO_TOGGLE(); // 目前最长是 194 ms 调用一次

#if 1
    static volatile u8 cmd_buff[10] = {0};    // 存放接收到的一条指令
    static volatile u8 cur_cmd_buff_len = 0;  // 指示当前接收到的指令的索引（之后会在程序中更新，不用清零）
    static volatile u8 dest_cmd_buff_len = 0; // 存放最终要接收的指令长度（之后会在程序中更新，不用清零）

    static volatile u8 timeout_enable = 0; // 超时计数使能
    static volatile u32 timeout_cnt = 0;   // 超时计数（基于系统时基，运行时值不为0）

    static volatile u8 uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 状态机

    u8 recv_byte;
    u8 check_sum = 0;        // 存放计算之后的校验和
    u8 i;                    // 循环计数值
    u8 is_recv_complete = 0; // 是否接收到完整的一帧指令
    uint32_t colors[MAX_NUM_COLORS];

    // 存放最终得到的电机状态
    static u8 motor_0_status = 0;
    static u8 motor_1_status = 0;

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

            // 超时之后，打印缓冲区内的数据
            my_printf("=================================>\n");
            my_printf("uart recv timeout\n");
            for (i = 0; i < ARRAY_SIZE(cmd_buff); i++)
            {
                my_printf("%02x ", (u16)cmd_buff[i]);
            }
            my_printf("=================================^\n");
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
                    my_printf("=================================>\n");
                    my_printf("check sum error\n");
                    for (i = 0; i < ARRAY_SIZE(cmd_buff); i++)
                    {
                        my_printf("%02x ", (u16)cmd_buff[i]);
                    }
                    my_printf("=================================^\n");

                    // timeout_cnt = 0;
                    timeout_enable = 0;                                     // 不使能超时计数
                    uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 重新接收数据
                }
                else
                {
                    // 校验和正确
                    my_printf("check sum ok\n");
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

    // 处理一帧数据：
    switch (cmd_buff[2])
    {
        // 判断是哪个电机
    case 0x01:
        // my_printf("obj == motor 0 \n");
        switch (cmd_buff[3])
        {
            // 判断要执行什么操作
        case 0x00: // 停止
            motor_0_status = 0;

            my_printf("motor 0 stop\n");
            // memset(colors, 0x00, sizeof(colors));
            // colors[0] = BLACK;

            // WS2812FX_set_coloQty(0, 1);
            // WS2812FX_setSegment_colorsOptions(
            //     0,                        // 第0段
            //     0,                        // 起始位置
            //     WS2812_LED_NUM_MAX - 1,   // 结束位置
            //     &WS2812FX_mode_static,    // 效果
            //     colors,                   // 颜色，WS2812FX_setColors设置
            //     WS2812FX_getSpeed_seg(0), // 速度
            //     SIZE_SMALL                // 选项，这里像素点大小：1
            // );
            // WS2812FX_trigger();
            // WS2812FX_start();
            break;
        case 0x01: // 正转
            motor_0_status = 1;
            my_printf("motor 0 forward\n");

            // memset(colors, 0x00, sizeof(colors));
            // colors[0] = RED;

            // WS2812FX_set_coloQty(0, 1);
            // WS2812FX_setSegment_colorsOptions(
            //     0,                        // 第0段
            //     0,                        // 起始位置
            //     WS2812_LED_NUM_MAX - 1,   // 结束位置
            //     &WS2812FX_mode_static,    // 效果
            //     colors,                   // 颜色，WS2812FX_setColors设置
            //     WS2812FX_getSpeed_seg(0), // 速度
            //     SIZE_SMALL                // 选项，这里像素点大小：1
            // );
            // WS2812FX_trigger();
            // WS2812FX_start();

            break;
        case 0x02: // 反转
            motor_0_status = 2;
            my_printf("motor 0 reverse\n");

            // memset(colors, 0x00, sizeof(colors));
            // colors[0] = GREEN;

            // WS2812FX_set_coloQty(0, 1);
            // WS2812FX_setSegment_colorsOptions(
            //     0,                        // 第0段
            //     0,                        // 起始位置
            //     WS2812_LED_NUM_MAX - 1,   // 结束位置
            //     &WS2812FX_mode_static,    // 效果
            //     colors,                   // 颜色，WS2812FX_setColors设置
            //     WS2812FX_getSpeed_seg(0), // 速度
            //     SIZE_SMALL                // 选项，这里像素点大小：1
            // );
            // WS2812FX_trigger();
            // WS2812FX_start();
            break;
        default:
            break;
        }
        break;
        // =======================================
    case 0x02:
        // my_printf("motor 1 \n");
        switch (cmd_buff[3])
        {
            // 判断要执行什么操作
        case 0x00: // 停止
            motor_1_status = 0;
            my_printf("motor 1 stop\n");
            break;
        case 0x01: // 正转
            motor_1_status = 1;
            my_printf("motor 1 forward\n");
            break;
        case 0x02: // 反转
            motor_1_status = 2;
            my_printf("motor 1 reverse\n");
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    if (motor_0_status == 0 && motor_1_status == 0)
    {
        // 如果两个电机都停止，关闭灯光
        memset(colors, 0x00, sizeof(colors));
        colors[0] = BLACK;

        WS2812FX_set_coloQty(0, 1);
        WS2812FX_setSegment_colorsOptions(
            0,                        // 第0段
            0,                        // 起始位置
            WS2812_LED_NUM_MAX - 1,   // 结束位置
            &WS2812FX_mode_static,    // 效果
            colors,                   // 颜色，WS2812FX_setColors设置
            WS2812FX_getSpeed_seg(0), // 速度
            SIZE_SMALL                // 选项，这里像素点大小：1
        );
        WS2812FX_trigger();
        WS2812FX_start();
    }
    else if (motor_0_status != 0 || motor_1_status != 0)
    {
        // 如果两个电机都启动，打开灯光 
        memset(colors, 0x00, sizeof(colors));
        colors[0] = RED | GREEN | BLUE;

        WS2812FX_set_coloQty(0, 1);
        WS2812FX_setSegment_colorsOptions(
            0,                        // 第0段
            0,                        // 起始位置
            WS2812_LED_NUM_MAX - 1,   // 结束位置
            &WS2812FX_mode_static,    // 效果
            colors,                   // 颜色，WS2812FX_setColors设置
            WS2812FX_getSpeed_seg(0), // 速度
            SIZE_SMALL                // 选项，这里像素点大小：1
        );
        WS2812FX_trigger();
        WS2812FX_start();
    }

    // 修改灯光效果后，需要保存效果到flash 
    user_data.color = colors[0];
    user_data_write();

    // 处理完成后，重新接收数据
    // timeout_cnt = 0;
    timeout_enable = 0; // 不使能超时计数
    uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE;

#if 0
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
                my_printf("check sum error\n");
                timeout_cnt = 0;
                timeout_enable = 0;                                     // 不使能超时计数
                uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE; // 重新接收数据
            }
            else
            {
                // 校验和正确
                my_printf("check sum ok\n");
                uart_data_handle_status = UART_DATA_HANDLE_STATUS_END;
            }
        }
        break;
    // ===============================================================
    default:

        break;
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

    switch (cmd_buff[2])
    {
        // 判断是哪个电机
    case 0x01:
        // my_printf("obj == motor 0 \n");
        switch (cmd_buff[3])
        {
            // 判断要执行什么操作
        case 0x00: // 停止
            my_printf("motor 0 stop\n");
            memset(colors, 0x00, sizeof(colors));
            colors[0] = BLACK;

            WS2812FX_set_coloQty(0, 1);
            WS2812FX_setSegment_colorsOptions(
                0,                        // 第0段
                0,                        // 起始位置
                WS2812_LED_NUM_MAX - 1,   // 结束位置
                &WS2812FX_mode_static,    // 效果
                colors,                   // 颜色，WS2812FX_setColors设置
                WS2812FX_getSpeed_seg(0), // 速度
                SIZE_SMALL                // 选项，这里像素点大小：1
            );
            WS2812FX_trigger();
            WS2812FX_start();

            break;
        case 0x01: // 正转
            my_printf("motor 0 forward\n");

            memset(colors, 0x00, sizeof(colors));
            colors[0] = RED;

            WS2812FX_set_coloQty(0, 1);
            WS2812FX_setSegment_colorsOptions(
                0,                        // 第0段
                0,                        // 起始位置
                WS2812_LED_NUM_MAX - 1,   // 结束位置
                &WS2812FX_mode_static,    // 效果
                colors,                   // 颜色，WS2812FX_setColors设置
                WS2812FX_getSpeed_seg(0), // 速度
                SIZE_SMALL                // 选项，这里像素点大小：1
            );
            WS2812FX_trigger();
            WS2812FX_start();

            break;
        case 0x02: // 反转
            my_printf("motor 0 reverse\n");

            memset(colors, 0x00, sizeof(colors));
            colors[0] = GREEN;

            WS2812FX_set_coloQty(0, 1);
            WS2812FX_setSegment_colorsOptions(
                0,                        // 第0段
                0,                        // 起始位置
                WS2812_LED_NUM_MAX - 1,   // 结束位置
                &WS2812FX_mode_static,    // 效果
                colors,                   // 颜色，WS2812FX_setColors设置
                WS2812FX_getSpeed_seg(0), // 速度
                SIZE_SMALL                // 选项，这里像素点大小：1
            );
            WS2812FX_trigger();
            WS2812FX_start();
            break;
        default:
            break;
        }
        break;
        // =======================================
    case 0x02:
        // my_printf("motor 1 \n");
        switch (cmd_buff[3])
        {
            // 判断要执行什么操作
        case 0x00: // 停止
            break;
        case 0x01: // 正转
            break;
        case 0x02: // 反转
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    // 处理完成后，重新接收数据
    timeout_cnt = 0;
    timeout_enable = 0; // 不使能超时计数
    uart_data_handle_status = UART_DATA_HANDLE_STATUS_IDLE;
#endif

#endif
}