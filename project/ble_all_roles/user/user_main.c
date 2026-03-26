#include "user_config.h"
#include "user_uart.h"
#include "SIT2515.h"
#include "driver_uart.h"
// #include "driver_gpio.h"

#include "uart_handle.h"
// #include "driver_gpio.h" // 初始化 测试时使用的gpio，需要引用该头文件

volatile user_data_t user_data = {0};

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

#if USER_DEBUG_ENABLE
    // user_debug_io_init();
#endif

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

    colorful_light_ctl.left_light_enable = 0;
    colorful_light_ctl.right_light_enable = 0;
    colorful_light_set_static_color(user_data.color);
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
    u8 buf[8] = {0};
    u8 ret = 0;
    u8 i;
    ret = CAN_Receive_Buffer(buf);
    if (ret == 0)
    {
        return;
    }

    if (ret != 6)
    {
// 如果这一帧数据的长度不一致，不处理这一帧数据
#if USER_DEBUG_ENABLE
        my_printf("can frame len err\n");
#endif
        return;
    }

    if (!(buf[0] == 0x00 &&
          buf[1] == 0x00 &&
          buf[2] == 0x01 &&
          buf[3] == 0x2A &&
          buf[4] == 0xD2))
    {
// 如果数据的前缀不一致，直接返回，不处理这一帧数据
#if USER_DEBUG_ENABLE
        my_printf("can prefix err\n");
#endif
        return;
    }

    switch (buf[5])
    {
    case 0xAB:
    case 0xBA: // 0xAB 0xBA 都是同一个功能
// 打开左边电机、打开左边幻彩灯
#if USER_DEBUG_ENABLE
        my_printf("left open\n");
#endif
        uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_FORWARD);
        break;

    case 0xAE:
    case 0xEA: // 0xAE 0xEA 都是同一个功能
// 打开右边电机、打开右边幻彩灯
#if USER_DEBUG_ENABLE
        my_printf("right open\n");
#endif
        uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_FORWARD);
        break;

    case 0xAA: // 左边电机、幻彩灯打开了就关闭左边的，右边电机、幻彩灯打开了就关闭右边的
#if USER_DEBUG_ENABLE
        my_printf("close\n");
#endif

        // 发送反转的控制命令，单片机收到后，执行反转的操作，电机过流或执行超时之后就会停止
        uart_send_cmd(MOTOR_INDEX_LEFT, MOTOR_CMD_REVERSE);
        uart_send_cmd(MOTOR_INDEX_RIGHT, MOTOR_CMD_REVERSE);
        break;
    }
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
    // 隔一段时间才保存，每次收到有效数据时，都重置这个时间
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

    // my_printf("user_main\n");
}