#ifndef _TFT_H
#define _TFT_H

#include "tft_spi.h"


void tft_set_window(u16 x0, u16 y0, u16 x1, u16 y1);
uint32_t tft_read_id(void);

void tft_80_st7735_init(void);
void tft_172_320_init(void);

void tft_init(void);
void tft_exit(void);

/**
 * @brief 软件模拟触发TE信号, 刷新gui
 **/
void tft_soft_te_tick(void);



/**
 * @brief 设置背光亮度
 * @param[in] level       亮度等级，使用无极调节时，范围0 ~ 100，否则为1~5
 * @param[in] stepless_en 是否使用无极调节
 *
 * @return  无
 **/
void tft_bglight_set_level(uint8_t level, bool stepless_en);

/**
 * @brief 首次设置亮度检测
 * @param 无
 *
 * @return  无
 **/
void tft_bglight_frist_set_check(void);

u8 tft_spi_getbyte(void);
void tft_spi_sendbyte(u8 val);
void tft_spi_send(void *buf, uint len);

void tft_frame_start(void);
void tft_frame_end(void);
void tft_write_data_start(void);
void tft_write_end();
#endif
