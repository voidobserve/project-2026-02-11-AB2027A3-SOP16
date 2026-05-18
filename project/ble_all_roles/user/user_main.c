#include "user_config.h"
#include "user_uart.h"
#include "SIT2515.h"
#include "driver_uart.h"
// #include "driver_gpio.h"

#include "uart_handle.h"
// #include "driver_gpio.h" // 初始化 测试时使用的gpio，需要引用该头文件

volatile user_data_t user_data = {0};

// volatile motor_status_t motor_sta = 0;

volatile motor_status_t motor_0_status = MOTOR_STATUS_NONE;
volatile motor_status_t motor_1_status = MOTOR_STATUS_NONE;

// CAN命令状态标志，用于防止重复执行相同命令
// static volatile u8 last_can_cmd = 0xFF; // 初始化为无效值

#define USER_DATE_SAVE_START_ADDR 0x00 // 起始地址

void user_data_write(void)
{
    bsp_param_write(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
    bsp_param_sync(); // 同步数据到flash
}

void user_data_read(void)
{
    bsp_param_read(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
#if USER_DEBUG_ENABLE
    // my_printf("save.color == 0x%lx\n", user_data.color);
#endif
}

void led_left_pwr_on(void)
{
    COLORFUL_LIGHT_LEFT_POWER_CTL_PIN_SET(); // 打开灯的电源
}

void led_left_pwr_off(void)
{
    COLORFUL_LIGHT_LEFT_POWER_CTL_PIN_RESET();
}

void led_right_pwr_on(void)
{
    COLORFUL_LIGHT_RIGHT_POWER_CTL_PIN_SET(); // 打开灯的电源
}

void led_right_pwr_off(void)
{
    COLORFUL_LIGHT_RIGHT_POWER_CTL_PIN_RESET();
}

// 初始化函数，在 bsp_sys.c -> bsp_sys_init() 中调用
void user_init(void)
{

#if WS2812_LIB_EN
    WS2812FX_init(WS2812_LED_NUM_MAX, WS2812_NEO_TYPE);
    // WS2812FX_setBrightness(255);
    WS2812FX_setBrightness(255 / 4); // 亮度（255/4，已经调节好）

    // 设置上电默认样式
    // uws2812_style_powerup_default();

#if UWS_POWERUP_DEFAULT == 0
    WS2812FX_show_cover_ptr(ws281x_none);
    // Adafruit_NeoPixel_clear();
    ws281x_show(Adafruit_NeoPixel_getPixels(), Adafruit_NeoPixel_getNumBytes()); // 即时填充发送灯全灭数据
#else
    WS2812FX_trigger();
    WS2812FX_start();
#endif
#endif //

    // 初始化串口
    user_uart_init();

    colorful_light_power_ctl_io_init(); // 幻彩灯的电源控制脚
    SIT2515_Init();                     // CAN收发器
    user_delay_ctx_init();

    user_data_read(); // 读取flash保存的数据
    if (user_data.valid != USER_DATA_VALID_VAL)
    {
        // 如果读出来的数据，校验不通过，说明是第一次上电
        // 初始化变量
        user_data.valid = USER_DATA_VALID_VAL;
        user_data.color = RED | GREEN | BLUE;

        user_data_write(); // 将数据写回flash
#if USER_DEBUG_ENABLE
        my_printf("is first pwr on\n");
#endif
    }
    else
    {
#if USER_DEBUG_ENABLE
        my_printf("is not first pwr on\n");
#endif
    }

    // last_can_cmd = 0xFF; // 默认是无效的指令
    // motor_0_status = MOTOR_STATUS_NONE; // 默认是无效状态
    // motor_1_status = MOTOR_STATUS_NONE; // 默认是无效状态
    colorful_light_ctl.left_light_enable = 0;
    colorful_light_ctl.right_light_enable = 0;
    colorful_light_set_static_color(user_data.color);

#if USER_DEBUG_ENABLE
    user_debug_io_init();
#endif

    // delay_ms(10);
}

// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
void user_ws2812_service(void)
{
#if WS2812_LIB_EN
    /*更新ws2812系统时钟计数*/
    static u32 last_tick = 0;
    u32 new_tick = tick_get();
    if (last_tick < new_tick)
    {
        run_tick_per_nms(new_tick - last_tick);
    }
    last_tick = new_tick;

    /*ws2812系统主任务*/
    WS2812FX_service();
#endif
}

// 处理can收发器接收到的数据
void can_handle(void)
{
    u8 cmd = 0;
    u8 buf[8] = {0};
    u8 ret = 0;
    u8 i;
    ret = CAN_Receive_Buffer(buf);
    if (ret == 0)
    {
        return;
    }

    // // 如果这一帧数据的长度不一致，不处理这一帧数据：
    //     if (ret != 6)
    //     {
    // #if USER_DEBUG_ENABLE
    //         my_printf("can frame len err\n");
    // #endif
    //         return;
    //     }

    // // 如果数据的前缀不一致，直接返回，不处理这一帧数据
    //     if (!(buf[0] == 0x00 &&
    //           buf[1] == 0x00 &&
    //           buf[2] == 0x01 &&
    //           buf[3] == 0x2A &&
    //           buf[4] == 0xD2))
    //     {
    // #if USER_DEBUG_ENABLE
    //         my_printf("can prefix err\n");
    // #endif
    //         return;
    //     }

    cmd = buf[2];
    if (cmd == 0x40 ||
        cmd == 0xC0 ||
        cmd == 0xD0 ||
        cmd == 0x10 ||
        cmd == 0x50 ||
        cmd == 0x90)
    {
#if USER_DEBUG_ENABLE
        my_printf("left open\n");
#endif
        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_LEFT_OFF);
        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_RIGHT_OFF);

        // 不能立即开灯，电机运转期间，电流过大，会导致灯一直频闪
        // colorful_light_ctl.left_light_enable = 1;
        // colorful_light_ctl.right_light_enable = 1;
        // led_left_pwr_on();
        // led_right_pwr_on();

        // 电机没有在正转，才执行
        if (motor_0_status != MOTOR_STATUS_FORWARD &&
            motor_0_status != MOTOR_STATUS_FORWARD_STOP)
        {
            // user_delay_ctx_cancel(USER_DELAY_CTX_MOTOR_LEFT_OFF);
            uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_FORWARD);

            motor_0_status = MOTOR_STATUS_FORWARD; // 立即设置状态，防止下一次重复进入
        }

        if (motor_1_status != MOTOR_STATUS_FORWARD &&
            motor_1_status != MOTOR_STATUS_FORWARD_STOP)
        {
            // user_delay_ctx_cancel(USER_DELAY_CTX_MOTOR_RIGHT_OFF);
            uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_FORWARD);

            motor_1_status = MOTOR_STATUS_FORWARD; // 立即设置状态，防止下一次重复进入
        }
    }
    else if (cmd == 0x80 ||
             cmd == 0x00)
    {
        // 左边电机、幻彩灯打开了就关闭左边的，右边电机、幻彩灯打开了就关闭右边的

        // 先关闭灯光，再转动电机
        colorful_light_ctl.left_light_enable = 0;
        colorful_light_ctl.right_light_enable = 0;
        led_left_pwr_off();
        led_right_pwr_off();

        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_LEFT_ON);
        // user_delay_ctx_cancel(USER_DELAY_CTX_LED_RIGHT_ON);
        // user_delay_ctx_set(USER_DELAY_CTX_LED_LEFT_OFF, 50);
        // user_delay_ctx_set(USER_DELAY_CTX_LED_RIGHT_OFF, 50);

        // 发送反转的控制命令，单片机收到后，执行反转的操作，电机过流或执行超时之后就会停止
        // uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_REVERSE);
        // uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_REVERSE);

        if (motor_0_status != MOTOR_STATUS_REVERSE &&
            motor_0_status != MOTOR_STATUS_REVERSE_STOP)
        {
            motor_0_status = MOTOR_STATUS_REVERSE; // 立即设置状态，防止下一次重复进入

            // user_delay_ctx_set(USER_DELAY_CTX_MOTOR_LEFT_OFF, 60);

            uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_REVERSE);
        }

        if (motor_1_status != MOTOR_STATUS_REVERSE &&
            motor_1_status != MOTOR_STATUS_REVERSE_STOP)
        {
            motor_1_status = MOTOR_STATUS_REVERSE; // 立即设置状态，防止下一次重复进入

            // user_delay_ctx_set(USER_DELAY_CTX_MOTOR_RIGHT_OFF, 60);

            uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_REVERSE);
        }
    }
}

