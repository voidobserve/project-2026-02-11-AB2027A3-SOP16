#include "driver_com.h"
#include "driver_iic.h"
#include "driver_gpio.h"
#include "i2c_slave.h"

#define HW_I2C_SCL_PORT    GPIOA_REG
#define HW_I2C_SCL_PIN     GPIO_PIN_1

#define HW_I2C_SDA_PORT    GPIOA_REG
#define HW_I2C_SDA_PIN     GPIO_PIN_2


////////////////////////////////////////////////////////////////////////////////
// iicsw:Master iic0:Slave
/*                         6543210
 *                       N wrPRStr AIRT
 * 00: SwAP            : 1 0010100 1    只发送了一个地址过来
 * 01: SwAPSw          : 1 0110100 1    只发送了一个地址过来，没来得及处理；又准备发送，被硬件Hold住
 * 02: SwAPSr          : 1 1010100 1    只发送了一个地址过来，没来得及处理；准备读，被硬件Hold住
 * 03: SwADDP          : 3 0010100 111  同00-02，多了一些数据
 * 04: SwADDDP         : 4              同03。但这是不可能的情况，05已经被硬件Hold住了。归到08
 * 05: SwADDD          : 4 0100100 111  发送了地址和数据，缓冲满了，被硬件Hold住，P可能还未到、或被Hold
 * 06:       DDDD      : 4 0100000 011  05处理完成，主机持续发送数据
 * 07:       DDDP      : 3 0010000 011  05处理完成，主机发送了一些数据和P
 * 08:       P         : 0 0010000 0000 05处理完成，主机发送了P
 * 09:       PSw       : 0 0110100 0000 05处理完成，主机发送了P，没来得及处理；又准备发送，硬件会Hold在此处
 * 10:       PSr       : 0 1010100 0000 05处理完成，主机发送了P，没来得及处理；又准备接收，被硬件Hold住
 * 11:       Rw        : 0 0101000 05处理完成，主机发送了R，准备继续发送，被硬件Hold住
 * 12:       Rr        : 0 1001000 05处理完成，主机发送了R，准备接收，被硬件Hold住
 * 06:       DDDD      : 4 0100000 011  05处理完成，主机持续发送数据
 * 07:       DDDP      : 3 0010000 011  05处理完成，主机发送了一些数据和P
 * 08:       P         : 0 0010000 0000 05处理完成，主机发送了P
 * 09:       PSw       : 0 0110100 0000 05处理完成，主机发送了P，没来得及处理；又准备发送，硬件会Hold在此处
 * 10:       PSr       : 0 1010100 0000 05处理完成，主机发送了P，没来得及处理；又准备接收，被硬件Hold住
 * 11:       Rw        : 0 0101000 05处理完成，主机发送了R，准备继续发送，被硬件Hold住
 * 12:       Rr        : 0 1001000 05处理完成，主机发送了R，准备接收，被硬件Hold住
 * 13: SwARw           : 1 0101100 发送了一个地址，再试图连续发送，被硬件Hold住
 * 14: SwARr           : 1 1001100 发送了一个地址，再试图接收数据，被硬件Hold住。这是标准读过程
 * 15:      DDDD       : 4 1000000
 * 16:      DDDP       : 3 0010000
 * 17:      P          : 0 0010000
 * 18: Sr              : 直接读
 * 19: 不是自己的addr，会不会有误Stop？
 *
 * P/PSw/PSr的问题：
 * 对P的处理，存在一个时间差：读状态，发现只是一个P及IDLE状态；再去清Done时，新的Sw/Sr已经上来了。
 * 这个时间差会导致新的Done状态被旧P状态清理的时候误请掉，所致IIC总线被长期锁定、得不到处理。
 * 解决办法是，清P Done之后，再把SRP状态读进来，如果还有状态，则表示有新的状态进来。
 */


typedef enum  {
	STM_IDLE = 0,
	STM_TX   = 1,
	STM_RX   = 2
}stm_t;


typedef struct {
  u8 stop_sign_flag; // 停止信号
  u8 rx_flag;        // 接收数据标记
  u8 tx_flag;        //发送数据标记
  stm_t sta_machine ;   //从机状态机 保存软件自己的状态机，记录Done信号到来之前软件的收发状态
  uint8_t reg_addr;
  uint8_t reg_index;
  uint8_t reg_buf[256];
} iic_slave_typedef;





