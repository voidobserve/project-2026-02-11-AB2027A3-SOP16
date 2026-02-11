#include "driver_ledc.h"
#include "driver_gpio.h"
#include "WS2815.h"

#define LEDC_OUT_PORT           GPIOA_REG
#define LEDC_OUT_PIN            GPIO_PIN_0

#define COLOR_FRAME_LENGTH      8
#define DMA_LENGTH              (COLOR_FRAME_LENGTH - COLOR_FRAME_LENGTH / 4)
#define SINGLE_COLOR_FILL       0x00ff1234  // WRGB

/**
 * custom buf, WGRB format:
 * bit[31:24]: W
 * bit[23:16]: G
 * bit[15:8]:  R
 * bit[7:0]:   B
 */
u32 color_buf[COLOR_FRAME_LENGTH];

/**
 * LEDC dma buf. For WS2815, it's 24bits for each color unit
 * BYTE[0]: B
 * BYTE[1]: R
 * BYTE[2]: G
 * BYTE[3]: B of the next color unit
 * BYTE[4]: R of the next color unit
 * ......
 */
u32 dma_buf[DMA_LENGTH];


AT(.com_text.isr)
void ledc_irq_handler(void)
{
    if (ledc_get_flag(LEDC_FLAG_DONE)) {
        ledc_clear_flag(LEDC_FLAG_DONE);
    }

    if (ledc_get_flag(LEDC_FLAG_RST)) {
        ledc_clear_flag(LEDC_FLAG_RST);
        ledc_dma_kick((u32)dma_buf, DMA_LENGTH);
        ledc_kick();
    }

    if (ledc_get_flag(LEDC_FLAG_DMA)) {
        ledc_clear_flag(LEDC_FLAG_DMA);
    }
}

u8 ws2815_byte_reversed(u8 data)
{
	data = ((data & 0xaa) >> 1) | ((data & 0x55) << 1);
	data = ((data & 0xcc) >> 2) | ((data & 0x33) << 2);
	data = (data >> 4) | (data << 4);
    return data;
}

u32 ws2815_WRGB_to_GRB(u32 color)
{
    color = ((color & 0x0000ff00) << 8) |
            ((color & 0x00ff0000) >> 8) |
            ((color & 0x000000ff));
    return color;
}

void ws2815_fill_dma_buffer(u32 *src, u32 *dest)
{
    u16 i;
    u8 *p_dest, *p_src;

    for (i = 0; i < COLOR_FRAME_LENGTH; i++) {
        color_buf[i] = ws2815_WRGB_to_GRB(SINGLE_COLOR_FILL);
    }

    p_src = (u8 *)src;
    p_dest = (u8 *)dest;

    for (i = 0; i < COLOR_FRAME_LENGTH * 4; i++) {
        if ((i + 1) % 4 == 0) {
            p_src++;
            continue;               // skip W of WRGB, WS2815 only support RGB
        }
        *p_dest = ws2815_byte_reversed(*p_src);
        p_src++;
        p_dest++;
    }
}

void ws2815_example_init(void)
{
    gpio_init_typedef gpio_init_struct;
    ledc_init_typedef ledc_init_struct;
    ledc_timing_init_typedef ledc_timing_init_struct;

    /* Config and enable clock */
    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_EN);
    clk_ledc_clk_set(CLK_LEDC_CLK_X24M_CLKDIV2);

    /* GPIO Init */
    gpio_init_struct.gpio_pin = LEDC_OUT_PIN;
    gpio_init_struct.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen = GPIO_FEN_PER;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_struct.gpio_drv = GPIO_DRV_6MA;
    gpio_init(LEDC_OUT_PORT, &gpio_init_struct);

    /* Config GPIO Mapping */
    gpio_func_mapping_config(LEDC_OUT_PORT, LEDC_OUT_PIN, GPIO_CROSSBAR_OUT_LEDCDAT);

    /* LEDC Base Ctrl Init */
    ledc_init_struct.output_invert = LEDC_OUT_INV_DIS;
    ledc_init_struct.format = LEDC_IN_FORMAT_RGB;
    ledc_init_struct.idle_level = LEDC_IDLE_LEVEL_LOW;
    ledc_init_struct.valid_data_mode = LEDC_VALID_DATA_DIR;
    ledc_init(&ledc_init_struct);

    /* LEDC Timing Init */
    ledc_timing_init_struct.baud = 15 - 1;      // period = 1 / (clk_12M / 15) = 1.25us
    ledc_timing_init_struct.delay = 0;
    ledc_timing_init_struct.code_0_high = 4;    // 1.25us / 15 * 4 = 333ns
    ledc_timing_init_struct.code_1_high = 11;   // 1.25us / 15 * 11 = 917ns
    ledc_timing_init_struct.reset_high = 20;
    ledc_timing_init_struct.reset_low = 300;
    ledc_timing_init(&ledc_timing_init_struct);

    /* Register interrupt, this interrupt source is not used by the module alone */
    /* Adjust the interrupt processing logic of related modules properly */
    ledc_pic_config(ledc_irq_handler, 0, LEDC_FLAG_RST | LEDC_FLAG_DONE, ENABLE);

    /* Initialize procedure parameters */
    ledc_set_frame_length(COLOR_FRAME_LENGTH);
    ledc_set_loop_number(0);

    /* Startup module, make sure to enable the module before starting DMA */
    ledc_cmd(ENABLE);
}

void ws2815_example_loop(void)
{
    ws2815_fill_dma_buffer(color_buf, dma_buf);
    ledc_dma_kick((u32)dma_buf, DMA_LENGTH);
    ledc_kick();

    while (1) {
    }
}
