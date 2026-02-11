#include "driver_com.h"
#include "driver_iic.h"
#include "driver_gpio.h"
#include "i2c_eeprom.h"
#include "soft_iic.h"

// 软件IIC使能：  软件:1    硬件:0
#define SOFT_I2C_ENABLE         0

#define HW_I2C_SCL_PORT    GPIOA_REG
#define HW_I2C_SCL_PIN     GPIO_PIN_1
#define HW_I2C_SDA_PORT    GPIOA_REG
#define HW_I2C_SDA_PIN     GPIO_PIN_2


/* Definition in soft_iic.h */
// #define SOFT_I2C_SCL_GPIO        GPIOA_REG
// #define SOFT_I2C_SCL_PIN         GPIO_PIN_1

// #define SOFT_I2C_SDA_PORT        GPIOA_REG
// #define SOFT_I2C_SDA_PIN         GPIO_PIN_2


u8 eeprom_buf[128];
u8 eeprom_init_flag = 0;

// 读取EEPROM一个字节
u8 eeprom_read_one_byte(u8 addr)
{
    u32 tmp;

    #if SOFT_I2C_ENABLE
        bsp_iicsw_read_bytes(EEPROM_DEV_ADDR,addr,(u8 *)(&tmp),1);
    #else
        iic_receive_data(EEPROM_DEV_ADDR, addr, &tmp, 1, 10);
    #endif // SOFT_I2C_ENABLE

    return (uint8_t)(tmp & 0xff);
}

// 写入EEPROM一个字节
void eeprom_write_one_byte(u8 addr,u8 data)
{
    #if SOFT_I2C_ENABLE
        bsp_iicsw_write_bytes(EEPROM_DEV_ADDR, addr, &data,1);
    #else
        iic_send_data(EEPROM_DEV_ADDR, addr, data, 1, 10);
    #endif // SOFT_I2C_ENABLE
}

// 写入EEPROM一个数据单元，最大4Bytes
u32 eeprom_read_len_byte(u16 addr,u8 len)
{
    u32 tmp = 0;

    #if SOFT_I2C_ENABLE
        u8 i;

        for (i = 0; i < len; i++) {
            tmp <<= 8;
            eeprom_read_one_byte(addr + len - i - 1);
        }
    #else
        iic_receive_data(EEPROM_DEV_ADDR, addr, &tmp, 1, 10);
    #endif // SOFT_I2C_ENABLE

    return (uint8_t)(tmp & 0xff);
}

// 读取EEPROM一个数据单元，最大4Bytes
void eeprom_write_len_byte(u16 addr,u32 data,u8 len)
{
    #if SOFT_I2C_ENABLE
        u8 i;
        for (i = 0; i < len; i++) {
            eeprom_write_one_byte(addr + i, (data >> (8 * i)) & 0xff);
        }
    #else
        iic_send_data(EEPROM_DEV_ADDR, addr, data, len, 10);
    #endif // SOFT_I2C_ENABLE
}

// 连续写EEPROM
void eeprom_write(u16 addr,u8 *p,u16 num)
{
    while (num--) {
        eeprom_write_one_byte(addr, *p);
        addr++;
        p++;
    }
}

// 连续读EEPROM
void eeprom_read(u16 addr,u8 *p,u16 num)
{
    while (num) {
        *p++ = eeprom_read_one_byte(addr);
        addr++;
        num--;
    }
}

// 硬件IIC初始化
void hardware_i2c_init(void)
{
    gpio_init_typedef gpio_init_structure;
    iic_init_typedef iic_init_struct;

    clk_gate2_cmd(CLK_GATE2_IIC, CLK_EN);

    gpio_init_structure.gpio_pin = HW_I2C_SDA_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HW_I2C_SDA_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = HW_I2C_SCL_PIN;
    gpio_init(HW_I2C_SCL_PORT, &gpio_init_structure);

    gpio_func_mapping_config(HW_I2C_SDA_PORT, HW_I2C_SDA_PIN, GPIO_CROSSBAR_OUT_I2CSDA);
    gpio_func_mapping_config(HW_I2C_SDA_PORT, HW_I2C_SDA_PIN, GPIO_CROSSBAR_IN_I2CSDA);

    gpio_func_mapping_config(HW_I2C_SCL_PORT, HW_I2C_SCL_PIN, GPIO_CROSSBAR_OUT_I2CSCL);
    gpio_func_mapping_config(HW_I2C_SCL_PORT, HW_I2C_SCL_PIN, GPIO_CROSSBAR_IN_I2CSCL);

    iic_init_struct.clk_source = IIC_CLK_X24MDIV8;
    iic_init_struct.scl_pose_div = 29;      // clk_source div29, it means freq of scl is 100KHz when works.
    iic_init_struct.sda_hold_cnt = 2;
    iic_init_struct.mode_sel = IIC_ROLE_MODE_MASTER;
    iic_init(&iic_init_struct);

    iic_cmd(ENABLE);
}

// 检测EEPROM是否在线
// 成功：1
// 失败：0
u8 eeprom_check(void)
{
    u8 tmp;
    tmp = eeprom_read_one_byte(CHECK_ADDR);
    if (tmp == CHECK_FLAG) {
        return 1;
    } else {
        eeprom_write_one_byte(CHECK_ADDR, CHECK_FLAG);
        delay_ms(10);
        tmp = eeprom_read_one_byte(CHECK_ADDR);
        if (tmp == CHECK_FLAG) {
            return 1;
        }
    }
    return 0;
}

// 初始化IIC
void i2c_eeprom_init(void)
{
    #if SOFT_I2C_ENABLE
        bsp_iicsw_init();
        printf("\n---> run with software iic <---\n");
    #else
        hardware_i2c_init();
        printf("\n---> run with hardware iic <---\n");
    #endif // SOFT_I2C_ENABLE
}

// 测试函数
void i2c_eeprom_example(void)
{
    if (!eeprom_check()) {
        printf("---> eeprom init failed!\n");
        eeprom_init_flag = 0;
    } else if (eeprom_init_flag == 0){
        printf("---> eeprom init success.\n");
        eeprom_init_flag = 1;
        printf("EEPROM:\n");
        eeprom_read(0, eeprom_buf, 128);
        print_r(eeprom_buf, 128);
    }
}