static iic_slave_typedef iic_slave_cb AT(.buf.iic_buf);


AT(.com_text.save.dat)
void save_dat(uint32_t dat, uint32_t cnt) // buf for check
{
        uint8_t addr = iic_slave_cb.reg_index;
        while (cnt--) {
            iic_slave_cb.reg_buf[addr] = dat & 0xFF;
            ++addr;
            dat >>= 8;
        }
}

AT(.com_text.read.dat)
static uint32_t read_dat(void)
{
	uint32_t dat;
	uint8_t addr = iic_slave_cb.reg_index;
	dat  = iic_slave_cb.reg_buf[addr++];
	dat |= iic_slave_cb.reg_buf[addr++] << 8;
	dat |= iic_slave_cb.reg_buf[addr++] << 16;
	dat |= iic_slave_cb.reg_buf[addr++] << 24;
	return dat;
}

AT(.com_text.bump.adr)
static void bump_adr(uint32_t cnt)
{
	iic_slave_cb.reg_index += cnt;
}

AT(.com_text.get.adr)
static uint32_t iic_get_adr(uint32_t dat)
{
	iic_slave_cb.reg_index = dat;
	return dat >> 8;
}


AT(.com_text.i2c.irq)
void i2c_irq(void)
{
    uint32_t sta;
    uint32_t dat;
    uint32_t sta1;

    if (iic_get_flag(IIC_IT_DONE)){
        sta = iic_slave_status_get();
        uint8_t cnt = iic_slave_data_len_get();
        if (cnt){
            dat = iic_slave_data_get();
            switch (iic_slave_cb.sta_machine){
                case STM_IDLE:
                    iic_slave_cb.reg_addr=dat;
					//printf("dat = %08x \n",dat);
                    dat = iic_get_adr(dat);//获取主机写入的寄存器地址
                    --cnt;
                case STM_RX:
                    save_dat (dat, cnt);//将数据写入寄存器buf
                    bump_adr (cnt);//寄存器地址递增偏移
                    break;
                case STM_TX:
                    bump_adr (cnt);
                    break;
            }
        }

        /* 清Stop
        * 主机发送Stop表示上一条指令已经结束。
        * 此刻状态已经接收下来（IICDATA迟点接收，只要不Kick就没关系），可以把因Stop产生的Done清理掉。
        * 但在看到Done到下面清Done之间，主要可能已经发送新的指令被硬件识别到，因此清Done之后要再看看状态有没有变化过
        * 处理data前处理这个Stop Done，的确会减少误清的机会。但其实无济于事，还会干扰处理流程
        */
        if (iic_slave_rx_stop_sign()==SET) {
            iic_slave_cb.stop_sign_flag = 1;
            iic_clear_done_flag();
            sta1 = iic_slave_status_get();
            /* 清Done，再重新读入状态
            * 如果状态有了变化，Done又没了，表示Done被误清了：
            * 下面的流程，按新的状态来处理
            */
            if (sta1!= sta){
                sta = sta1;
            }
            iic_slave_cb.sta_machine  = STM_IDLE;
        }
        else{
            iic_clear_done_flag();
        }

        // 最后响应新的请求
        if (iic_slave_is_tx_status(sta)) {
			dat = read_dat();
			iic_slave_data_send(dat);
            iic_clear_slave_tx_status();
            iic_slave_cb.sta_machine  = STM_TX;
            iic_slave_cb.tx_flag=1;
        } else {
            iic_clear_slave_rx_status();
            if(iic_slave_is_rx_status(sta)==RESET){
                iic_slave_cb.sta_machine  =STM_IDLE;
            }
            else{
                iic_slave_cb.sta_machine  =STM_RX;
            }
            iic_slave_cb.rx_flag=1;
        }
    }
}



