// encoding UTF-8
//******************************
// SIT2515 驱动代码 仅供参考
/*
 * @file    SIT2515.c
 * @author  FAE Team
 * @version V1.0.3bat
 * @date    2-6-2023
 */
#include "include.h"
#include "SIT2515.h"
#include "driver_gpio.h"

#include "user_config.h"

volatile unsigned char CAN_R_Buffer[8];  // CAN接收数据保存缓冲区
volatile unsigned char CAN_R_RecNum = 0; // 接收数据个数

// #define STANDARD_ID 0x02E1
#define STANDARD_ID 0x012A
/**********************************************************************************
 * 文件名  ：SIT2515.c
 * 描述    ：SIT2515驱动
 **********************************************************************************/
#define SIT2515_CS_IO_PORT GPIOA_REG // CAN收发器片选IO对应的端口号
#define SIT1515_CS_IO_PIN GPIO_PIN_4 // CAN收发器片选引脚序号

#define SIT2515_CS_SET() gpio_set_bits(SIT2515_CS_IO_PORT, SIT1515_CS_IO_PIN)
#define SIT2515_CS_RESET() gpio_reset_bits(SIT2515_CS_IO_PORT, SIT1515_CS_IO_PIN)

#define SIT2515_CS_INIT()                                       \
    do                                                          \
    {                                                           \
        gpio_init_typedef gpio_init_structure;                  \
        gpio_init_structure.gpio_pin = SIT1515_CS_IO_PIN;       \
        gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;         \
        gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;           \
        gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;         \
        gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;      \
        gpio_init_structure.gpio_drv = GPIO_DRV_6MA;            \
        gpio_init(SIT2515_CS_IO_PORT, &gpio_init_structure);    \
        gpio_reset_bits(SIT2515_CS_IO_PORT, SIT1515_CS_IO_PIN); \
    } while (0)

// 定义 SPI GPIO 引脚映射
#define SPI_SCK_IO_PORT GPIOA_REG // SPI SCK IO对应的端口号
#define SPI_SCK_IO_PIN GPIO_PIN_3 // SPI SCK IO引脚序号
// #define SPI_SCK_PIN 6
// #define SPI_SCK_SET() GPIOASET = BIT(SPI_SCK_PIN)
// #define SPI_SCK_RESET() GPIOACLR = BIT(SPI_SCK_PIN)
// #define SIT2515_SCK_INIT()             \
//     {                                  \
//         GPIOADE |= BIT(SPI_SCK_PIN);   \
//         GPIOADIR &= ~BIT(SPI_SCK_PIN); \
//     }
#define SPI_SCK_SET() gpio_set_bits(SPI_SCK_IO_PORT, SPI_SCK_IO_PIN)
#define SPI_SCK_RESET() gpio_reset_bits(SPI_SCK_IO_PORT, SPI_SCK_IO_PIN)
#define SIT2515_SCK_INIT()                                 \
    do                                                     \
    {                                                      \
        gpio_init_typedef gpio_init_structure;             \
        gpio_init_structure.gpio_pin = SPI_SCK_IO_PIN;     \
        gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;    \
        gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;      \
        gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;    \
        gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL; \
        gpio_init_structure.gpio_drv = GPIO_DRV_6MA;       \
        gpio_init(SPI_SCK_IO_PORT, &gpio_init_structure);  \
        gpio_reset_bits(SPI_SCK_IO_PORT, SPI_SCK_IO_PIN);  \
    } while (0)

