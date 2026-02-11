#include "include.h"
#include "driver_sddac.h"
#include "res.h"
#include "api_codec.h"
#include "driver_gpio.h"

/*****************************************************/
//Only WAV_TEST can use ADPCM format test.
#define WAV_PACKED_FORMAT   1   // 0: PCM
                                // 1: ADPCM
/*****************************************************/

#define PCM16_2_ADPCM_BLOCK(x)              ((x-2)/4+4)
#define ADPCM_BLOCK_2_PCM16_BYTES(x)        ((x-4)*4+2)
#define ADPCM_SAMPLE_2_BLOCK(x)             ((x-1)/2+4)

typedef struct {
    u8 *addr;
    u32 pos;
    u32 len;
} spi_res_cb_t;

spi_res_cb_t spires_wav_cb;

typedef struct {
    u8 sta;
    u8 nch;
    u32 spr;
#if WAV_PACKED_FORMAT
    u16 block_sample_size; // Number of samples in one block for IMA-ADPCM
#endif // WAV_PACKED_FORMAT
    u8 *buf;
} wav_res_cb_t;

wav_res_cb_t wavres_cb;

void tone_play_volume_set(int8_t vol);
uint16_t tone_play_volume_get(void);
void tone_play_volume_proc(int16_t *dest, int16_t *src, uint16_t len);

u8 wav_res_buf[512];
#if WAV_PACKED_FORMAT
// One ADPCM Block min 32B sample, max 512B.
// That means decode buf_size(PCM 16bits) min 114B, max 2034B.
u16 adpcm_block_bytes;
u8 adpcm_decode_obuf[2][ADPCM_BLOCK_2_PCM16_BYTES(512)] AT(.codec.adpcm.buf);
#endif // WAV_PACKED_FORMAT

AT(.text.spr.table)
const uint16_t tbl_sample_rate[] = {
    8000,
    12000,
    16000,
    24000,
    48000
};

AT(.text.spr.table)
const uint16_t tbl_sample_rate_sfr[] = {
    SDDAC_NORMAL_SPR_8k,
    SDDAC_NORMAL_SPR_12k,
    SDDAC_NORMAL_SPR_16k,
    SDDAC_NORMAL_SPR_24k,
    SDDAC_NORMAL_SPR_48k
};

//功放mute脚控制，用户补充代码
void wav_audio_output_mute(bool mute)
{
    if (mute) {
        // gpio_set_bits(GPIOB_REG, GPIO_PIN_2);
    } else {
        // gpio_reset_bits(GPIOB_REG, GPIO_PIN_2);
    }
}

void wav_dec_buf_init(void)
{
#if WAV_PACKED_FORMAT
    uint16_t sp_len = ADPCM_BLOCK_2_PCM16_BYTES(512);
    for (int16_t i = 0; i < sp_len; i++) {
        adpcm_decode_obuf[0][i] = 0;
        adpcm_decode_obuf[1][i] = 0;
    }
#else
    uint16_t sp_len = sizeof(wav_res_buf);
    for (int16_t i = 0; i < sp_len; i++) {
       wav_res_buf[0] = 0;
    }
#endif  //WAV_PACKED_FORMAT
}



AT(.text.aviobuf)
uint8_t sddac_spr_cvt(uint sample_rate)
{
	uint i;
	for (i=0; i < (sizeof(tbl_sample_rate) / 2); i++) {
		if (sample_rate == tbl_sample_rate[i]) {
			return tbl_sample_rate_sfr[i];
		}
	}
	printf("--->wav_res_spr_err\n");
	return 0xff;
}


AT(.text.spires)
int wav_res_stream_read(void *buf, unsigned int size)
{
    int rlen;
    spi_res_cb_t *s = &spires_wav_cb;

    if ((s->pos + size) >= s->len) {
        rlen = s->len - s->pos;
    } else {
        rlen = size;
    }
    if (rlen) {
        memcpy(buf, s->addr + s->pos, rlen);
    }
    s->pos += rlen;

    return rlen;
}

AT(.text.spires)
bool wav_res_is_play(void)
{
    if (wavres_cb.sta) {
        return true;
    }
    return false;
}

uint16_t wav_res_play_kick(u32 addr, u32 len)
{
    int ret;
    spi_res_cb_t *s = &spires_wav_cb;
    wav_res_cb_t *w = &wavres_cb;

    s->addr = (u8 *)addr;
    s->len = len;
    s->pos = 0;

    memset(&wavres_cb, 0, sizeof(wav_res_cb_t));
    w->buf = wav_res_buf;
    ret = wav_res_stream_read(w->buf, 512);

    if (ret != 512) {
        return 0;
    }

#if WAV_PACKED_FORMAT
    return wav_res_analize(w->buf, &w->nch, &w->spr, &w->block_sample_size);
#else
    return wav_res_analize(w->buf, &w->nch, &w->spr, NULL);
#endif // WAV_PACKED_FORMAT
}

void wav_res_init(void)
{
    wav_audio_output_mute(1);

    //dac init
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init(GPIOA_REG, &gpio_init_structure);

    FUNO_PA2SEL(FO_AUDIO);                                    //Map dac output PA2
    
    wavres_cb.sta = 0;

    wav_audio_output_mute(0);
}

void wav_res_deinit(void)
{
    wavres_cb.sta = 0;
    wav_audio_output_mute(1);
    gpio_deinit(GPIOA_REG, GPIO_PIN_2);
    sddac_cmd(DISABLE);
    clk_gate0_cmd(CLK_GATE0_DAC, CLK_DIS);
}

