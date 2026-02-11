#ifndef __USB_AUDIO_DESC_H
#define  __USB_AUDIO_DESC_H

#include "include.h"

#define USB_AUDIO_EP_IN_INDEX           2
#define USB_AUDIO_EP_OUT_INDEX          2

#define AUDIO_SPEAKER_SAMPLE            16000                                   /* The sample rate must be the same as that configured for the sdadc */
#define AUDIO_MIC_SAMPLE                16000                                   /* The sample rate must be the same as that configured for the sdadc */

#define AUDIO_ISOOUT_SIZE               (AUDIO_SPEAKER_SAMPLE / 1000 * 1 * 2)   /* Only support Mono and PCM16bits */
#define AUDIO_ISOIN_SIZE                (AUDIO_MIC_SAMPLE / 1000 * 1 * 2)       /* Only support Mono and PCM16bits */
#define AUDIO_TOTAL_NR                  (USB_SPEAKER_EN + USB_MIC_EN)

#define AUDIO_ISOOUT_TYPE               0x09                                    /* bit[1:0] 0:ctrl 1:isoc 2:bulk 3:int */
#define AUDIO_ISOIN_TYPE                0x05                                    /* bit[1:0] 0:ctrl 1:isoc 2:bulk 3:int */

enum{
    UA_SPEAKER_INPUT_ID = 0x01,
    UA_SPEAKER_UNIT_ID,
    UA_SPEAKER_OUTPUT_ID,
    UA_MIC_INPUT_ID,
    UA_MIC_UNIT_ID,
    UA_MIC_OUTPUT_ID,
};


uint8_t *usb_audio_itf_mic_stream_desc_get(uint8_t *length);
uint8_t *usb_audio_itf_speaker_stream_desc_get(uint8_t *length);
uint8_t *usb_audio_itf_header_desc_get(uint8_t *length);

#endif
