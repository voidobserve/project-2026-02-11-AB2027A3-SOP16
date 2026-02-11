#include "include.h"
#include "driver_sdadc.h"
#include "driver_sddac.h"
#include "usb_audio.h"
#include "usb_com.h"
#include "usb_audio_desc.h"
#include "bsp_sddac.h"
#include "bsp_sdadc.h"
#include "utils_ring_buffer.h"
#include "bsp_audio_dec.h"

#if USB_AUDIO_EN

#define RING_BUF_LEN           (2*1024)

uda_t uda_0 AT(.usb_buf.usb);

typedef struct {
    ring_buf_t ring_buf;
    s16 speed;
} au_cbuf_cb_t;

u8 usbmic_compli_xp_sys = 0;

#if USB_MIC_EN

AT(.usbdev.com.table)
static const u8 mic_vol_tbl[3] = {
    0x00,//min
    0x00,//max
    0x01,//res
};

AT(.usbdev.com.table)
static const u8 mic_vol_tbl1[3] = {
    0xf9,//min
    0x1e,//max
    0x50,//res
};

epcb_t epcb2_tx AT(.usb_buf.usb);
u8 ep2_isoc_in[AUDIO_ISOIN_SIZE + 2];
u8 ude_micbuf[RING_BUF_LEN] AT(.usb_buf.aubuf.mic);
au_cbuf_cb_t aumic_cb AT(.usb_buf.aubuf.mic);
#endif

#if USB_SPEAKER_EN
AT(.usbdev.com.table)
static const u8 spk_vol_tbl[3] = {
    0x00,//min
    0xff,//max
    0x01,//res
};

epcb_t epcb2_rx AT(.usb_buf.usb);
u8 ep2_isoc_out[AUDIO_ISOOUT_SIZE + 4];
u8 ude_aubuf[AUDIO_ISOOUT_SIZE] AT(.usb_buf.aubuf.spk);
u8 ude_speakerbuf[RING_BUF_LEN] AT(.usb_buf.aubuf.spk);
au_cbuf_cb_t auspeaker_cb AT(.usb_buf.aubuf.spk);
#endif

void ude_isoc_tx_process(void);
void uda_run_loop_execute(void);

#if USB_MIC_EN
AT(.usbdev.com)
void uda_set_isocin_flag(u8 val)
{
    uda_t *uda = &uda_0;
    uda->flag_isocin_int = val;
}

AT(.usbdev.com)
uint8_t uda_get_isocin_flag(void)
{
    uda_t *uda = &uda_0;
    return uda->flag_isocin_int;
}

AT(.text.func.usbdev)
void uda_mic_start_do(void)
{
    /* Start mic */
    printf("[UAC] mic start\n");

    sdadc_cmd(ENABLE);
}

AT(.text.func.usbdev)
void uda_mic_stop_do(void)
{
    /* Sotp mic */

    printf("[UAC] mic stop\n");

    sdadc_cmd(DISABLE);
}

// API for upper layer
AT(.usbdev.com)
void usb_audio_mic_stream(void *ibuf, u32 sample)
{
    bool active_tx_flag = 0;
    uint32_t ret = 0;
    if (aumic_cb.ring_buf.data_len < AUDIO_ISOIN_SIZE) {
        active_tx_flag = 1;
    }

    ret = utils_ring_buffer_write(&aumic_cb.ring_buf, ibuf, (sample << 1));

    if (active_tx_flag && ret) {
        ude_isoc_tx_process();
    }
}

AT(.usbdev.com)
static void uda_mic_stop(uda_t *uda)
{
    printf("%s\n", __func__);

#if (USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)
    uda_mic_stop_do();
#endif //(USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)

    uda->flag_isocin_en = 0;

#if LE_VOICE_REMOTE_DONGLE_EN
    bsp_audio_dec_deinit();
#endif //LE_VOICE_REMOTE_DONGLE_EN
}

