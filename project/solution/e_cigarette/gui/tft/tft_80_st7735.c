#include "include.h"

#if (GUI_SELECT == GUI_TFT_80_ST7735)

void tft_80_st7735_init(void)
{
    //ST7735S+HSD0.96
    printf("%s\n", __func__);

    tft_write_cmd(0x11);	 //sleep out
	delay_ms(120);

    tft_write_cmd(0xB1);    //不同颜色下屏幕的刷新率 B1 B2 B3
    tft_write_data(0x05);
    tft_write_data(0x3C);
    tft_write_data(0x3C);

    tft_write_cmd(0xB2);
    tft_write_data(0x05);
    tft_write_data(0x3C);
    tft_write_data(0x3C);

    tft_write_cmd(0xB3);
    tft_write_data(0x05);
    tft_write_data(0x3C);
    tft_write_data(0x3C);
    tft_write_data(0x05);
    tft_write_data(0x3C);
    tft_write_data(0x3C);

    tft_write_cmd(0xB4);    //B4 屏幕是否反色
    tft_write_data(0x03);
    tft_write_data(0x02);

    // tft_write_cmd(0x21);    //21 屏幕反色

    tft_write_cmd(0xC0);    //C0 C1 C2 C3 C4显示屏在不用颜色不同模式下的电压
    tft_write_data(0xE0);
    tft_write_data(0x00);
    tft_write_data(0x84);

    tft_write_cmd(0xC1);
    tft_write_data(0xC4);

    tft_write_cmd(0xC2);
    tft_write_data(0x0D);
    tft_write_data(0x00);

    tft_write_cmd(0xC3);
    tft_write_data(0x8D);
    tft_write_data(0x2A);

    tft_write_cmd(0xC4);
    tft_write_data(0x8D);
    tft_write_data(0xEE);

    tft_write_cmd(0xC5);    //VCOM
    tft_write_data(0x04);

    tft_write_cmd(0x36);   //显示方向：横屏/竖屏 颜色格式：RGB/BGR
    tft_write_data(0xC8);

    tft_write_cmd(0xE0);	 //配置伽马极性
    tft_write_data(0x09);
    tft_write_data(0x18);
    tft_write_data(0x1A);
    tft_write_data(0x21);
    tft_write_data(0x3F);
    tft_write_data(0x3B);
    tft_write_data(0x31);
    tft_write_data(0x32);
    tft_write_data(0x2F);
    tft_write_data(0x2A);
    tft_write_data(0x30);
    tft_write_data(0x37);
    tft_write_data(0x00);
    tft_write_data(0x05);
    tft_write_data(0x00);
    tft_write_data(0x10);

    tft_write_cmd(0xE1);	//配置伽马极性
    tft_write_data(0x07);
    tft_write_data(0x14);
    tft_write_data(0x15);
    tft_write_data(0x1A);
    tft_write_data(0x3F);
    tft_write_data(0x33);
    tft_write_data(0x2C);
    tft_write_data(0x2E);
    tft_write_data(0x2B);
    tft_write_data(0x28);
    tft_write_data(0x2F);
    tft_write_data(0x3E);
    tft_write_data(0x00);
    tft_write_data(0x04);
    tft_write_data(0x02);
    tft_write_data(0x10);

    tft_write_cmd(0x35);
    tft_write_data(0x00);

    tft_write_cmd(0x3A);	//像素格式
    tft_write_data(0x05); 	//05  16bit/pixel

    tft_write_cmd(0x29);	//进行显示
    tft_write_cmd(0x2C);
}
#endif