// #define SPI_MOSI_PIN 2
// #define SPI_MOSI_SET() GPIOFSET = BIT(SPI_MOSI_PIN)
// #define SPI_MOSI_RESET() GPIOFCLR = BIT(SPI_MOSI_PIN)
// #define SIT2515_MOSI_INIT()             \
//     {                                   \
//         GPIOFDE |= BIT(SPI_MOSI_PIN);   \
//         GPIOFDIR &= ~BIT(SPI_MOSI_PIN); \
//     }
#define SPI_MOSI_IO_PORT GPIOA_REG // SPI MOSI IO对应的端口号
#define SPI_MOSI_IO_PIN GPIO_PIN_2 // SPI MOSI IO引脚序号
#define SPI_MOSI_SET() gpio_set_bits(SPI_MOSI_IO_PORT, SPI_MOSI_IO_PIN)
#define SPI_MOSI_RESET() gpio_reset_bits(SPI_MOSI_IO_PORT, SPI_MOSI_IO_PIN)
#define SIT2515_MOSI_INIT()                                 \
    do                                                      \
    {                                                       \
        gpio_init_typedef gpio_init_structure;              \
        gpio_init_structure.gpio_pin = SPI_MOSI_IO_PIN;     \
        gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;     \
        gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;       \
        gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;     \
        gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;  \
        gpio_init_structure.gpio_drv = GPIO_DRV_6MA;        \
        gpio_init(SPI_MOSI_IO_PORT, &gpio_init_structure);  \
        gpio_reset_bits(SPI_MOSI_IO_PORT, SPI_MOSI_IO_PIN); \
    } while (0)

// #define SPI_MISO_PIN 5
// #define SIT2515_MISO_INIT()            \
//     {                                  \
//         GPIOADE |= BIT(SPI_MISO_PIN);  \
//         GPIOADIR |= BIT(SPI_MISO_PIN); \
//         GPIOAPU |= BIT(SPI_MISO_PIN);  \
//     }
// #define MISO_IS_H() (GPIOA & BIT(SPI_MISO_PIN))

// 输入上拉
#define SPI_MISO_IO_PORT GPIOA_REG // SPI MISO IO对应的端口号
#define SPI_MISO_IO_PIN GPIO_PIN_1 // SPI MISO IO引脚序号
#define SIT2515_MISO_INIT()                                 \
    do                                                      \
    {                                                       \
        gpio_init_typedef gpio_init_structure;              \
        gpio_init_structure.gpio_pin = SPI_MISO_IO_PIN;     \
        gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;      \
        gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;       \
        gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;     \
        gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;  \
        gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;    \
        gpio_init(SPI_MISO_IO_PORT, &gpio_init_structure);  \
        gpio_reset_bits(SPI_MISO_IO_PORT, SPI_MISO_IO_PIN); \
    } while (0)
#define MISO_IS_H() gpio_read_bit(SPI_MISO_IO_PORT, SPI_MISO_IO_PIN)

// #define SPI_MODE_CFG SPI_MODE0
#define SPI_MODE_CFG SPI_MODE1
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

// #define bsp_spi_delay() delay_us(1)
#define bsp_spi_delay() delay_us(5)

void SPI_MasterInit1(void)
{
    SIT2515_SCK_INIT();
    if ((SPI_MODE_CFG == SPI_MODE0) || (SPI_MODE_CFG == SPI_MODE1))
    {
        SPI_SCK_RESET();
    }
    else
    {
        SPI_SCK_RESET();
    }
    SIT2515_MOSI_INIT();
    SIT2515_MISO_INIT(); // MISO 上拉输入
}

/*******************************************************************************
 * 函数名  : SPI_ReadByte
 * 描述    : 通过SPI读取一个字节数据
 * 输入    : 无
 * 输出    : 无
 * 返回值  : rByte(读取到的一个字节数据)
 * 说明    : 无
 *******************************************************************************/
unsigned char SPI_ReadByte(void)
{
    unsigned char i, rByte = 0;

    SPI_SCK_RESET();
    bsp_spi_delay();
    for (i = 0; i < 8; i++)
    {
        SPI_SCK_SET();
        bsp_spi_delay();
        rByte <<= 1;
        if (MISO_IS_H())
        {
            rByte |= BIT(0);
        }
        SPI_SCK_RESET();
        bsp_spi_delay();
    }
    return rByte;
}