AT(.usbdev.com)
static void uda_mic_start(uda_t *uda)
{
    printf("%s\n", __func__);

    uda->flag_isocin_en = 1;

    memset(uda->isoc_in->buf, 0, AUDIO_ISOIN_SIZE);

    if (usb_ep_transfer(uda->isoc_in)) {
        usb_ep_start_transfer(uda->isoc_in, AUDIO_ISOIN_SIZE);
    }

    memset(&aumic_cb, 0, sizeof(aumic_cb));
    utils_ring_buffer_init(&aumic_cb.ring_buf, ude_micbuf, RING_BUF_LEN);

#if (USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)
    uda_mic_start_do();
#endif //(USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)

#if LE_VOICE_REMOTE_DONGLE_EN
    bsp_audio_dec_init();
#endif //LE_VOICE_REMOTE_DONGLE_EN
}


///USB AUDIO iso in process(mic)
AT(.usbdev.com)
void ude_isoc_tx_process(void)
{
    int mlen;
    au_cbuf_cb_t *s = &aumic_cb;
    uda_t *uda = &uda_0;
    u16 frame_size = AUDIO_ISOIN_SIZE;

    if (usb_ep_transfer(uda->isoc_in)) {
        mlen = frame_size + s->speed;
        if (!utils_ring_buffer_read(&s->ring_buf, uda->isoc_in->buf, mlen)) {
            //no data
            return;
        }

        if (uda->mic_mute) {
            memset(uda->isoc_in->buf, 0, mlen);
        }
        usb_ep_start_transfer(uda->isoc_in, mlen);
    }
}

AT(.usbdev.com)
bool uda_get_mic_mute(void)
{
    u8 buf = uda_0.mic_mute;

    return usb_ep0_start_transfer(&buf, 1);
}

AT(.usbdev.com)
void uda_set_mic_mute(u8 val)
{
    uda_0.mic_mute = val;
    printf("mic set mute: %0d\n", val);
}

AT(.usbdev.com)
void uda_set_mic_volume(u16 val)
{
    printf("mic set volume: %04x\n", val);
    uda_0.mic_vol = val;
}

AT(.usbdev.com)
bool uda_get_mic_volume(u8 req)
{
    u8 buf[2] = {0};
    u8 value;

    if(usbmic_compli_xp_sys) {
        uda_t *uda = &uda_0;
        if (req == UA_GET_CUR) {
            value = uda->mic_vol;
        } else {
            value = mic_vol_tbl[(req&0x7f) - 2];
        }
        buf[0] = value;
        buf[1] = 0x00;
        if (req != UA_GET_RES) {
            value = mic_vol_tbl1[(req&0x7f) - 2];
            buf[1] = value;
        }
        if (req == UA_GET_CUR) {
            buf[0] = 0x00;
            buf[1] = 0x09;
        }
    } else {
        uda_t *uda = &uda_0;
        if (req == UA_GET_CUR) {
            value = uda->mic_vol;
            printf("value4 = %02x\n",value);
        } else {
            value = mic_vol_tbl[(req&0x7f) - 2];
            printf("value5 = %02x\n",value);
        }
        buf[0] = value;
        buf[1] = 0x00;
        if (req != UA_GET_RES) {
            buf[1] = 0x80;
        }
    }

    return usb_ep0_start_transfer(buf, 2);
}

#endif

#if USB_SPEAKER_EN
AT(.usbdev.com)
void uda_set_isocout_flag(u8 val)
{
    uda_t *uda = &uda_0;
    uda->flag_isocout_int = val;
}

AT(.usbdev.com)
uint8_t uda_get_isocout_flag(void)
{
    uda_t *uda = &uda_0;
    return uda->flag_isocout_int;
}

AT(.text.func.usbdev)
void uda_speaker_start_do(void)
{
    /* Start mic */
    printf("[UAC] speaker start\n");

    sddac_cmd(ENABLE);
}

