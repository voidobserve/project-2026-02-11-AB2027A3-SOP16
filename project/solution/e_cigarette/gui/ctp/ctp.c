#include "include.h"
#include "driver_iic.h"
#include "driver_gpio.h"

#if (CTP_SELECT != CTP_NO)
#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_K(...)            printk(__VA_ARGS__)
AT(.com_rodata)
const char str_xy[] = "%d: %d, %d\n";
#else
#define TRACE(...)
#define TRACE_K(...)
#endif

//TP相关配置
#define CTP_CNT_SHORT_MOV                (GUI_SCREEN_WIDTH / 30)    //CTP短划阈值(屏幕距离)
#define CTP_CNT_LONG_MOV                 (GUI_SCREEN_WIDTH / 3)     //CTP长划阈值(屏幕距离)
#define CTP_CNT_LONG                     40                         //CTP长按时间阈值(CTP INT时钟个数)
#define I2C_WRITE_ADDR(ADDR)     		 ((ADDR) << 1)				//CTP IIC写地址
#define I2C_READ_ADDR(ADDR)      		 ((ADDR) << 1 | 1)			//CTP IIC读地址

enum {
    CTP_STA_IDLE,
    CTP_STA_PRESS,
    CTP_STA_PRESS_LONG,
    CTP_STA_MOV_SHORT,
    CTP_STA_MOV_LONG,
};


static struct ctp_cb_t {
    s32 x;          //当前坐标
    s32 y;
    s32 sx;         //起始坐标
    s32 sy;
    s32 last_dx;    //最后一次相对偏移
    s32 last_dy;
    u8 cnt;
    u8 sta;
    bool flag_last;
    bool init;
} ctp_cb;


AT(.com_text.ctp)
void ctp_int_isr(void)
{
    if (WKUPEDG & BIT(16 + WAKEUP_IDX_PORT_FALL)) {
        WKUPCPND = BIT(16 + WAKEUP_IDX_PORT_FALL);
#if (CTP_SELECT == CTP_CST8X)
        ctp_cst8x_readkick();
#endif
    }
}

//处理TP的消息
AT(.com_text.ctp)
void ctp_msg_deal(bool press)
{
    if (press) {
        // reset_sleep_delay_all();

        int mx, my;
        mx = abs_s(ctp_cb.x - ctp_cb.sx);
        my = abs_s(ctp_cb.y - ctp_cb.sy);
        switch (ctp_cb.sta) {
        case CTP_STA_IDLE:
            ctp_cb.sta = CTP_STA_PRESS;                             //按下
            ctp_cb.sx = ctp_cb.x;
            ctp_cb.sy = ctp_cb.y;
            msg_enqueue(MSG_CTP_TOUCH);                             //触摸事件
            ctp_cb.cnt++;
            break;

        case CTP_STA_PRESS:
        case CTP_STA_PRESS_LONG:
        case CTP_STA_MOV_SHORT:
            if (ctp_cb.sta == CTP_STA_PRESS) {
                ctp_cb.cnt++;
                if (ctp_cb.cnt >= CTP_CNT_LONG) {
                    msg_enqueue(MSG_CTP_LONG);                       //长按
                    ctp_cb.sta = CTP_STA_PRESS_LONG;
                    return;
                }
            }
            if (mx >= my) {
                if (mx >= CTP_CNT_LONG_MOV) {
                    if (ctp_cb.x < ctp_cb.sx) {
                        if (ctp_cb.sta != CTP_STA_MOV_SHORT) {
                            msg_enqueue(MSG_CTP_SHORT_LEFT);        //补发左短划
                        }
                        msg_enqueue(MSG_CTP_LONG_LEFT);             //左长划
                        ctp_cb.sta = CTP_STA_MOV_LONG;
                    } else {
                        if (ctp_cb.sta != CTP_STA_MOV_SHORT) {
                            msg_enqueue(MSG_CTP_SHORT_RIGHT);       //补发右短划
                        }
                        msg_enqueue(MSG_CTP_LONG_RIGHT);            //右长划
                        ctp_cb.sta = CTP_STA_MOV_LONG;
                    }
                    return;
                }
                if (ctp_cb.sta != CTP_STA_MOV_SHORT && mx >= CTP_CNT_SHORT_MOV) {
                    if (ctp_cb.x < ctp_cb.sx) {
                        msg_enqueue(MSG_CTP_SHORT_LEFT);            //左短划
                        ctp_cb.sta = CTP_STA_MOV_SHORT;
                    } else {
                        msg_enqueue(MSG_CTP_SHORT_RIGHT);           //右短划
                        ctp_cb.sta = CTP_STA_MOV_SHORT;
                    }
                    return;
                }
            } else {
                if (my >= CTP_CNT_LONG_MOV) {
                    if (ctp_cb.y < ctp_cb.sy) {
                        if (ctp_cb.sta != CTP_STA_MOV_SHORT) {
                            msg_enqueue(MSG_CTP_SHORT_UP);          //补发上短划
                        }
                        msg_enqueue(MSG_CTP_LONG_UP);               //上长划
                        ctp_cb.sta = CTP_STA_MOV_LONG;
                    } else {
                        if (ctp_cb.sta != CTP_STA_MOV_SHORT) {
                            msg_enqueue(MSG_CTP_SHORT_DOWN);        //补发下短划
                        }
                        msg_enqueue(MSG_CTP_LONG_DOWN);             //下长划
                        ctp_cb.sta = CTP_STA_MOV_LONG;
                    }
                    return;
                }
                if (ctp_cb.sta != CTP_STA_MOV_SHORT && my >= CTP_CNT_SHORT_MOV) {
                    if (ctp_cb.y < ctp_cb.sy) {
                        msg_enqueue(MSG_CTP_SHORT_UP);              //上短划
                        ctp_cb.sta = CTP_STA_MOV_SHORT;
                    } else {
                        msg_enqueue(MSG_CTP_SHORT_DOWN);            //下短划
                        ctp_cb.sta = CTP_STA_MOV_SHORT;
                    }
                    return;
                }
            }
            break;

        case CTP_STA_MOV_LONG:
        default:
            break;
        }
    } else {
        if (ctp_cb.sta == CTP_STA_PRESS) {
            msg_enqueue(MSG_CTP_CLICK);                             //单击
        } else if (ctp_cb.sta == CTP_STA_PRESS_LONG) {
            msg_enqueue(MSG_CTP_LONG_LIFT);                         //长按抬起
        }

        ctp_cb.cnt = 0;
        ctp_cb.sta = CTP_STA_IDLE;
    }
}

