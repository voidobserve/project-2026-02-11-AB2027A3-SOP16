#include "include.h"
#include "driver_gpio.h"
#include "io_interrupt.h"
#include "driver_lowpwr.h"


#define IOKEY_PORT          GPIOA_REG
#define IOKEY_PIN           GPIO_PIN_7

AT(.com_text)
const char gpio_irq_prt[] = "WKUPEDG = %x,wkpnd = %x\n";
AT(.com_text)
const char gpio_irq_prt_pa7[] = "GPIO_PA7 wakeup!\n";
AT(.com_text)
const char gpio_irq_prt_pb1[] = "GPIO_PB1 wakeup!\n";
AT(.com_text)
const char gpio_irq_prt_pb2[] = "GPIO_PB2 wakeup!\n";
AT(.com_text)
const char gpio_irq_prt_pb3[] = "GPIO_PB3 wakeup!\n";
AT(.com_text)
const char gpio_irq_prt_pb4[] = "GPIO_PB4 wakeup!\n";

AT(.com_text.gpio_isr)
void gpio_isr_callback(void)
{
    uint8_t wkpnd;

    if (RTCCON9 & WK_LP_GPIO) {
        wkpnd = lowpwr_sleep_get_io_wksrc() >> 16;
    } else {
        wkpnd = WKUPEDG >> 16;
    }
    
    printf(gpio_irq_prt, WKUPEDG, wkpnd, lowpwr_sleep_get_io_wksrc(), RTCCON9);

    if (wkpnd & BIT(WAKEUP_IDX_PA7)) {
        printf(gpio_irq_prt_pa7);
    }
    
    if (wkpnd & BIT(WAKEUP_IDX_PB1)) {
        printf(gpio_irq_prt_pb1);
    }
    
    if (wkpnd & BIT(WAKEUP_IDX_PB2)) {
        printf(gpio_irq_prt_pb2);
    }
    
    if (wkpnd & BIT(WAKEUP_IDX_PB3)) {
        printf(gpio_irq_prt_pb3);
    }
    if (wkpnd & BIT(WAKEUP_IDX_PB4)) {
        printf(gpio_irq_prt_pb4);
    }

    gpio_edge_pending_clear();                      //clear wakeup pending
}

void io_interrupt_init(void)
{
    gpio_edge_cap_typedef config;

    config.edge = GPIO_EDGE_FALLING;
    config.gpiox = IOKEY_PORT;
    config.gpio_pin = IOKEY_PIN;
    config.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_edge_capture_config(&config);

    config.edge = GPIO_EDGE_FALLING;
    config.gpiox = GPIOB_REG;
    config.gpio_pin = GPIO_PIN_1;
    config.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_edge_capture_config(&config);

    gpio_edge_pic_config(gpio_isr_callback, 0);
}