AT(.text.func.usbdev)
void uda_speaker_stop_do(void)
{
    printf("[UAC] speaker stop\n");

    sddac_cmd(DISABLE);
}

AT(.usbdev.com)
static void uda_speaker_stop(uda_t *uda)
{
    printf("%s\n", __func__);

    uda_speaker_stop_do();

    uda->flag_isocout_en = 0;
}

AT(.usbdev.com)
static void uda_speaker_start(uda_t *uda)
{
    printf("%s\n", __func__);

    uda->flag_isocout_en = 1;

    memset(&auspeaker_cb, 0x00, sizeof(auspeaker_cb));

    utils_ring_buffer_init(&auspeaker_cb.ring_buf, ude_speakerbuf, RING_BUF_LEN);

    uda_speaker_start_do();
}

///USB AUDIO iso out process(speaker)
AT(.usbdev.com)
void ude_isoc_rx_process(void)
{
    uda_t *uda = &uda_0;
    u8 *aubuf = ude_aubuf;

    if (uda->spk_mute) {
        memset(aubuf, 0, AUDIO_ISOOUT_SIZE);
    } else {
        memcpy(aubuf, ep2_isoc_out, AUDIO_ISOOUT_SIZE);
    }

    usb_ep_clear_rx_fifo(uda->isoc_out);

    utils_ring_buffer_write(&auspeaker_cb.ring_buf, aubuf, AUDIO_ISOOUT_SIZE);
}

AT(.usbdev.com)
bool uda_get_spk_volume(u8 req)
{
    u8 buf[2]={0};
    u8 value;

    uda_t *uda = &uda_0;
    if (req == UA_GET_CUR) {
        value = uda->spk_vol;
    } else {
        value = spk_vol_tbl[(req&0x7f) - 2];
    }
    buf[0] = value;
    buf[1] = 0x00;
    if (req != UA_GET_RES) {
        buf[1] = 0x80;
    }

    return usb_ep0_start_transfer(&buf, 2);
}

AT(.usbdev.com)
void uda_set_spk_mute(u8 val)
{
    uda_0.spk_mute = val;
    printf("spk set mute: %d\n", val);
}

AT(.usbdev.com)
void uda_set_spk_volume(u16 val)
{
    printf("spk set volume: %04x\n", val);
    uda_0.spk_vol = (u8)val;
}

AT(.usbdev.com)
void uda_speaker_sync(void)
{
    au_cbuf_cb_t *s = &auspeaker_cb;

    switch(s->speed) {
        case -2:
            if(s->ring_buf.data_len >= 1152) {
                s->speed = 0;
            }
            break;
        case 0:
            if(s->ring_buf.data_len >= 1152) {
                s->speed = 2;
            } else if(s->ring_buf.data_len < 384) {
                s->speed = -2;
            }
            break;
        case 2:
            if(s->ring_buf.data_len < 1152) {
                s->speed = 0;
            }
            break;
    }
}

AT(.usbdev.com)
bool uda_get_spk_mute(void)
{
    u8 buf = uda_0.spk_mute;

    return usb_ep0_start_transfer(&buf, 1);
}


// API for upper layer
AT(.com_text.usbdev)
void usb_audio_speaker_stream(void *obuf, u32 *sample)
{
    au_cbuf_cb_t *s = &auspeaker_cb;
    int mlen;

    mlen = AUDIO_ISOOUT_SIZE + s->speed;
    if (!utils_ring_buffer_read(&s->ring_buf, obuf, mlen)) {
        memset(obuf, 0x00, mlen);
    }

    *sample = mlen >> 1;
}
#endif

void usb_audio_process(void)
{
    uda_run_loop_execute();
}

