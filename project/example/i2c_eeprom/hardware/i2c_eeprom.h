#ifndef _I2C_EEPROM_H
#define _I2C_EEPROM_H


// EEPROM dev address[7:1]. bit0 means R/W, soft control.
#define EEPROM_DEV_ADDR         0xa8

// 上电检测EEPROM芯片读取内存地址及数值
#define CHECK_FLAG              0xab
#define CHECK_ADDR              127

void hardware_i2c_init(void);
u8 eeprom_read_one_byte(u8 addr);
void eeprom_write_one_byte(u8 addr,u8 data);
void eeprom_write_len_byte(u16 addr,u32 data,u8 len);
u32 eeprom_read_len_byte(u16 addr,u8 len);
void eeprom_write(u16 addr,u8 *p,u16 num);
void eeprom_read(u16 addr,u8 *p,u16 num);
u8 eeprom_check(void);

void i2c_eeprom_init(void);
void i2c_eeprom_example(void);

#endif
