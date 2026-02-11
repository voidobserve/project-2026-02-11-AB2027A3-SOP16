#include "include.h"

#include "driver_gpio.h"
#include "port_ctp.h"

// 硬件IIC初始化
void port_ctp_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = PORT_CTP_RST_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(PORT_CTP_RST_PORT, &gpio_init_structure);       //RESET


    gpio_init_structure.gpio_pin = PORT_CTP_SCL_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(PORT_CTP_SCL_PORT, &gpio_init_structure);      //CLK

    gpio_init_structure.gpio_pin = PORT_CTP_SDA_PIN;
    gpio_init(PORT_CTP_SDA_PORT, &gpio_init_structure);      //DATA

    gpio_func_mapping_config(PORT_CTP_SDA_PORT, PORT_CTP_SDA_PIN, GPIO_CROSSBAR_OUT_I2CSDA);
    gpio_func_mapping_config(PORT_CTP_SDA_PORT, PORT_CTP_SDA_PIN, GPIO_CROSSBAR_IN_I2CSDA);

    gpio_func_mapping_config(PORT_CTP_SCL_PORT, PORT_CTP_SCL_PIN, GPIO_CROSSBAR_OUT_I2CSCL);
    gpio_func_mapping_config(PORT_CTP_SCL_PORT, PORT_CTP_SCL_PIN, GPIO_CROSSBAR_IN_I2CSCL);

    gpio_edge_cap_typedef config;
    config.edge = GPIO_EDGE_FALLING;
    config.gpiox = PORT_CTP_INT_PORT;
    config.gpio_pin = PORT_CTP_INT_PIN;
    config.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_edge_capture_config(&config);                       //INT
}

//复位
void port_ctp_reset(void)
{
    PORT_CTP_RST_L();
    delay_5ms(2);
    PORT_CTP_RST_H();
    delay_5ms(21);
}


void port_ctp_exit(void)
{
    gpio_deinit(PORT_CTP_RST_PORT, PORT_CTP_RST_PIN); 
    gpio_deinit(PORT_CTP_SCL_PORT, PORT_CTP_SCL_PIN); 
    gpio_deinit(PORT_CTP_SDA_PORT, PORT_CTP_SDA_PIN); 
}