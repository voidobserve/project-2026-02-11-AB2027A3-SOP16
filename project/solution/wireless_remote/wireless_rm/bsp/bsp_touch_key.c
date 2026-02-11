#include "include.h"
#include "driver_touch_key.h"
#include "bsp_touch_key.h"

#if BSP_TOUCH_KEY_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
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
     .press_vari_en = 0,
     .rels_vari_en = 0,
     .press_fil_sel = 0,
     .rels_fil_sel = 0,
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

void bsp_touch_key_init(void)
{
    touch_key_var_init();

    #if 1
    int res;
    tkey_cfg_t tkey_cfg_v;

    memcpy(&tkey_cfg_v, &tkey_cfg, sizeof(tkey_cfg_t));

    touch_key_channel_set();
    
    touch_key_sw_reset();

    res = touch_key_init(TOUCH_KEY, (void *)&tkey_cfg_v, 1, CLK_TOUCH_KEY_RC2M, 0);

    if (tk_cb.ch != 0xff) {
        if (!res) {
            delay_5ms(5);
        }
    }
    #endif
}

AT(.com_rodata.tkey_dbg)
const char tkey_dbg_check[] = "inpcon 0x%08x, TKVARI 0x%08x tkbcnt %d, tkcnt %d,%d,%d,%d\n";

AT(.com_periph.touch.check)
uint8_t touch_key_check(void)
{
    if (tk_cb.ch == 0xff) {
        return TOUCH_KEY_STA_ERROR;
    }

    if (!touch_key_get_key()) {
        return TOUCH_KEY_STA_ERROR;
    }

    uint8_t key_sta_change = 0;
    uint16_t tkbcnt = GET_REG_TKBCNT_TKBCNT();

    #if TRACE_EN
	static uint32_t dbg_tick = 0;

	if (tick_check_expire(dbg_tick, 200)) {
		dbg_tick = tick_get();
	    printf(tkey_dbg_check, INPCON, TKVARI, tkbcnt, tkcnt_tmr[0], tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3]);
	}
    #endif

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
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_get_key_id[] = "ch %d, sel ch %d\n";

AT(.com_periph.touch.get_key_id)
u8 bsp_get_touch_key_id(void)
{
    uint8_t key_value = touch_key_check();

    if (TOUCH_KEY_DET_NO_KEY == key_value) {
        return KEY_ID_NO;
    }

    for (uint8_t ch_idx = 0; ch_idx < SUPPORT_TOUCH_KEY_NUM; ch_idx ++) {
        //if (touch_key_state[ch_idx] != tk_cb.touch_key_sta[ch_idx]) {
        //    touch_key_state[ch_idx] = tk_cb.touch_key_sta[ch_idx];

            if (TOUCH_KEY_STA_PRESSED == tk_cb.touch_key_sta[ch_idx]) {
                //printf(tkey_dbg_get_key_id, ch_idx, GET_REG_TKCON_TKSEL());
                return KEY_ID_PP + ch_idx;
            }

            //if (TOUCH_KEY_STA_RELEASED == tk_cb.touch_key_sta[ch_idx]) {
            //}
        //}
    }

    return KEY_ID_NO;
}

#endif // BSP_TOUCH_KEY_EN
