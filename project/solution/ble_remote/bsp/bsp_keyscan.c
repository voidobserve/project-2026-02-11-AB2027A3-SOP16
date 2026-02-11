#include "include.h"
#include "bsp_keyscan.h"
#include "driver_keyscan.h"
#include "driver_gpio.h"

#if BSP_KEYSCAN_EN

struct{
    uint32_t key_valA;
    uint32_t key_valB;
}keyscan_cb;

AT(.com_text.keyscan.isr)
void keyscan_isr_handler(void)
{
    if (keyscan_get_flag(KEYSCAN_IT_VALID) != RESET) {
        keyscan_cb.key_valA = keyscan_get_groupA_key_id();
        keyscan_cb.key_valB = keyscan_get_groupB_key_id();
        keyscan_clear_flag(KEYSCAN_IT_VALID);
#if SYS_SLEEP_EN
#if LE_REMOTE_HOLD_KEY_LOWPOWER_EN
    if (!sys_cb.sleep_keymsg_2_iopin.key_msg)
#endif  //LE_REMOTE_HOLD_KEY_LOWPOWER_EN
    {
        if(sys_cb.sleep_sta != LOWPWR_SLEEP_EXIT){
            sys_cb.sleep_prevent = true;
        }
        lowpwr_sleep_delay_reset();
    }
#endif
    }

    if (keyscan_get_flag(KEYSCAN_IT_INVALID) != RESET) {
        keyscan_cb.key_valA = 0;
        keyscan_cb.key_valB = 0;
        keyscan_clear_flag(KEYSCAN_IT_INVALID);
    }
}

void bsp_keyscan_init(void)
{
    gpio_init_typedef gpio_init_struct;
    keyscan_init_typedef keyscan_init_struct;

    memset(&keyscan_cb, 0, sizeof(keyscan_cb));

    clk_gate0_cmd(CLK_GATE0_LP, CLK_EN);
    clk_clk2m_ks_set(CLK_CLK2M_KS_X24MDIV12_RTC);

    gpio_init_struct.gpio_pin = GPIO_PIN_ALL;
    gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_struct.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_struct.gpio_pupd = GPIO_PUPD_PD200K;
	gpio_init(GPIOA_REG, &gpio_init_struct);

    //参考ab202x_sdk开发指南 键盘扫描（keyscan） 章节的配置说明
    keyscan_init_struct.key_mask = 0xff;        //PA0~PA7
    keyscan_init_struct.stable_times = 255;
    keyscan_init_struct.debounce_times = 1;
    keyscan_init_struct.wait_times = 1;
    keyscan_init_struct.key_mode = KEYSCAN_MODE_NORMAL;
    keyscan_init_struct.sw_reset = KEYSCAN_SWRSTN_RELEASE_RESET;
    keyscan_init_struct.low_power_mode = KEYSCAN_LOWPWR_EN;  //Use lowpower dedicated circuits
    keyscan_init_struct.wakeup = KEYSCAN_WKUP_EN;
    keyscan_init(&keyscan_init_struct);

    keyscan_pic_config(keyscan_isr_handler, 0, KEYSCAN_IT_VALID | KEYSCAN_IT_INVALID, ENABLE);

    keyscan_cmd(ENABLE);
}


AT(.com_text.key_scan)
uint8_t bsp_get_keyscan_id(void)
{
    uint8_t key_id = KEY_ID_NO;

    // static char key_val_info[] = "A:%x, B:%x\n";
    // if (keyscan_cb.key_valA || keyscan_cb.key_valB) {
    //     printf(key_val_info, keyscan_cb.key_valA, keyscan_cb.key_valB);
    // }

    switch (keyscan_cb.key_valA) {
        case KA1:
            key_id = KEY_ID_K1;
            break;
        case KA2:
            key_id = KEY_ID_K2;
            break;
        case KA3:
            key_id = KEY_ID_K3;
            break;
        case KA4:
            key_id = KEY_ID_K4;
            break;
        case KA5:
            key_id = KEY_ID_K5;
            break;
        case KA6:
            key_id = KEY_ID_K6;
            break;
        case KB2:
            key_id = KEY_ID_K7;
        break;
        // case 0x2000030:
        //     //KEYSCAN_MODE_MATRIX 组合按键, KA5 + KA6
        //     key_id = KEY_ID_K13;                                                                                  //
        // break;
        case 0x0000030:
            //KEYSCAN_MODE_NORMAL 组合按键, KA5 + KA6
            key_id = KEY_ID_K13;                                                                                  //
        break;

        default:
            break;
    }

    switch (keyscan_cb.key_valB) {
        case KI1:
            key_id = KEY_ID_K8;
            break;
        case KI2:
            key_id = KEY_ID_K9;
            break;
        case KI3:
            key_id = KEY_ID_K10;
            break;
        case KI4:
            key_id = KEY_ID_K11;
            break;
        case KI5:
            key_id = KEY_ID_K12;
            break;
        default:
            break;
    }

    return key_id;
}

#endif // BSP_SCANKEY_EN
