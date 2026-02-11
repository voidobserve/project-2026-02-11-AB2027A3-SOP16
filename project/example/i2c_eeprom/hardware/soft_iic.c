#include "soft_iic.h"
#include "include.h"

static void bsp_iicsw_delay (void)
{
    bsp_iicsw_delay_us (IIC_DELAY_US);
}

static void bsp_iicsw_delayN (uint32_t n)
{
  while (n--)
  bsp_iicsw_delay_us (IIC_DELAY_US);
}


// 外部用，模拟scl被拉住
void bsp_iicsw_scl_hold (uint32_t us)
{
    // printf("hold %d us\n", us);
    const uint32_t scl_bit = IIC_SCL_PIN;
    IIC_SCL_DIR &= ~scl_bit;
    bsp_iicsw_delay_us(us);
    IIC_SCL_DIR |=  scl_bit;
}

static inline void bsp_iicsw_scl_out (uint32_t lvl)
{
    const uint32_t scl_bit = IIC_SCL_PIN;
    if (lvl)
        IIC_SCL_DIR |=  scl_bit;
    else
        IIC_SCL_DIR &= ~scl_bit;
}

static inline void bsp_iicsw_sda_out (uint32_t lvl)
{
    const uint32_t sda_bit = IIC_SDA_PIN;
    if (lvl)
    IIC_SDA_DIR |= sda_bit;
    else
    IIC_SDA_DIR &= ~sda_bit;
}



static int wait_scl_high1(void)
{
    int t;
    for (t = 10000; t; --t){
        bsp_iicsw_delay();
        if (IIC_SCL_LVL) break;
    }
    bsp_iicsw_delay();
    return t;
}

static int wait_scl_high(void)
{
    int t;
    for (t = 10000; t; --t) {
        bsp_iicsw_delay();

        if (IIC_SCL_LVL) break;
    }
    bsp_iicsw_delay();
    return t;
}




static void bsp_iicsw_send_start (void)
{
    bsp_iicsw_scl_out(1);
    bsp_iicsw_sda_out(1);
    bsp_iicsw_delayN(1);
    bsp_iicsw_sda_out(0);
    bsp_iicsw_delayN(1);
    bsp_iicsw_scl_out(0);
}

static void bsp_iicsw_send_stop (void)
{
    bsp_iicsw_scl_out(0);
    bsp_iicsw_sda_out(0);
    bsp_iicsw_delayN(1);
    bsp_iicsw_scl_out(1);
    wait_scl_high();
    bsp_iicsw_delayN(1);
    bsp_iicsw_sda_out(1);

}


static void bsp_iicsw_send_restart (void)
{
    bsp_iicsw_sda_out(1);
    bsp_iicsw_delayN(1);
    bsp_iicsw_scl_out(1);
    wait_scl_high1();
    bsp_iicsw_delayN(2);
    bsp_iicsw_sda_out(0);
    bsp_iicsw_delayN(1);
    bsp_iicsw_scl_out(0);
}

uint32_t bsp_iicsw_send_byte (uint8_t dat)
{
    int i;
    uint32_t ack;

    for (i = 0; i < 8; ++i) {
        bsp_iicsw_sda_out(dat >> 7);
        bsp_iicsw_delayN(2);
        bsp_iicsw_scl_out(1);
        if (! wait_scl_high()) return 0; // slave hold scl timeout
        bsp_iicsw_scl_out(0);
        dat <<= 1;
    }

    // recv ack
    bsp_iicsw_sda_out (1);
    for (i = 0; i < 100; ++i) { // read ack
        bsp_iicsw_delay ();
        if (IIC_SDA_LVL == 0)
          break;
    }
    bsp_iicsw_scl_out (1);
    ack = IIC_SDA_LVL;
    bsp_iicsw_delay ();
    bsp_iicsw_scl_out (0);
    bsp_iicsw_delay ();
    return !ack;
}


static uint8_t bsp_iicsw_recv_byte (uint8_t ack_lvl)
{
    uint8_t dat = 0;
    int i;

    bsp_iicsw_sda_out (1); // input
    for (i = 0; i < 8; ++i) {
        bsp_iicsw_delayN  (2);
        bsp_iicsw_scl_out (1);
        if (! wait_scl_high()) return 0; // slave hold scl timeout
        dat <<= 1;
        if (IIC_SDA_LVL)
          ++dat;
        bsp_iicsw_scl_out (0);
    }

    bsp_iicsw_sda_out (ack_lvl);
    bsp_iicsw_delay   ();
    bsp_iicsw_scl_out (1);
    bsp_iicsw_delay   ();
    bsp_iicsw_scl_out (0);

    return dat;
}


void bsp_iicsw_write_bytes (uint8_t devaddr, uint8_t addr, uint8_t *dat, uint32_t len)
{
     uint8_t sys_clk=sys_clk_get();
    sys_clk_set(SYS_24M);
    bsp_iicsw_send_start();
    bsp_iicsw_send_byte(devaddr);
    bsp_iicsw_send_byte(addr);
    while (len--)
    bsp_iicsw_send_byte(*dat++);
    bsp_iicsw_send_stop();
    sys_clk_set(sys_clk);
}



/* 写和读之间用Restart */
void bsp_iicsw_read_bytes (uint8_t devaddr, uint8_t addr, uint8_t *dat, uint32_t len)
{
   uint8_t sys_clk=sys_clk_get();
    sys_clk_set(SYS_24M);
    int i;

    bsp_iicsw_send_start ();
    bsp_iicsw_send_byte  (devaddr);
    bsp_iicsw_send_byte  (addr);
    bsp_iicsw_send_restart ();

    bsp_iicsw_send_byte(devaddr| 1); // read
    for (i = 0; i < len; ++i){
      dat[i] = bsp_iicsw_recv_byte(i+1<len?IIC_ACK:IIC_NACK);
    }
    bsp_iicsw_send_stop();
    sys_clk_set(sys_clk);
}

void bsp_iicsw_init (void)
{
    const uint32_t scl_bit = IIC_SCL_PIN;
    const uint32_t sda_bit = IIC_SDA_PIN;

    IIC_SDA_PD&= ~sda_bit;
    IIC_SDA_PU|= sda_bit;
    IIC_SDA_FEN&= ~sda_bit;
    bsp_iicsw_sda_out (1);
    IIC_SDA_CLR=sda_bit;

    IIC_SCL_PD &= ~scl_bit;
    IIC_SCL_PU |= scl_bit;
    IIC_SCL_FEN &= ~scl_bit;
    bsp_iicsw_scl_out (1);
    IIC_SCL_CLR =scl_bit;
}

uint8_t write_dat[]={0X0A,0XFF,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10};

void bsp_iicsw_master_example(void)
{

	   printf("--->IIC i2c_master_example \n");
	   while (1) {

		   delay_ms(500);
		   #if 1
		   printf("--->bsp_iicsw_write_bytes\n");
	       bsp_iicsw_write_bytes (0XA8, 0X01, write_dat, sizeof(write_dat));
	       write_dat[0]=write_dat[0]+1%255;
           #else
		   printf("--->bsp_iicsw_read_bytes \n");
		   uint8_t read_dat[5]={0};
		   memset(read_dat,0,sizeof(read_dat));
		   bsp_iicsw_read_bytes(0XA8, 0X02,read_dat,sizeof(read_dat));
		   for(uint8_t i=0;i<sizeof(read_dat);i++){
               printf("read_dat[%d] = %02x\n",i,read_dat[i]);
           }
           #endif

	   }


}





