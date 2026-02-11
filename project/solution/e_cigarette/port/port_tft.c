#include "include.h"
#include "port_tft.h"


//TFT IO INIT
void port_tft_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = TFT_SPI_CS_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(TFT_SPI_CS_PORT, &gpio_init_structure);       //CS
    gpio_set_bits(TFT_SPI_CS_PORT, TFT_SPI_CS_PIN);

    gpio_init_structure.gpio_pin = TFT_SPI_RS_PIN;
    gpio_init(TFT_SPI_RS_PORT, &gpio_init_structure);       //RS
    gpio_set_bits(TFT_SPI_RS_PORT, TFT_SPI_RS_PIN);

    gpio_init_structure.gpio_pin = TFT_SPI_EN_PIN;
    gpio_init(TFT_SPI_EN_PORT, &gpio_init_structure);       //EN
    gpio_set_bits(TFT_SPI_EN_PORT, TFT_SPI_EN_PIN);

    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_pin = TFT_SPI_CLK_PIN;
    gpio_init(TFT_SPI_CLK_PORT, &gpio_init_structure);      //CLK
    gpio_reset_bits(TFT_SPI_CLK_PORT, TFT_SPI_CLK_PIN);

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pin = TFT_SPI_DATA_PIN;
    gpio_init(TFT_SPI_DATA_PORT, &gpio_init_structure);     //DATA
    gpio_reset_bits(TFT_SPI_DATA_PORT, TFT_SPI_DATA_PIN);

    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_pin = TFT_SPI_BL_PIN;
    gpio_init(TFT_SPI_BL_PORT, &gpio_init_structure);       //BL time3 pwm

    gpio_func_mapping_config(TFT_SPI_CLK_PORT, TFT_SPI_CLK_PIN, GPIO_CROSSBAR_OUT_SPI1CLK);
    gpio_func_mapping_config(TFT_SPI_DATA_PORT, TFT_SPI_DATA_PIN, GPIO_CROSSBAR_OUT_SPI1D0);
    gpio_func_mapping_config(TFT_SPI_BL_PORT, TFT_SPI_BL_PIN, GPIO_CROSSBAR_OUT_T3PWM0);
}

//TFT IO RESET
void port_tft_reset(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = TFT_SPI_RST_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(TFT_SPI_RST_PORT, &gpio_init_structure);       //RESET
    gpio_set_bits(TFT_SPI_RST_PORT, TFT_SPI_RST_PIN);
    delay_ms(10);
    gpio_reset_bits(TFT_SPI_RST_PORT, TFT_SPI_RST_PIN);
    delay_ms(20);
    gpio_set_bits(TFT_SPI_RST_PORT, TFT_SPI_RST_PIN);
    delay_ms(50);
}

//TFT IO DEINIT
void port_tft_exit(void)
{
    gpio_deinit(TFT_SPI_CS_PORT, TFT_SPI_CS_PIN);
    gpio_deinit(TFT_SPI_RS_PORT, TFT_SPI_RS_PIN);
    gpio_deinit(TFT_SPI_EN_PORT, TFT_SPI_EN_PIN);
    gpio_deinit(TFT_SPI_CLK_PORT, TFT_SPI_CLK_PIN);
    gpio_deinit(TFT_SPI_DATA_PORT, TFT_SPI_DATA_PIN);
    gpio_deinit(TFT_SPI_BL_PORT, TFT_SPI_BL_PIN); 
}