/*******************************************************************************
 * 函数名  : SPI_SendByte
 * 描述    : SPI发送一个字节数据
 * 输入    : dt:待发送的数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void SPI_SendByte(unsigned char dt)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        SPI_SCK_RESET();
        bsp_spi_delay();
        if ((dt << i) & 0x80)
            SPI_MOSI_SET();
        else
            SPI_MOSI_RESET();
        bsp_spi_delay();
        SPI_SCK_SET();
        bsp_spi_delay();
    }
    SPI_SCK_RESET();
    bsp_spi_delay();
}

/*******************************************************************************
 * 函数名  : SIT2515_WriteByte
 * 描述    : 通过SPI向SIT2515指定地址寄存器写1个字节数据
 * 输入    : addr:SIT2515寄存器地址,dat:待写入的数据
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void SIT2515_WriteByte(unsigned char addr, unsigned char dat)
{
    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_WRITE); // 发送写命令
    SPI_SendByte(addr);      // 发送地址
    SPI_SendByte(dat);       // 写入数据
    SIT2515_CS_SET();        // 置SIT2515的CS为高电平
}

/*******************************************************************************
 * 函数名  : SIT2515_WriteMultipleData
 * 描述    : 通过SPI向SIT2515向TXB0DLC 和TXB0D 写数据
 * 输入    : u8 *pbuf 数据存放位置,len:写入的数据长度
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void SIT2515_WriteMultipleData(u8 *pbuf, u8 addr, u8 len)
{
    unsigned char i;
    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_WRITE); // 发送写命令
    SPI_SendByte(addr);      // 发送地址
    SPI_SendByte(len);       // 发送数据长度
    for (i = 0; i < len; i++)
    {
        SPI_SendByte(pbuf[i]); // 写入数据
    }
    SIT2515_CS_SET(); // 置SIT2515的CS为高电平
}
/*******************************************************************************
 * 函数名  : SIT2515_ReadByte
 * 描述    : 通过SPI从SIT2515指定地址寄器读1个字节数据
 * 输入    : addr:SIT2515寄存器地址
 * 输出    : 无
 * 返回值  : rByte:读取到寄存器的1个字节数据
 * 说明    : 无
 *******************************************************************************/
unsigned char SIT2515_ReadByte(unsigned char addr)
{
    unsigned char rByte;

    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_READ); // 发送读命令
    SPI_SendByte(addr);     // 发送地址
    rByte = SPI_ReadByte(); // 读取数据
    SIT2515_CS_SET();       // 置SIT2515的CS为高电平
    return rByte;           // 返回读到的一个字节数据
}

void SIT2515_ReadMultipleByte(unsigned char addr, u8 *pbuf, unsigned char len)
{
    unsigned char i;

    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_READ); // 发送读命令
    SPI_SendByte(addr);     // 发送地址
    for (i = 0; i < len; i++)
    {
        pbuf[i] = SPI_ReadByte(); // 读取数据
    }
    SIT2515_CS_SET(); // 置SIT2515的CS为高电平
}

unsigned char SIT2515_ReadMultipleData(u8 *pbuf, u8 addr)
{
    unsigned char i;
    unsigned char len;
    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_READ); // 发送读命令
    // SPI_SendByte(RXB0DLC);  // 发送地址
    SPI_SendByte(addr);   // 发送地址
    len = SPI_ReadByte(); // 读取数据个数
    // printf("read len = %d\n",len);
    for (i = 0; i < len; i++)
    {
        pbuf[i] = SPI_ReadByte(); // 读取数据
    }
    SIT2515_CS_SET(); // 置SIT2515的CS为高电平
    return len;
}

/*******************************************************************************
 * 函数名  : SIT2515_ShowReg
 * 描述    : 串口打印寄存器值
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 将内部寄存器值通过串口打印输出
 *******************************************************************************/
void SIT2515_ShowReg(void)
{
    unsigned char i;
    for (i = 0; i < 128; i++)
    {
        if ((i % 8) == 0)
        {
            printf("\r\n ");
        }
        printf("R0x%02x=0x%02x  ", i, SIT2515_ReadByte(i));
    }
}
/*******************************************************************************
 * 函数名  : SIT2515_Reset
 * 描述    : 发送复位指令软件复位SIT2515
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 将内部寄存器复位为缺省状态,并将器件设定为配置模式
 *******************************************************************************/
