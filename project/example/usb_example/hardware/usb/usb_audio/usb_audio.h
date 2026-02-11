#ifndef _USB_AUDIO_H
#define _USB_AUDIO_H

typedef struct _uda_t {
#if USB_MIC_EN
    epcb_t *isoc_in;                         //mic endpoint
    u8 flag_isocin_int;
    u8 flag_isocin_en;
    u16 mic_vol;
    u8 mic_mute;
#endif
#if USB_SPEAKER_EN
    epcb_t *isoc_out;                        //speaker endpoint
    u8 flag_isocout_int;
    u8 flag_isocout_en;
    u8 spk_vol;
    u8 spk_mute;
#endif
} uda_t;


void usb_isoc_ep_reset(void);
void usb_audio_init(void);
void usb_audio_deinit(void);
void usb_audio_process(void);

void usb_audio_set_isocin_flag(u8 val);
uint8_t usb_audio_get_isocin_flag(void);
bool usb_audio_get_mic_mute(void);
bool usb_audio_get_mic_volume(u8 req);
void usb_audio_set_mic_mute(u8 val);
void usb_audio_set_mic_volume(u16 val);
void usb_audio_mic_stream(void *ibuf, u32 sample);

bool usb_audio_get_spk_mute(void);
bool usb_audio_get_spk_volume(u8 req);
void usb_audio_set_isocout_flag(u8 val);
uint8_t usb_audio_get_isocout_flag(void);
void usb_audio_set_spk_mute(u8 val);
void usb_audio_set_spk_volume(u16 val);
void usb_audio_speaker_stream(void *obuf, u32 *sample);

#endif // _USB_AUDIO_H
