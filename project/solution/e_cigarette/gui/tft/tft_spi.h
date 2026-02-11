#ifndef __TFT_SPI_H__
#define __TFT_SPI_H__

void tft_write_cmd(u8 cmd);
void tft_write_data(u8 data);
void tft_write_end();
void tft_spi_isr(void);

#endif // __TFT_SPI_H__
