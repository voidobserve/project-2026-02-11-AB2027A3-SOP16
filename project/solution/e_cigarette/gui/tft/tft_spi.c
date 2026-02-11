#include "include.h"
#include "driver_spi.h"

AT(.com_text.tft_spi)
void tft_spi_send(void *buf, uint len)
{
    spi_it_cmd(SPI1, DISABLE);                         //IE DIS
    spi_set_dma_addr(SPI1, (uint32_t)buf);
    spi_set_dma_cnt(SPI1, len);
    while(!spi_get_flag(SPI1, SPI_FLAG_PENDING));      //Wait pending
    spi_it_cmd(SPI1, ENABLE);                          //IE EN
}

AT(.com_text.tft_spi)
u8 tft_spi_getbyte(void)
{
    u8 val;
    spi_it_cmd(SPI1, DISABLE);                         //IE DIS
    spi_dir_sel(SPI1, SPI_DIR_RX);                     //RX
    spi_send_data(SPI1, 0xFF);
    while(!spi_get_flag(SPI1, SPI_FLAG_PENDING));      //Wait pending
    val = spi_receive_data(SPI1);
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);            //Clear pending
    spi_it_cmd(SPI1, ENABLE);                          //IE EN
    return val;
}

AT(.com_text.tft_spi)
void tft_spi_sendbyte(u8 val)
{
    spi_it_cmd(SPI1, DISABLE);                         //IE DIS
    spi_dir_sel(SPI1, SPI_DIR_TX);                     //TX
    spi_send_data(SPI1, val);
    while(!spi_get_flag(SPI1, SPI_FLAG_PENDING));      //Wait pending
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    spi_it_cmd(SPI1, ENABLE);                          //IE EN
}

AT(.com_text.tft_spi)
void tft_write_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    TFT_RS_CMD();
	tft_spi_sendbyte(cmd);
    TFT_RS_DATA();
}

AT(.com_text.tft_spi)
void tft_read_cmd(u8 cmd)
{
    tft_write_cmd(cmd);
}

AT(.com_text.tft_spi)
void tft_write_data(u8 data)
{
	tft_spi_sendbyte(data);
}

AT(.com_text.tft_spi)
void tft_write_end()
{
    TFT_SPI_CS_DIS();
}

AT(.com_text.tft_spi)
void tft_write_data_start(void)
{
    tft_write_cmd(0x2C);      //TFT_RAMWR
}

AT(.com_text.tft_spi)
void tft_spi_kick(void *buf, uint len)
{
    spi_set_dma_addr(SPI1, (uint32_t)buf);
    spi_set_dma_cnt(SPI1, len);
}

AT(.com_text.tft_spi)
void tft_spi_wait(void)
{
    thread_driver_sem_take();
}

AT(.com_text.tft_spi)
void tft_spi_isr(void)
{
    if (spi_get_flag(SPI1, SPI_FLAG_PENDING)) {
        spi_clear_flag(SPI1, SPI_FLAG_PENDING);
        thread_driver_sem_release();
    }
}

void tft_set_window(u16 x0, u16 y0, u16 x1, u16 y1)
{
    x0 += GUI_SCREEN_OFS_X;
    x1 += GUI_SCREEN_OFS_X;
    y0 += GUI_SCREEN_OFS_Y;
    y1 += GUI_SCREEN_OFS_Y;

    tft_write_cmd(0x2A);        //TFT_CASET
    tft_write_data(BYTE1(x0));
    tft_write_data(BYTE0(x0));
    tft_write_data(BYTE1(x1));
    tft_write_data(BYTE0(x1));

    tft_write_cmd(0x2B);        //TFT_PASET
    tft_write_data(BYTE1(y0));
    tft_write_data(BYTE0(y0));
    tft_write_data(BYTE1(y1));
    tft_write_data(BYTE0(y1));
    tft_write_end();
}

uint32_t tft_read_id(void)
{
    uint32_t id = 0;
    tft_read_cmd(0x04);
    id = tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    tft_write_end();
    return id;
}

#if 0 //纯色图推屏测试
//static u8 debug_tft_dma_buf[320*2];	//注意长度需要大于等于x1*2
void spi_display_point(uint16_t x1, uint16_t y1, uint16_t color)
{
    delay_5ms(200);
    LCD_BL_EN();

    tft_set_window(0, 0, x1, y1);
    tft_frame_start();

    printf("%s DESPICON[%x] sys_clk[%d]\n", __func__, SPI1CON, sys_clk_get());
#if 0//DMA buf
    u8 byte_len = 2;
    u8 dataH = color >> 8;
    u8 dataL = color & 0xff;

    printf("data_len[%d] = %d*%d*%d\n", x1*y1*byte_len, x1, y1, byte_len);
    for (int i = 0; i <= x1*byte_len; i += byte_len) {
        debug_tft_dma_buf[i + 1] = dataH;       //dataH => SDA
        debug_tft_dma_buf[i] = dataL;           //dataL => DC
    }

    for (int i = 0; i <= y1; i++) {
        WDT_CLR();
        tft_spi_send(debug_tft_dma_buf, x1*byte_len);
    }
#else
    for (int x = 0; x <= x1; x++) {
        for (int y = 0; y <= y1; y++) {
            WDT_CLR();
            tft_write_data(color >> 8);
            tft_write_data(color);
        }
    }
#endif
    tft_write_end();
    printf("%s done\n", __func__);
}
#endif


