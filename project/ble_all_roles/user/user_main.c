#include "user_config.h"
#include "user_uart.h"
#include "SIT2515.h"

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

    my_printf("save.color == 0x%lx\n", user_data.color);
}

// 初始化函数，在 bsp_sys.c -> bsp_sys_init() 中调用
void user_init(void)
{

#if WS2812_LIB_EN
    WS2812FX_init(WS2812_LED_NUM_MAX, WS2812_NEO_TYPE);
    WS2812FX_setBrightness(255);

    // 设置上电默认样式
    uws2812_style_powerup_default();

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
    user_debug_io_init();
#endif

    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_9;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOB_REG, &gpio_init_structure);
    gpio_set_bits(GPIOB_REG, GPIO_PIN_9); 

    SIT2515_SPICS_Init(); // 初始化片选脚
    SIT2515_Init();

    user_data_read(); //
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

void can_test(void)
{
    u8 buf[8] = {0};
    u8 ret = 0;
    u8 i;
    ret = CAN_Receive_Buffer(buf);
    if (ret == 0)
    {
        return;
    }

    for (i = 0; i < ret; i++)
    {
        my_printf("buf[%02d] == %02x\n", (u16)i, buf[i]);
    }
}

// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
// 会循环调用，所以该函数内部不用写 while(1)
void user_main(void)
{
    uart_data_handle();
    can_test(); 
}