bool tone_play_wav_test(void)
{
    uint16_t offset;
    u8 *ptr;
    u16 len;

    wav_dec_buf_init();

#if WAV_PACKED_FORMAT
    offset = wav_res_play_kick(RES_BUF_RINGTONE_ADPCM_WAV, RES_LEN_RINGTONE_ADPCM_WAV);
#else
    offset = wav_res_play_kick(RES_BUF_RINGTONE_WAV, RES_LEN_RINGTONE_WAV);
#endif // WAV_PACKED_FORMAT

    if(offset && (wavres_cb.nch == 1)){         //only support mono
        printf("offset:%d\n",offset);
        printf("nch:%d\n",wavres_cb.nch);
        printf("spr:%d\n",wavres_cb.spr);

        wavres_cb.sta = 1;
        spires_wav_cb.pos = offset;

#if WAV_PACKED_FORMAT
        if (wavres_cb.block_sample_size) {
            printf("ADPCM Block: %d(Samples)\n", wavres_cb.block_sample_size);
        }

        /* Check block size enough to decode. */
        adpcm_block_bytes = ADPCM_SAMPLE_2_BLOCK(wavres_cb.block_sample_size);

        printf("ADPCM Block Size: %d\n", adpcm_block_bytes);

        wav_res_stream_read(wavres_cb.buf, adpcm_block_bytes);

        adpcm_decode_block((uint8_t*)&adpcm_decode_obuf[0], wavres_cb.buf, wavres_cb.block_sample_size);
        tone_play_volume_proc((s16*)&adpcm_decode_obuf[0][0], (s16*)&adpcm_decode_obuf[0][0], wavres_cb.block_sample_size);
        ptr = (uint8_t*)&adpcm_decode_obuf[0];
        len = wavres_cb.block_sample_size;
#else
        wav_res_stream_read(wavres_cb.buf, 512);
        tone_play_volume_proc((s16*)&wavres_cb.buf[0], (s16*)&wavres_cb.buf[0], 256);
        ptr = wavres_cb.buf;
        len = 256;               // HalfWord
#endif // WAV_PACKED_FORMAT

        wav_audio_output_mute(1);
        GPIOADE &= ~BIT(2);
        
        clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
        clk_gate0_cmd(CLK_GATE0_DAC, CLK_EN);

        sddac_init_typedef sddac_config;

        sddac_config.dma_buf = (uint8_t*)ptr;
        sddac_config.dma_size = len;
        sddac_config.sample_rate = sddac_spr_cvt(wavres_cb.spr);

        if(sddac_config.sample_rate == 0xFF){
            return false;
        }

        sddac_init(&sddac_config);

        sddac_cmd(ENABLE);
        delay_5ms(10);
        GPIOADE |= BIT(2);
        wav_audio_output_mute(0);
        return true;
    }

    return false;
}


AT(.text.spires)
void wav_res_dec_process(int done)
{
    int ret;
    wav_res_cb_t *w = &wavres_cb;

#if WAV_PACKED_FORMAT
    uint32_t half_done_point = wavres_cb.block_sample_size / 2;
    uint32_t all_done_point = half_done_point;

    if(wavres_cb.block_sample_size % 2){
        half_done_point = wavres_cb.block_sample_size / 2 + 1;
        all_done_point = wavres_cb.block_sample_size - half_done_point;
    }

    if(done == SDDAC_IT_ALL_DONE){
        tone_play_volume_proc((s16*)&adpcm_decode_obuf[0][half_done_point * 2], (s16*)&adpcm_decode_obuf[1][half_done_point * 2], all_done_point);
    }else{
        ret = wav_res_stream_read(w->buf, adpcm_block_bytes);
        if (ret < adpcm_block_bytes) {
            w->sta = 0;
            GPIOADE &= ~BIT(2);
            sddac_cmd(DISABLE);
            return;
        }
        adpcm_decode_block((uint8_t*)&adpcm_decode_obuf[1], w->buf, wavres_cb.block_sample_size);
        tone_play_volume_proc((s16*)&adpcm_decode_obuf[0][0], (s16*)&adpcm_decode_obuf[1][0], half_done_point);
    }
#else
    if(done == SDDAC_IT_ALL_DONE){
        ret = wav_res_stream_read(&w->buf[256], 256);
        tone_play_volume_proc((s16*)&w->buf[256], (s16*)&w->buf[256], 128);
    }else{
        ret = wav_res_stream_read(w->buf, 256);
        tone_play_volume_proc((s16*)&w->buf[0], (s16*)&w->buf[0], 128);
    }
    if (ret <= 0) {
        w->sta = 0;
        GPIOADE &= ~BIT(2);
        sddac_cmd(DISABLE);
        return;
    }
#endif // WAV_PACKED_FORMAT
}

void wav_res_play_proc(void)
{
    if (wavres_cb.sta == 1){
        if(sddac_get_flag(SDDAC_IT_ALL_DONE)){
            sddac_clear_flag(SDDAC_IT_ALL_DONE);
            if(wav_res_is_play()){
                wav_res_dec_process(SDDAC_IT_ALL_DONE);
            }
        }
        if(sddac_get_flag(SDDAC_IT_HALF_DONE)){
            sddac_clear_flag(SDDAC_IT_HALF_DONE);
            if(wav_res_is_play()){
                wav_res_dec_process(SDDAC_IT_HALF_DONE);
            }
        }
    }
}