void SIT2515_Reset(void)
{
    SIT2515_CS_RESET(); // 置SIT2515的CS为低电平
    bsp_spi_delay();
    SPI_SendByte(CAN_RESET); // 发送寄存器复位命令
    SIT2515_CS_SET();        // 置SIT2515的CS为高电平
    delay_ms(1);             // 通过软件延时约nms(不准确)
    printf("SIT2515_Reset %02x\n", SIT2515_ReadByte(CANSTAT));
}

/*******************************************************************************
 * 函数名  : SIT2515_SPICS_Init
 * 描述    : SIT2515  SPI片选IO 初始化
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    :
 *******************************************************************************/
void SIT2515_SPICS_Init(void)
{
    SIT2515_CS_INIT();
    SIT2515_CS_SET();
}

/*******************************************************************************
 * 函数名  : SIT2515_Init
 * 描述    : SIT2515初始化配置
 * 输入    : 无
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 初始化包括：软件复位、工作波特率设置、标识符相关配置等。
 *******************************************************************************/

#if 1
void SIT2515_Init(void)
{
    unsigned char temp = 0;

    printf("SIT2515_Init\n");
    SIT2515_SPICS_Init();
    SPI_MasterInit1();
    SIT2515_Reset(); // 发送复位指令软件复位SIT2515
    delay_5ms(2);    // 通过软件延时约nms(不准确)

    // 设置波特率 为 500Kbps
    SIT2515_WriteByte(CNF1, CAN_500Kbps);
    // set CNF2,SAM=0,在采样点对总线进行一次采样，PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
    SIT2515_WriteByte(CNF2, 0x80 | PHSEG1_3TQ | PRSEG_1TQ);
    // set CNF3,PHSEG2=(2+1)TQ=3TQ,同时当CANCTRL.CLKEN=1时设定CLKOUT引脚为时间输出使能位
    SIT2515_WriteByte(CNF3, PHSEG2_3TQ);

    SIT2515_WriteByte(TXB0SIDH, (STANDARD_ID >> 3) & 0xff); // 发送缓冲器0标准标识符高位
    SIT2515_WriteByte(TXB0SIDL, (STANDARD_ID << 5) & 0xff); // 发送缓冲器0标准标识符低位

    // RXB0  配制
    SIT2515_WriteByte(RXB0CTRL, 0x00);                      //
    SIT2515_WriteByte(RXF0SIDH, (STANDARD_ID >> 3) & 0xff); // 配置验收滤波寄存器n标准标识符高位
    SIT2515_WriteByte(RXF0SIDL, (STANDARD_ID << 5) & 0xff); // 配置验收滤波寄存器n标准标识符低位
    SIT2515_WriteByte(RXM0SIDH, 0xFF);                      // 配置验收屏蔽寄存器n标准标识符高位
    SIT2515_WriteByte(RXM0SIDL, 0xE0);                      // 配置验收屏蔽寄存器n标准标识符低位

    SIT2515_WriteByte(RXF0EID8, 0x00); // 配置验收滤波寄存器n扩展标识符高位
    SIT2515_WriteByte(RXF0EID0, 0x00); // 配置验收滤波寄存器n扩展标识符低位
    SIT2515_WriteByte(RXM0EID8, 0x00); // 配置验收屏蔽寄存器n扩展标识符高位
    SIT2515_WriteByte(RXM0EID0, 0x00); // 配置验收屏蔽寄存器n扩展标识符低位

    SIT2515_WriteByte(RXF1SIDH, (STANDARD_ID >> 3) & 0xff); // 配置验收滤波寄存器n标准标识符高位
    SIT2515_WriteByte(RXF1SIDL, (STANDARD_ID << 5) & 0xff); // 配置验收滤波寄存器n标准标识符低位

    SIT2515_WriteByte(RXF1EID8, 0x00); // 配置验收滤波寄存器n扩展标识符高位
    SIT2515_WriteByte(RXF1EID0, 0x00); // 配置验收滤波寄存器n扩展标识符低位

    // RXB1 配制
    SIT2515_WriteByte(RXB1CTRL, 0x00); //
    SIT2515_WriteByte(RXF2SIDH, 0x00); // 配置验收滤波寄存器n标准标识符高位
    SIT2515_WriteByte(RXF2SIDL, 0x00); // 配置验收滤波寄存器n标准标识符低位
    SIT2515_WriteByte(RXF2EID8, 0x00); // 配置验收滤波寄存器n扩展标识符高位
    SIT2515_WriteByte(RXF2EID0, 0x00); // 配置验收滤波寄存器n扩展标识符低位
    SIT2515_WriteByte(RXM1SIDH, 0x00); // 配置验收屏蔽寄存器n标准标识符高位
    SIT2515_WriteByte(RXM1SIDL, 0x00); // 配置验收屏蔽寄存器n标准标识符低位
    SIT2515_WriteByte(RXM1EID8, 0x00); // 配置验收屏蔽寄存器n扩展标识符高位
    SIT2515_WriteByte(RXM1EID0, 0x00); // 配置验收屏蔽寄存器n扩展标识符低位
    SIT2515_WriteByte(RXF3SIDH, 0x00); // 配置验收滤波寄存器n标准标识符高位
    SIT2515_WriteByte(RXF3SIDL, 0x00); // 配置验收滤波寄存器n标准标识符低位
    SIT2515_WriteByte(RXF3EID8, 0x00); // 配置验收滤波寄存器n扩展标识符高位
    SIT2515_WriteByte(RXF3EID0, 0x00); // 配置验收滤波寄存器n扩展标识符低位

    SIT2515_WriteByte(CANINTF, 0x00); // 清空CAN中断标志寄存器的所有位(必须由MCU清空)
    SIT2515_WriteByte(CANINTE, 0x01); // 配置CAN中断使能寄存器的接收缓冲器0 满中断使能,其它位禁止中断

    SIT2515_WriteByte(CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED); // 将SIT2515设置为正常模式,退出配置模式
    delay_5ms(2);
    temp = SIT2515_ReadByte(CANSTAT); // 读取CAN状态寄存器的值
    printf("CANSTAT %02x\n", temp);
    if (OPMODE_NORMAL != (temp & 0xE0)) // 判断SIT2515是否已经进入正常模式
    {
        SIT2515_WriteByte(CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED); // 再次将SIT2515设置为正常模式,退出配置模式
    }

// 添加验证读取
#if USER_DEBUG_ENABLE
    my_printf("CNF1 = %02x\n", SIT2515_ReadByte(CNF1));       // 应该返回   (0x00：500Kbps ，0x01: 250Kbps)
    my_printf("CNF2 = %02x\n", SIT2515_ReadByte(CNF2));       // 应该返回 0x90
    my_printf("CNF3 = %02x\n", SIT2515_ReadByte(CNF3));       // 应该返回 0x02
    my_printf("CANCTRL = %02x\n", SIT2515_ReadByte(CANCTRL)); // 正常模式
    my_printf("CANSTAT = %02x\n", SIT2515_ReadByte(CANSTAT)); // 状态寄存器
#endif
}
#endif

