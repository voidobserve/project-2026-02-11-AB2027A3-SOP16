#include "include.h"
#include "bsp_io_key.h"

#if BSP_IOKEY_EN

void bsp_io_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

#if IO_KEY_WK0_EN
    /* KEY0(WK0/PP) Init */
    wko_io_config(ENABLE, DISABLE, ENABLE);
#endif

    /* KEY1 Init */
    if (IOKEY_KEY1_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY1_PIN;
        gpio_init(IOKEY_KEY1_PORT, &gpio_init_structure);
    }

    /* KEY2 Init */
    if (IOKEY_KEY2_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY2_PIN;
        gpio_init(IOKEY_KEY2_PORT, &gpio_init_structure);
    }

    /* KEY3 Init */
    if (IOKEY_KEY3_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY3_PIN;
        gpio_init(IOKEY_KEY3_PORT, &gpio_init_structure);
    }

    /* KEY4 Init */
    if (IOKEY_KEY4_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY4_PIN;
        gpio_init(IOKEY_KEY4_PORT, &gpio_init_structure);
    }

    /* KEY5 Init */
    if (IOKEY_KEY5_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY5_PIN;
        gpio_init(IOKEY_KEY5_PORT, &gpio_init_structure);
    }

    /* KEY6 Init */
    if (IOKEY_KEY6_PORT != NULL) {
        gpio_init_structure.gpio_pin = IOKEY_KEY6_PIN;
        gpio_init(IOKEY_KEY6_PORT, &gpio_init_structure);
    }

}

AT(.com_text.key.scan)
u8 bsp_get_io_key_id(void)
{
#if IO_KEY_WK0_EN
    if (wko_io_read_bit() == RESET) {                                   // KEY0(WK0/PP)
        return KEY_ID_PP;
    }
#endif
    if ((IOKEY_KEY1_PORT != NULL) && (gpio_read_bit(IOKEY_KEY1_PORT, IOKEY_KEY1_PIN) == RESET)) {
        return KEY_ID_K1;
    } else if ((IOKEY_KEY2_PORT != NULL) && (gpio_read_bit(IOKEY_KEY2_PORT, IOKEY_KEY2_PIN) == RESET)) {
        return KEY_ID_K2;
    } else if ((IOKEY_KEY3_PORT != NULL) && (gpio_read_bit(IOKEY_KEY3_PORT, IOKEY_KEY3_PIN) == RESET)) {
        return KEY_ID_K3;
    } else if ((IOKEY_KEY4_PORT != NULL) && (gpio_read_bit(IOKEY_KEY4_PORT, IOKEY_KEY4_PIN) == RESET)) {
        return KEY_ID_K4;
    } else if ((IOKEY_KEY5_PORT != NULL) && (gpio_read_bit(IOKEY_KEY5_PORT, IOKEY_KEY5_PIN) == RESET)) {
        return KEY_ID_K5;
    } else if ((IOKEY_KEY6_PORT != NULL) && (gpio_read_bit(IOKEY_KEY6_PORT, IOKEY_KEY6_PIN) == RESET)) {
        return KEY_ID_K6;
    }

    return KEY_ID_NO;
}

#endif // BSP_IOKEY_EN