///USB AUDIO endpoint reset
AT(.usbdev.com)
void usb_isoc_ep_reset(void)
{
    uda_t *uda = &uda_0;

#if USB_MIC_EN
    if (uda->isoc_in->index) {
        usb_ep_reset(uda->isoc_in);
    }
#endif // USB_MIC_EN

#if USB_SPEAKER_EN
    if (uda->isoc_out->index) {
        usb_ep_reset(uda->isoc_out);
    }
#endif // USB_SPEAKER_EN
}

AT(.usbdev.com)
void uda_run_loop_execute(void)
{
    uda_t *uda = &uda_0;

#if USB_MIC_EN
    if (uda->flag_isocin_en != uda->flag_isocin_int) {
        if (uda->flag_isocin_int) {
            delay_5ms(2);           //delay 10ms启动MIC, 等待SET CUR命令先处理（例如设采样率）
            uda_mic_start(uda);
        } else {
            uda_mic_stop(uda);
        }
    }
#endif // USB_MIC_EN

#if USB_SPEAKER_EN
    if (uda->flag_isocout_en != uda->flag_isocout_int) {
        if (uda->flag_isocout_int) {
            uda_speaker_start(uda);
        } else {
            uda_speaker_stop(uda);
        }
    }
#endif // USB_MIC_EN
}

void uda_exit(uda_t *uda)
{
#if USB_MIC_EN
    if (uda->flag_isocin_en) {
        uda_mic_stop(uda);
    }
#endif
}

void uda_init(void )
{
    epcb_t *epcb;

    memset(&uda_0, 0, sizeof(uda_t));

    uda_t *uda = &uda_0;

#if USB_MIC_EN
    uda_0.isoc_in  = &epcb2_tx;
    memset(uda->isoc_in, 0x00, sizeof(epcb_t));
    usb_ep_callback_register(ude_isoc_tx_process, USB_AUDIO_EP_IN_INDEX, EP_DIR_IN);
     //usb audio mic ep init
    memset(ep2_isoc_in,  0, sizeof(ep2_isoc_in));
    epcb = uda->isoc_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_AUDIO_EP_IN_INDEX;
    epcb->type = EP_TYPE_ISOC;
    epcb->epsize = AUDIO_ISOIN_SIZE;
    epcb->buf = ep2_isoc_in;
    epcb->first_pkt = 0;
    epcb->xlen = 0;

    if (usb_ep_get_max_len(epcb->index) < AUDIO_ISOIN_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    uda->mic_vol = 0x6a;        //min:0x00, max:0xff
    uda->mic_mute = 0;
    uda->flag_isocin_int = 0;
    uda->flag_isocin_en = 0;
#endif

#if USB_SPEAKER_EN
    uda_0.isoc_out = &epcb2_rx;
    memset(uda->isoc_out, 0x00, sizeof(epcb_t));
    usb_ep_callback_register(ude_isoc_rx_process, USB_AUDIO_EP_OUT_INDEX, EP_DIR_OUT);
    //usb audio speaker ep init
    memset(ep2_isoc_out, 0, sizeof(ep2_isoc_out));
    epcb = uda->isoc_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_AUDIO_EP_OUT_INDEX;
    epcb->type = EP_TYPE_ISOC;
    epcb->epsize = AUDIO_ISOOUT_SIZE;
    epcb->buf = ep2_isoc_out;

    if (usb_ep_get_max_len(epcb->index) < AUDIO_ISOOUT_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    uda->spk_vol = 0x80;        //min:0x00, max:0xff
    uda->spk_mute = 0;
    uda->flag_isocout_int = 0;
    uda->flag_isocout_en = 0;
#endif

    printf("uda_init success\n");
}

void usb_audio_init(void)
{
#if USB_SPEAKER_EN
    bsp_sddac_init();
#endif // USB_SPEAKER_EN

#if (USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)
    bsp_sdadc_init();
#endif //(USB_MIC_EN && !LE_VOICE_REMOTE_DONGLE_EN)
}

void usb_audio_deinit(void)
{
    uda_t *uda = &uda_0;

    uda_exit(uda);
}

#endif