/*******************************************************************************
 * 函数名  : CAN_Send_Buffer
 * 描述    : CAN发送指定长度的数据
 * 输入    : *CAN_TX_Buf(待发送数据缓冲区指针),len(待发送数据长度)
 * 输出    : 无
 * 返回值  : 无
 * 说明    : 无
 *******************************************************************************/
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf, unsigned char len)
{

    unsigned int dly = 0;
    while ((SIT2515_ReadByte(TXB0CTRL) & 0x08) && (dly < 100)) // 快速读某些状态指令,等待TXREQ标志清零
    {
        dly++;
        delay_ms(1);
    }
    printf("CAN_Send_Buffer-> TXB0CTRL = %02x, delay_cnt = %d\n", SIT2515_ReadByte(TXB0CTRL), dly);
    SIT2515_WriteMultipleData(CAN_TX_Buf, TXB0DLC, len); // 将本帧待发送的数据长度和数据写入发送缓冲器0的发送长度寄存器
    SIT2515_WriteByte(TXB0CTRL, 0x08);                   // 请求发送报文
}

/*******************************************************************************
 * 函数名  : CAN_Receive_Buffer
 * 描述    : CAN接收一帧数据
 * 输入    : *CAN_TX_Buf(待接收数据缓冲区指针)
 * 输出    : 无
 * 返回值  : len(接收到数据的长度,0~8字节)
 * 说明    : 无
 *******************************************************************************/
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
    unsigned char len = 0;
    unsigned char temp = 0;
    u8 sid_h = 0;
    u8 sid_l = 0;
    u16 sid = 0;

    // printf("CAN_RX_STATUS = %02x\n",SIT2515_ReadByte(CAN_RX_STATUS));
    temp = SIT2515_ReadByte(CANINTF);

    // my_printf("CANINTF = %02x\n", temp);

    if (temp & 0x01) // RX0
    {
        sid_h = SIT2515_ReadByte(RXB0SIDH);
        sid_l = SIT2515_ReadByte(RXB0SIDL);
        // my_printf("RXB0SIDH = %02x,  RXB0SIDL = %02x\n", sid_h, sid_l);
        sid = (sid_h << 3) | (sid_l >> 5);
        // my_printf("sid = %x\n", sid);
        if (sid == STANDARD_ID) // USER_TO_DO　测试时，屏蔽这里，接收所有数据帧
        {
            if ((sid_l & 0x10) == 0) // 数据帧，非远程帧
            {
                len = SIT2515_ReadMultipleData(CAN_RX_Buf, RXB0DLC); // 接收数据并返回数据长度
                // my_printf("CAN RX0 (%d):", len);
                // my_printf(CAN_RX_Buf, len);

                // u8 i = 0;
                // for (i = 0; i < len; i++)
                // {
                //     my_printf("%02x ", CAN_RX_Buf[i]);
                // }
                // my_printf("\n");
            }
        }

        SIT2515_WriteByte(CANINTF, temp & ~(0x03)); // 清除中断标志位(中断标志寄存器必须由MCU清零)
    }

    if (temp & 0x02) // RX1
    {
        // len=SIT2515_ReadMultipleData(CAN_RX_Buf,RXB1DLC);//接收数据并返回数据长度

        // my_printf("CAN RX1: \n");
        // print_r(CAN_RX_Buf, len);
        sid_h = SIT2515_ReadByte(RXB1SIDH);
        sid_l = SIT2515_ReadByte(RXB1SIDL);
        // my_printf("RXB1SIDH = %02x,  RXB1SIDL = %02x\n", sid_h, sid_l);
        sid = (sid_h << 3) | (sid_l >> 5);
        // my_printf("sid = %x\n", sid);
        if (sid == STANDARD_ID) // USER_TO_DO　测试时，屏蔽这里，接收所有数据帧
        {
            // my_printf("sid = %x\n", sid);
            if ((sid_l & 0x10) == 0) // 数据帧，非远程帧
            {
                len = SIT2515_ReadMultipleData(CAN_RX_Buf, RXB1DLC); // 接收数据并返回数据长度
                // my_printf("CAN RX1 (%d):", len);
                // my_printf(CAN_RX_Buf, len);

                // u8 i = 0;
                // for (i = 0; i < len; i++)
                // {
                //     my_printf("%02x ", CAN_RX_Buf[i]);
                // }
                // my_printf("\n");
            }
        }

        SIT2515_WriteByte(CANINTF, temp & ~(0x02)); // 清除中断标志位(中断标志寄存器必须由MCU清零)
    }

    return len;
}