AT(.com_text.ctp)
void ctp_read_point(void)
{
    bool press = false;

    s32 last_x = ctp_cb.x;
    s32 last_y = ctp_cb.y;
#if (CTP_SELECT == CTP_CST8X)
    press = ctp_cst8x_get_point(&ctp_cb.x, &ctp_cb.y);
#endif
    if (press) {
        if (ctp_cb.flag_last) {
            ctp_cb.last_dx = ctp_cb.x - last_x;
            ctp_cb.last_dy = ctp_cb.y - last_y;
        } else {
            ctp_cb.flag_last = true;
            ctp_cb.last_dx = 0;
            ctp_cb.last_dy = 0;
        }
    } else {
        ctp_cb.flag_last = false;
    }
    TRACE_K(str_xy, press, ctp_cb.x, ctp_cb.y);               //打印坐标
    ctp_msg_deal(press);
}


AT(.com_text.ctp)
void ctp_isr(void)
{
    if (iic_get_flag(IIC_IT_DONE)) {
        iic_clear_done_flag();
        ctp_read_point();
    }
}

//获取坐标，返回触屏点击状态
AT(.com_text.ctp)
bool ctp_get_cur_point(s32 *sx, s32 *sy, s32 *x, s32 *y)
{
    bool press;
    GLOBAL_INT_DISABLE();
    *sx = ctp_cb.sx;
    *sy = ctp_cb.sy;
    *x = ctp_cb.x;
    *y = ctp_cb.y;
    press = (ctp_cb.sta != CTP_STA_IDLE);
    GLOBAL_INT_RESTORE();
    return press;
}

//获取坐标差量，返回触屏点击状态
AT(.com_text.ctp)
bool ctp_get_dxy(s32 *dx, s32 *dy)
{
    bool press;
    GLOBAL_INT_DISABLE();
    press = (ctp_cb.sta != CTP_STA_IDLE);
    if (press) {
        *dx = ctp_cb.x - ctp_cb.sx;
        *dy = ctp_cb.y - ctp_cb.sy;
    }
    GLOBAL_INT_RESTORE();
    return press;
}

//获取起始X,Y坐标
AT(.com_text.ctp)
point_t ctp_get_sxy(void)
{
    point_t pt;
    GLOBAL_INT_DISABLE();
    pt.x = ctp_cb.sx;
    pt.y = ctp_cb.sy;
    GLOBAL_INT_RESTORE();
    return pt;
}

