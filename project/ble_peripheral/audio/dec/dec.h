#ifndef _DEC_H_
#define _DEC_H_
#include "include.h"
#include "driver_sddac.h"

/////////////////SIN
enum {
    INDEX_SIN_RES_RING = 0,
    INDEX_SIN_RES_MAX,
};

uint32_t sin_res_sinDate16_addr_get(void);
uint32_t sin_res_sinDate16_len_get(void);

void sin_res_init(void);
void sin_res_deinit(void);
bool sin_res_play(int16_t *buf, uint32_t buf_len);
uint8_t sin_res_play_proc(SDDAC_IT_TYPEDEF done_type);

/////////////////MP3
void mp3_res_init(void);
void mp3_res_deinit(void);
bool mp3_res_play(uint32_t addr, uint32_t len);
uint8_t mp3_res_play_proc(SDDAC_IT_TYPEDEF done_type);

/////////////////WAV
void wav_res_init(void);
void wav_res_deinit(void);
bool wav_res_play(uint32_t addr, uint32_t len);
uint8_t wav_res_play_proc(SDDAC_IT_TYPEDEF done_type);
#endif  //_DEC_H_
