#ifndef _CTP_H
#define _CTP_H

void ctp_init(void);
void ctp_exit(void);
bool ctp_get_cur_point(s32 *sx, s32 *sy, s32 *x, s32 *y);
bool ctp_get_dxy(s32 *dx, s32 *dy);
point_t ctp_get_sxy(void);
point_t ctp_get_last_dxy(void);
point_t ctp_get_rxy(widget_t *widget);
void ctp_sw_read_point(void);

void ctp_iic_readkick(void *buf, u8 addr, int len);
bool ctp_iic_read(void *buf, u8 addr, int len);


bool ctp_cst8x_init(void);
void ctp_cst8x_readkick(void);
bool ctp_cst8x_get_point(s32 *x, s32 *y);



#endif
