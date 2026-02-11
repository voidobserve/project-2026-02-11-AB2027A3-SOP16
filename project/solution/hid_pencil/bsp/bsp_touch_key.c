#include "include.h"
#include "driver_touch_key.h"
#include "bsp_touch_key.h"

#if BSP_TOUCH_KEY_EN

AT(.com_rodata.touch_key_1)
const tkey_ch_t tkey1 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0x00,
    .itrim = 0x11,
};

AT(.com_rodata.touch_key_2)
const tkey_ch_t tkey2 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
};

AT(.com_rodata.touch_key_3)
const tkey_ch_t tkey3 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
};

AT(.com_rodata.touch_key)
const tkey_cfg_t tkey_cfg = {
    .key[0] = &tkey1,
    //.key[0] = NULL,
    //.key[1] = &tkey1,
    .key[1] = NULL,
    //.key[2] = &tkey2,
    .key[2] = NULL,
    //.key[3] = &tkey3,
    .key[3] = NULL,

    //TKTMR Register
    .fil_low = FIL_LOW,
    .fil_high = FIL_HIGH,
    .fil_except = FIL_EXCEPT,
    .fil_val = FIL_VAL,
    .to_except = TO_EXCEPT,

    //TKPTHD Register
    .tkpthd = TKPTHRESH,
    .tkrthd = TKRTHRESH,
    .pto_except = PTO_EXCEPT,

    //TKETHD Register
    .tksthd = SMALLTHD,
    .tklthd = LARGETHD,

    //TKVARI Register
    .tkvthd = 20,

    //TKVARITHD Register
    .tkarthd = 20,
    .tkaethd = SMALLTHD,
    .tkvfil  = 7,
    .tkbadd  = SMALLTHD,

    //TKCON2 Register
     .press_vari_en = 1,
     .rels_vari_en = 1,
     .press_fil_sel = 1,
     .rels_fil_sel = 1,
     .press_vari_thd = 20,
     .rels_vari_thd = 20,
     .to_bcnt_thd = 10,
};

tk_cb_t tk_cb;
u16 tkcnt_tmr[4];       //每个通路的实时TKCNT

//中断调用
AT(.com_text.touch_key.isr)
void touch_key_range_isr(u32 tk_ch, u16 tkcnt)
{
    tkcnt_tmr[tk_ch] = tkcnt;
}

void touch_key_var_init(void)
{
    memset(&tk_cb, 0, sizeof(tk_cb));

    tk_cb.limit = 28;
    tk_cb.range_thresh = TKRTHRESH;
    tk_cb.ch = 0xff;
    tk_cb.fil_except = FIL_EXCEPT;
    tk_cb.bcnt_sfr = (psfr_t)(&TKBCNT);
    tk_cb.range_en = 0;                                 //软件range校准是否使能
    tk_cb.to_cnt = TO_EXCEPT;                           //10秒
}

void touch_key_channel_set(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        tkey_ch_t *ch = (tkey_ch_t *)tkey_cfg.key[i];
        if (ch) {
            if (ch->type == 1) {
                tk_cb.ch = i;
            }
        }
    }
}

void bsp_touch_key_init(void)
{
    touch_key_var_init();

    #if 1
    int res;
    tkey_cfg_t tkey_cfg_v;

    memcpy(&tkey_cfg_v, &tkey_cfg, sizeof(tkey_cfg_t));

    touch_key_channel_set();

    res = touch_key_init(TOUCH_KEY, (void *)&tkey_cfg_v, 1, CLK_TOUCH_KEY_RC2M, 0);

    if (tk_cb.ch != 0xff) {
        if (!res) {
            delay_5ms(5);
        }
    }
    #endif
}

AT(.com_periph.touch.check)
uint8_t touch_key_check(void)
{
    //printf("ch %d\n", tk_cb.ch);
    if (tk_cb.ch == 0xff) {
        return TOUCH_KEY_STA_ERROR;
    }

    if (!touch_key_get_key()) {
        return TOUCH_KEY_STA_ERROR;
    }

    uint16_t tkbcnt = GET_REG_TKBCNT_TKBCNT();
    //uint16_t tkbcnt_avg = GET_REG_TKBCNT_TKBCNT_AVG;

    //printf("0x%08x(%d),0x%08x(%d)\n", tkbcnt, tkbcnt, tkcnt_tmr[tk_cb.ch], tkcnt_tmr[tk_cb.ch]);

    if ((tkcnt_tmr[tk_cb.ch] > tkbcnt) && (tkcnt_tmr[tk_cb.ch] - tkbcnt > TKPTHRESH)) {
        // Key Pressed
        return TOUCH_KEY_STA_PRESSED;
    //} else if ((tkcnt_tmr[tk_cb.ch] > tkbcnt) && (tkcnt_tmr[tk_cb.ch] - tkbcnt < TKRTHRESH)) {
    } else if (ABS(tkcnt_tmr[tk_cb.ch], tkbcnt) < TKRTHRESH) {
        // Key Release
        return TOUCH_KEY_STA_RELEASED;
    }

    return TOUCH_KEY_STA_ERROR;
}

AT(.com_periph.touch.get_key_id)
u8 bsp_get_touch_key_id(void)
{
    uint8_t key_state = touch_key_check();

    if (TOUCH_KEY_STA_ERROR == key_state) {
        return KEY_ID_NO;
    }

    if (TOUCH_KEY_STA_PRESSED == key_state) {
        //printf("ch %d, touch key channel %d, %s\n", tk_cb.ch, GET_REG_TKCON_TKSEL(), TO_STRING(TOUCH_KEY_STA_PREESED));
        return KEY_ID_PP;
    }

//    if (TOUCH_KEY_STA_RELEASED == key_state) {
//        printf("ch %d, touch key channel %d, %s\n", tk_cb.ch, GET_REG_TKCON_TKSEL(), TO_STRING(TOUCH_KEY_STA_RELEASED));
//    }
//
//    if (TOUCH_KEY_STA_ERROR == key_state) {
//        printf("ch %d, touch key channel %d, %s\n", tk_cb.ch, GET_REG_TKCON_TKSEL(), TO_STRING(TOUCH_KEY_STA_ERROR));
//    }


    return KEY_ID_NO;
}

#endif // BSP_TOUCH_KEY_EN