// 串口发送指令，让两个电机都反转
// void uart_send_motor_reverse(void)
// {
//     uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_REVERSE);
//     uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_REVERSE);
// }

void uart_send_motor_left_reverse(void)
{
    uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_REVERSE);
}

void uart_send_motor_right_reverse(void)
{
    uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_REVERSE);
}

// 处理蓝牙服务发送过来的数据
void ble_user_server_message_deal(u8 *buff, u16 len)
{
    if (len < 6)
    {
        return;
    }

    if (buff[0] != 0x04 ||
        buff[1] != 0x01 ||
        buff[2] != 0x1E)
    {
        // 指令的前缀不一致，直接返回
        return;
    }

    // buff[3]、buff[4]、buff[5]分别对应R、G、B的数值
    user_data.color = (buff[3] << 16) | (buff[4] << 8) | buff[5];
    // 隔一段时间才保存
    user_delay_ctx_set(USER_DELAY_CTX_SAVE_DATA, USER_DATA_SAVE_INTERVAL_MS);
    colorful_light_set_static_color(user_data.color);
}

// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
// 会循环调用，所以该函数内部不用写 while(1)
void user_main(void)
{
    uart_data_handle();
    can_handle();
    user_delay_ctx_handle();
    user_ws2812_service();

    // delay_ms(10);

    // my_printf("user_main\n");
}