//获取最后一次偏移
AT(.com_text.ctp)
point_t ctp_get_last_dxy(void)
{
    point_t pt;
    GLOBAL_INT_DISABLE();
    pt.x = ctp_cb.last_dx;
    pt.y = ctp_cb.last_dy;
    GLOBAL_INT_RESTORE();
    return pt;
}

//读数据KICK
AT(.com_text.ctp)
void ctp_iic_readkick(void *buf, u8 addr, int len)
{
    iic_dma_start(IIC_RECEIVE_MODE, I2C_READ_ADDR(TP_IIC_ADDR), addr, buf, len);
}

//读数据接口
AT(.com_text.ctp)
bool ctp_iic_read(void *buf, u8 addr, int len)
{
    u32 ticks;
    ctp_iic_readkick(buf, addr, len);
    ticks = tick_get();
    while (!(IICCON0 & BIT(31))) {
        if (tick_check_expire(ticks, 20)) {
            printf("tp_i2c_read time out ERROR\n");
            return false;
        }
    }

    if (!ctp_cb.init) {
        IICCON0 |= BIT(29);                //Clear Pending
    }
    return true;
}

//初始化
void ctp_init(void)
{
    iic_init_typedef iic_init_struct;
    bool res = false;

    memset(&ctp_cb, 0, sizeof(ctp_cb));

    //CTP IO INIT
    port_ctp_init();

    //初始化I2C
    clk_gate2_cmd(CLK_GATE2_IIC, CLK_EN);
    iic_init_struct.clk_source = IIC_CLK_X24MDIV8;
    iic_init_struct.scl_pose_div = 29;      // clk_source div29, it means freq of scl is 100KHz when works.
    iic_init_struct.sda_hold_cnt = 0;
    iic_init_struct.mode_sel = IIC_ROLE_MODE_MASTER;
    iic_init(&iic_init_struct);
    iic_cmd(ENABLE);

    //复位TP/等待模块使能
    port_ctp_reset();
#if (CTP_SELECT == CTP_CST8X)
    res = ctp_cst8x_init();
#endif
    if (!res) {
        TRACE("CTP ERROR!\n");
        return;
    }

    iic_pic_config(ctp_isr, 0, IIC_IT_DONE, ENABLE);

    //INT信号中断
    gpio_edge_pic_config(ctp_int_isr, 0);
}

void ctp_exit(void)
{
    iic_deinit();
    port_ctp_exit();
    gpio_edge_pic_disable();
}

//升级写KICK
void ctp_iic_write_update_kick(u8 dev_addr, u16 addr, u8 *cmd, int len)
{
    iic_dma_start(IIC_SEND_MODE, I2C_WRITE_ADDR(TP_IIC_ADDR), addr, cmd, len);
    delay_ms(2);

}

//升级写
bool ctp_iic_update_write(u8 dev_addr, u16 addr, u8 *cmd, int len)
{
    u32 ticks;
    ctp_iic_write_update_kick(dev_addr, addr, cmd, len);
    ticks = tick_get();
    while (!(IICCON0 & BIT(31))) {
        if (tick_check_expire(ticks, 20)) {
            printf("WRITE UPDATE IIC TOUT\n");
            return false;
        }
    }
    IICCON0 |= BIT(29);                //Clear Pending
    return true;
}

//升级读KICK
void ctp_iic_update_readkick(u8 dev_addr, void *buf, int len)
{
    ctp_iic_readkick(buf, dev_addr, len);
}

//升级读
bool ctp_iic_update_read(u8 dev_addr, void *rbuf, int rlen, u16 w_addr, u8 *w_cmd, int wlen)
{
    ctp_iic_update_write(dev_addr, w_addr, w_cmd, wlen);

    u32 ticks;
    ctp_iic_update_readkick(dev_addr, rbuf, rlen);
    ticks = tick_get();
    while (!(IICCON0 & BIT(31))) {
        if (tick_check_expire(ticks, 20)) {
            printf("READ UPDATE IIC TOUT\n");
            return false;
        }
    }
    IICCON0 |= BIT(29);                //Clear Pending

    delay_ms(2);
    return true;
}



#else
void ctp_init(void) {}
void ctp_exit(void) {}
bool ctp_get_dxy(s32 *dx, s32 *dy) {return false;}
point_t ctp_get_sxy(void) {return (point_t){0, 0};}
point_t ctp_get_last_dxy(void) {return (point_t){0, 0};}
#endif
