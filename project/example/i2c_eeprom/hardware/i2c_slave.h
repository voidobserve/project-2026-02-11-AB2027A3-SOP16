#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#define DEVICE_ADDR                         0xa8
#define SLAVE_CHECK_FLAG                    0xab
#define SLAVE_CHECK_ADDR                    127

void i2c_slave_init(void);
void i2c_slave_example(void);
void iic_slave_reg_value_set(uint16_t reg_addr,uint8_t *buf,uint16_t len);
void iic_slave_reg_value_get(uint16_t reg_addr,uint8_t *buf,uint16_t len);




#endif