// 初始化IIC
void i2c_slave_init(void)
{
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = HW_I2C_SDA_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HW_I2C_SDA_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = HW_I2C_SCL_PIN;
    gpio_init(HW_I2C_SCL_PORT, &gpio_init_structure);



    clk_gate2_cmd(CLK_GATE2_IIC, CLK_EN);

    gpio_func_mapping_config(HW_I2C_SDA_PORT, HW_I2C_SDA_PIN, GPIO_CROSSBAR_OUT_I2CSDA);
    gpio_func_mapping_config(HW_I2C_SDA_PORT, HW_I2C_SDA_PIN, GPIO_CROSSBAR_IN_I2CSDA);

    gpio_func_mapping_config(HW_I2C_SCL_PORT, HW_I2C_SCL_PIN, GPIO_CROSSBAR_OUT_I2CSCL);
    gpio_func_mapping_config(HW_I2C_SCL_PORT, HW_I2C_SCL_PIN, GPIO_CROSSBAR_IN_I2CSCL);

    iic_init_typedef iic_init_struct;
    //iic_init_struct.clk_source = IIC_CLK_X24MDIV8;
    //iic_init_struct.scl_pose_div = 29;      // clk_source div29, it means freq of scl is 100KHz when works.
    //iic_init_struct.sda_hold_cnt = 2;

    iic_init_struct.mode_sel = IIC_ROLE_MODE_SLAVE;
    iic_init(&iic_init_struct);

    iic_pic_config(i2c_irq, 0, IIC_IT_DONE, ENABLE);
    iic_slave_addr_set(DEVICE_ADDR);

    iic_cmd(ENABLE);
    memset(&iic_slave_cb,0,sizeof(iic_slave_cb));
    iic_slave_cb.sta_machine = STM_IDLE;
    printf("\n---> run with hardware iic <---\n");

}


void iic_slave_reg_value_set(uint16_t reg_addr,uint8_t *buf,uint16_t len)
{
    if(reg_addr>sizeof(iic_slave_cb.reg_buf)){
        printf("address out of range\n");
        return ;
    }
    if(reg_addr+len>sizeof(iic_slave_cb.reg_buf)){
        printf("The data length is too long\n");
        return ;
    }

    memcpy(&iic_slave_cb.reg_buf[reg_addr],buf,len);
}

void iic_slave_reg_value_get(uint16_t reg_addr,uint8_t *buf,uint16_t len)
{
    if(reg_addr>sizeof(iic_slave_cb.reg_buf)){
        printf("address out of range\n");
        return ;
    }
    if(reg_addr+len>sizeof(iic_slave_cb.reg_buf)){
        printf("The data length is too long\n");
        return ;
    }
    memcpy(buf,&iic_slave_cb.reg_buf[reg_addr],len);
}


// 测试函数
void i2c_slave_example(void)
{
    uint8_t test_data[]={0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X09,0X10};
    iic_slave_reg_value_set(0x00,test_data,sizeof(test_data));

    while(1)
	{
        static uint32_t dbg_tick = 0;
        if (tick_check_expire(dbg_tick, 50)) {
            dbg_tick = tick_get();

            if (iic_slave_cb.tx_flag&&iic_slave_cb.stop_sign_flag){
                printf("[iic master read] iic_slave_cb.reg_index = %d\n",iic_slave_cb.reg_index);
                printf("[iic master read] iic_slave_cb.reg_addr = %d\n",iic_slave_cb.reg_addr);
                print_r(&iic_slave_cb.reg_buf[iic_slave_cb.reg_addr], iic_slave_cb.reg_index-iic_slave_cb.reg_addr);
                test_data[1]=test_data[1]+1;
                test_data[2]=test_data[2]+1;
                iic_slave_reg_value_set(0x00,test_data,sizeof(test_data));
            }

            if (iic_slave_cb.rx_flag&&iic_slave_cb.stop_sign_flag){

                printf("[iic master write]iic_slave_cb.reg_index = %d\n",iic_slave_cb.reg_index);
                printf("[iic master write]iic_slave_cb.reg_addr = %d\n",iic_slave_cb.reg_addr);
                print_r(&iic_slave_cb.reg_buf[iic_slave_cb.reg_addr], iic_slave_cb.reg_index-iic_slave_cb.reg_addr);
            }


            if(iic_slave_cb.stop_sign_flag){
                iic_slave_cb.stop_sign_flag=0;
                iic_slave_cb.rx_flag=0;
                iic_slave_cb.tx_flag=0;
                iic_slave_cb.reg_index=0;
            }

        }
    }
}




