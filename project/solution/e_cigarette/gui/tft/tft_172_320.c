#include "include.h"

#if (GUI_SELECT == GUI_TFT_172_320)

void tft_172_320_init(void)
{
    printf("%s\n", __func__);

    tft_write_cmd(0xfe);
    tft_write_cmd(0xef);
    tft_write_cmd(0x36);
    tft_write_data(0x48);
    tft_write_cmd(0x3a);
    tft_write_data(0x05);

    tft_write_cmd(0x86);
    tft_write_data(0x98);
    tft_write_cmd(0x89);
    tft_write_data(0x03);
    tft_write_cmd(0x8b);
    tft_write_data(0x80);
    tft_write_cmd(0x8d);
    tft_write_data(0x33);
    tft_write_cmd(0x8e);
    tft_write_data(0x0f);

    tft_write_cmd(0xe8);
    tft_write_data(0x12);
    tft_write_data(0x00);

    tft_write_cmd(0xc3);
    tft_write_data(0x31);
    tft_write_cmd(0xc4);
    tft_write_data(0x00);
    tft_write_cmd(0xc9);
    tft_write_data(0x0f);

    tft_write_cmd(0xff);
    tft_write_data(0x62);

    tft_write_cmd(0x99);
    tft_write_data(0x3e);
    tft_write_cmd(0x9d);
    tft_write_data(0x4b);
    tft_write_cmd(0x98);
    tft_write_data(0x3e);
    tft_write_cmd(0x9c);
    tft_write_data(0x4b);

    tft_write_cmd(0xF0);
    tft_write_data(0x8a);//1_3f
    tft_write_data(0x0c);//2_3f
    tft_write_data(0x09);//4_1f
    tft_write_data(0x08);//6_1f
    tft_write_data(0x05);//0_f\13_f
    tft_write_data(0x2f);//20_7f

    tft_write_cmd(0xF2);
    tft_write_data(0x8a);//1_3f
    tft_write_data(0x0c);//2_3f
    tft_write_data(0x09);//4_1f
    tft_write_data(0x08);//6_1f
    tft_write_data(0x06);//0_f\13_f
    tft_write_data(0x2f);//20_7f

    tft_write_cmd(0xF1);
    tft_write_data(0x49);//43_7f
    tft_write_data(0x8f);//27\57
    tft_write_data(0x6f);//36\59
    tft_write_data(0x2C);//61
    tft_write_data(0x31);//62
    tft_write_data(0xff);//50_f\63_f

    tft_write_cmd(0xF3);
    tft_write_data(0x46);//43_7f
    tft_write_data(0x7f);//27\57
    tft_write_data(0x5f);//36\59
    tft_write_data(0x17);//61
    tft_write_data(0x19);//62
    tft_write_data(0xff);//50_f\63_f



    tft_write_cmd(0x11);
    delay_ms(120);
    tft_write_cmd(0x29);
    tft_write_cmd(0x2c);
}
#endif

