/**
******************************************************************************************************************************
*
*@file touch_key.c
*
*@brief Source file for touch key example.User need to dymamically adjust tkpthd and tkrthd for each channel according to the
*       actual TKBCNT and TKCNT(you can see both value after macro TOUCH_KEY_TRACE set to 1).
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/

#include "touch_key_example.h"
#include "driver_touch_key.h"

#define TOUCH_KEY_TRACE                 0

#if TOUCH_KEY_TRACE
#define TOUCH_KEY_DUMP(...)              printf(__VA_ARGS__)
#else
#define TOUCH_KEY_DUMP(...)
#endif

static uint8_t touch_key_state[SUPPORT_TOUCH_KEY_NUM];

AT(.com_rodata.touch_key_0)
const tkey_ch_t tkey0 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
    .tkpthd = 28,
    .tkrthd = 15,
};

AT(.com_rodata.touch_key_1)
const tkey_ch_t tkey1 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
    .tkpthd = 28,
    .tkrthd = 15,
};

AT(.com_rodata.touch_key_2)
const tkey_ch_t tkey2 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
    .tkpthd = 28,
    .tkrthd = 15,
};

AT(.com_rodata.touch_key_3)
const tkey_ch_t tkey3 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x14,
    .tkpthd = 28,
    .tkrthd = 15,
};

AT(.com_rodata.touch_key_cfg)
const tkey_cfg_t tkey_cfg = {
    .key[0] = &tkey0,
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

    //if (tk_ch == tk_cb.ch) {
    //    tkey_bcnt_range_exception(&tk_cb, tkcnt);
    //}
}

void touch_key_var_init(void)
{
    memset(&tk_cb, 0, sizeof(tk_cb));

    tk_cb.limit = 28;
    tk_cb.range_thresh = TKRTHRESH;
    tk_cb.ch = 0xff;
    tk_cb.fil_except = FIL_EXCEPT;
    tk_cb.bcnt_sfr = (psfr_t)(&ADDR_REG_TKBCNT);
    tk_cb.range_en = 0;                                 //软件range校准是否使能
    tk_cb.to_cnt = TO_EXCEPT;                           //10秒

    memset(tk_cb.touch_key_sta, TOUCH_KEY_STA_RELEASED, sizeof(tk_cb.touch_key_sta));
    memset(touch_key_state, TOUCH_KEY_STA_RELEASED, sizeof(touch_key_state));
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

void touch_key_init_example(void)
{
    touch_key_var_init();

    int res;
    tkey_cfg_t tkey_cfg_v;

    memcpy(&tkey_cfg_v, &tkey_cfg, sizeof(tkey_cfg_t));

    touch_key_channel_set();
    touch_key_sw_reset();

    res = touch_key_init(TOUCH_KEY, (void *)&tkey_cfg_v, 1, CLK_TOUCH_KEY_RC2M, 0);

    if (tk_cb.ch != 0xff) {
        if (!res) {
            delay_5ms(5);
            //TKBCNT = tkcnt_tmr[tk_cb.ch];
        }
    }
}

AT(.com_periph.touch.check)
uint8_t touch_key_check(void)
{
    if (tk_cb.ch == 0xff) {
        return TOUCH_KEY_DET_NO_KEY;
    }

    if (!touch_key_get_key()) {
        return TOUCH_KEY_DET_NO_KEY;
    }

    uint8_t key_sta_change = 0;
    uint16_t tkbcnt = GET_REG_TKBCNT_TKBCNT();

    TOUCH_KEY_DUMP("tkbcnt:0x%08x, tkcnt:0x%08x,0x%08x,0x%08x,0x%08x\n", tkbcnt, tkcnt_tmr[0], tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3]);

    for (uint8_t ch_idx = 0; ch_idx < SUPPORT_TOUCH_KEY_NUM; ch_idx ++) {
        if ((tkcnt_tmr[ch_idx] > tkbcnt) && (tkcnt_tmr[ch_idx] - tkbcnt > tkey_cfg.key[ch_idx]->tkpthd)) {
            // Key Pressed
            key_sta_change = 1;
            tk_cb.touch_key_sta[ch_idx] =  TOUCH_KEY_STA_PRESSED;
        //} else if (ABS(tkcnt_tmr[ch_idx], tkbcnt) < tkey_cfg.key[ch_idx]->tkrthd) {
        } else if ((tkbcnt - tkcnt_tmr[ch_idx]) < tkey_cfg.key[ch_idx]->tkrthd) {
            // Key Released
            key_sta_change = 1;
            tk_cb.touch_key_sta[ch_idx] = TOUCH_KEY_STA_RELEASED;
        }
    }

    if (key_sta_change) {
        return TOUCH_KEY_DET_CHANGE;
    }

    return TOUCH_KEY_DET_NO_KEY;
}

AT(.com_periph.touch.get_key_id)
void touch_key_scan_example(void)
{
    uint8_t key_value = touch_key_check();

    if (TOUCH_KEY_DET_NO_KEY == key_value) {

        return;
    }

    for (uint8_t ch_idx = 0; ch_idx < SUPPORT_TOUCH_KEY_NUM; ch_idx ++) {
        if (touch_key_state[ch_idx] != tk_cb.touch_key_sta[ch_idx]) {
            touch_key_state[ch_idx] = tk_cb.touch_key_sta[ch_idx];

            if (TOUCH_KEY_STA_PRESSED == tk_cb.touch_key_sta[ch_idx]) {
                printf("ch %d, touch key channel %d, %s\n", ch_idx, GET_REG_TKCON_TKSEL(), TO_STRING(TOUCH_KEY_STA_PRESSED));
            }

            if (TOUCH_KEY_STA_RELEASED == tk_cb.touch_key_sta[ch_idx]) {
                printf("ch %d, touch key channel %d, %s\n", ch_idx, GET_REG_TKCON_TKSEL(), TO_STRING(TOUCH_KEY_STA_RELEASED));
            }
        }
    }
}

