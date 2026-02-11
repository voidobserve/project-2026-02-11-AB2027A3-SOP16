#ifndef _SPI1_MASTER_H
#define _SPI1_MASTER_H


/* 例程调试参数 */
#define SPI1_MASTER_ID                           0x40140000  // Jedec唯一ID，通过读ID判断FLASH是否在线, 需要改成SPIFLASH对应的ID
#define SPI1_MASTER_CHECK_ONLINE_ADDR            0
#define SPI1_MASTER_CHECK_ONLINE_INFO            "2 WIRE SPI TEST"

/* SPI IO Definition */
#define SPI1_MASTER_CLK_PORT                     GPIOA_REG
#define SPI1_MASTER_CLK_PIN                      GPIO_PIN_0
#define SPI1_MASTER_MISO_PORT                    GPIOA_REG
#define SPI1_MASTER_MISO_PIN                     GPIO_PIN_1
#define SPI1_MASTER_MOSI_PORT                    GPIOA_REG
#define SPI1_MASTER_MOSI_PIN                     GPIO_PIN_2
#define SPI1_MASTER_CS_PORT                      GPIOA_REG
#define SPI1_MASTER_CS_PIN                       GPIO_PIN_3

/* 设备ID */
#define W25Q80                              0XEF13
#define W25Q16                              0XEF14
#define W25Q32                              0XEF15
#define W25Q64                              0XEF16
#define W25Q128                             0XEF17
#define W25Q256                             0XEF18

/* 指令表 */
#define SPI1_MASTER_PAGE_PROGRAM                 0x02        // 页写
#define SPI1_MASTER_READ_DATA                    0x03        // 读数据
#define SPI1_MASTER_WRITE_ENABLE                 0x06        // 写使能
#define SPI1_MASTER_WRITE_DISABLE                0x04        // 写失能
#define SPI1_MASTER_SECTOR_ERASE                 0x20        // 扇区擦除(min erase unit 4096Byte)
#define SPI1_MASTER_READ_STATUS_REG1             0x05        // 读SR1
#define SPI1_MASTER_READ_STATUS_REG2             0x35
#define SPI1_MASTER_READ_STATUS_REG3             0x15
#define SPI1_MASTER_WRITE_STATUS_REG1            0x01
#define SPI1_MASTER_WRITE_STATUS_REG2            0x31
#define SPI1_MASTER_WRITE_STATUS_REG3            0x11
#define SPI1_MASTER_FAST_READ_DATA               0x0B
#define SPI1_MASTER_FAST_READ_DUAL               0x3B
#define SPI1_MASTER_BLOCK_ERASE                  0xD8
#define SPI1_MASTER_CHIP_ERASE                   0xC7
#define SPI1_MASTER_POWER_DOWN                   0xB9
#define SPI1_MASTER_RELEASE_POWER_DOWN           0xAB
#define SPI1_MASTER_DEVICE_ID                    0xAB
#define SPI1_MASTER_MANUFACT_DEVICE_ID           0x90
#define SPI1_MASTER_JEDEC_DEVICE_ID              0x9F
#define SPI1_MASTER_ENABLE_4BYTE_ADDR            0xB7
#define SPI1_MASTER_EXIT_4BYTE_ADDR              0xE9

void spi1_master_erase_sector(u32 addr);
void spi1_master_erase_block(u32 addr);
void spi1_master_erase_chip(void);
uint8_t spi1_master_read_status_register(uint8_t idx);
void spi1_master_read(u8 *buf, u32 addr, u32 len);
void spi1_master_write_no_check(u8 *buf, u32 addr, u32 len);
void spi1_master_write(u8 *buf, u32 addr, u32 len);
uint32_t spi1_master_jedec_id_read(void);
uint16_t spi1_master_manufact_id_read(void);

void spi1_master_init(u32 baud);
void spi1_master_example(void);

#endif // _SPI1_FLASH_H
