#include "key_scan.h"

#define KI7_KEY_DOWN_EVENT      0x01
#define KA7_KEY_DOWN_EVENT      0x02
#define ALL_KEY_RELEASE_EVENT   0x80

u8 custom_event = 0;

#if INTERRUPT_EN
AT(.com_text.isr)
void keyscan_isr_handler(void)
{
    uint32_t key_val1;
    uint32_t key_val2;

    if (keyscan_get_flag(KEYSCAN_IT_VALID) != RESET) {
        key_val1 = keyscan_get_groupA_key_id();
        key_val2 = keyscan_get_groupB_key_id();
        if (key_val2 & KI7) {
            custom_event |= KI7_KEY_DOWN_EVENT;
        }
        if (key_val1 & KA7) {
            custom_event |= KA7_KEY_DOWN_EVENT;
        }
        keyscan_clear_flag(KEYSCAN_IT_VALID);
    }

    if (keyscan_get_flag(KEYSCAN_IT_INVALID) != RESET) {
        custom_event |= ALL_KEY_RELEASE_EVENT;
        keyscan_clear_flag(KEYSCAN_IT_INVALID);
    }
}
#endif // INTERRUPT_EN

void key_scan_init(void)
{
    gpio_init_typedef gpio_init_struct;
    keyscan_init_typedef keyscan_init_struct;

    clk_gate0_cmd(CLK_GATE0_LP, CLK_EN);
    clk_clk2m_ks_set(CLK_CLK2M_KS_X24MDIV12_RTC);

    gpio_init_struct.gpio_pin = GPIO_PIN_ALL;
    gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_struct.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_struct.gpio_pupd = GPIO_PUPD_PD200K;
	gpio_init(GPIOA_REG, &gpio_init_struct);

    keyscan_init_struct.key_mask = 0xff;        // PA0~PA7
    keyscan_init_struct.stable_times = 255;
    keyscan_init_struct.debounce_times = 1;  
    keyscan_init_struct.wait_times = 1;      
    keyscan_init_struct.key_mode = KEYSCAN_MODE_NORMAL;
    keyscan_init_struct.sw_reset = KEYSCAN_SWRSTN_RELEASE_RESET;
    keyscan_init_struct.low_power_mode = KEYSCAN_LOWPWR_EN;  //Use lowpower dedicated circuits
    keyscan_init_struct.wakeup = KEYSCAN_WKUP_EN;

    keyscan_init(&keyscan_init_struct);

#if INTERRUPT_EN
    keyscan_pic_config(keyscan_isr_handler, 0, KEYSCAN_IT_VALID | KEYSCAN_IT_INVALID, ENABLE);
#endif // INTERRUPT_EN

    keyscan_cmd(ENABLE);
}

#if INTERRUPT_EN
static void key_event_handler(void)
{
    if (custom_event & KI7_KEY_DOWN_EVENT) {
        printf("KI7 Down\n");
        custom_event &= ~KI7_KEY_DOWN_EVENT;
    }

    if (custom_event & KA7_KEY_DOWN_EVENT) {
        printf("KA7 Down\n");
        custom_event &= ~KA7_KEY_DOWN_EVENT;
    }

    if (custom_event & ALL_KEY_RELEASE_EVENT) {
        printf("All Key Release\n");
        custom_event &= ~ALL_KEY_RELEASE_EVENT;
    }
}
#endif // INTERRUPT_EN

void key_scan_example(void)
{
#if INTERRUPT_EN
    if (custom_event) {
        key_event_handler();
    }
#else
    if (keyscan_get_flag(KEYSCAN_IT_VALID) != RESET) {
        printf("---> GroupA Key: %X\n", keyscan_get_groupA_key_id());
        printf("---> GroupB Key: %X\n", keyscan_get_groupB_key_id());
        keyscan_clear_flag(KEYSCAN_IT_VALID);
    }

    if (keyscan_get_flag(KEYSCAN_IT_INVALID) != RESET) {
        printf("---> All Key Release\n");
        keyscan_clear_flag(KEYSCAN_IT_INVALID);
    }
#endif // INTERRUPT_EN
}
