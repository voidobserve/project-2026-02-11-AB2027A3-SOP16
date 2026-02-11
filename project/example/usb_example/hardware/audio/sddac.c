#include "include.h"
#include "driver_gpio.h"
#include "driver_sddac.h"
#include "sddac.h"
#include "usb_audio.h"

#if USB_SPEAKER_EN

#define SDDAC_DMA_SIZE              16


u16 sddac_buf[SDDAC_DMA_SIZE + 1] AT(.user_buf.sddac);


AT(.com_text.isr.sddac)
void sddac_isr(void)
{
    if (sddac_get_flag(SDDAC_IT_ALL_DONE)) {
        u32 len;
        usb_audio_speaker_stream(sddac_buf, &len);
        sddac_dma_kick_start((u8 *)sddac_buf, len);
        sddac_clear_flag(SDDAC_IT_ALL_DONE);
    }
}

void bsp_sddac_init(void)
{
    gpio_init_typedef gpio_init_structure;
    sddac_init_typedef sddac_config;

    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_EN);

    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIOA_REG, &gpio_init_structure);

    FUNO_PA2SEL(FO_AUDIO);                                    //Map dac output PA2

    memset((u8 *)sddac_buf, 0x00, sizeof(sddac_buf));

    sddac_config.dma_buf = (u8 *)sddac_buf;
    sddac_config.dma_size = sizeof(sddac_buf) / 2;
    sddac_config.sample_rate = SDDAC_NORMAL_SPR_16k;
    sddac_init(&sddac_config);

    sddac_pic_config(sddac_isr, 0, SDDAC_IT_ALL_DONE, ENABLE);

    //sddac_cmd(ENABLE);
}
#endif
