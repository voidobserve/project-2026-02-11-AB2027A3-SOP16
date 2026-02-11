#ifndef __WL_VOICE_H
#define __WL_VOICE_H
#include "include.h"

#define WL_VOICE_PAYLOAD_LEN                (4+128)   //2byte presample + 2byte index + 128byte偏差值    
#define WL_VOICE_HEAD_LEN                   (2)       //1byte wireless packet type + 1byte enc data len
#define WL_VOICE_BUF_LEN                    (WL_VOICE_HEAD_LEN + WL_VOICE_PAYLOAD_LEN)
#define WL_VOICE_RING_BUF_LEN               (10*WL_VOICE_PAYLOAD_LEN) 

typedef enum {
    WL_VOICE_STATUS_STOP          = 0x00,
    WL_VOICE_STATUS_START         = 0x01,
} WL_VOICE_STATUS_TYPEDEF;

typedef struct{
    uint8_t voice_status;
    uint8_t exit_clk;
}wl_voice_var_t;

void user_enc_buf_process(uint8_t *buffer, uint32_t size);
void wl_voice_start(void);
void wl_voice_stop(void);
void wl_voice_init(void);
void wl_voice_proc(void);

#endif
