#include "include.h"
#include "wireless.h"
#include "api_sysclk.h"

#include "bsp_sdadc_voice.h"
#include "utils_ring_buffer.h"

static wl_voice_var_t wl_voice_var AT(.wl.var);
static ring_buf_t wl_voice_ring_buf AT(.wl.ring_buf);
static uint8_t ring_buf[WL_VOICE_RING_BUF_LEN] AT(.wl.ring_buf);

AT(.text.app.wl.var_init)
static void wl_voice_var_init(void)
{
    memset(&wl_voice_var, 0, sizeof(wl_voice_var_t));
}

AT(.com_text.enc.proc)
static void wl_voice_enqueue_enc_buf_proc(uint8_t *buffer, uint32_t size)
{
    utils_ring_buf_write(&wl_voice_ring_buf, buffer, size);
}

AT(.com_text.enc.proc)
void user_enc_buf_process(uint8_t *buffer, uint32_t size)
{
    wl_voice_enqueue_enc_buf_proc(buffer, size);
}

AT(.text.app.proc.ble)
static uint32_t wl_voice_dequeue_enc_buf_proc(uint8_t *buffer, uint32_t size)
{
    return utils_ring_buf_read(&wl_voice_ring_buf, buffer, size);
}

AT(.text.app.proc.ble)
static void wl_voice_data_notify_process(void)
{
    if (wl_voice_var.voice_status != WL_VOICE_STATUS_START) {
        return;
    }

    uint8_t voice_buf[WL_VOICE_BUF_LEN];
    uint32_t enc_len = wl_voice_dequeue_enc_buf_proc(&voice_buf[2], WL_VOICE_PAYLOAD_LEN);

    if (enc_len != WL_VOICE_PAYLOAD_LEN) {
        return;
    }

    voice_buf[0] = WIRELESS_PACKET_TYPE_AUDIO;
    voice_buf[1] = WL_VOICE_PAYLOAD_LEN;
    wireless_send_for_con(wireless_con_handle_get(), voice_buf, sizeof(voice_buf));

}

AT(.text.prod_test.voice_start)
void wl_voice_start(void)
{
    if (wl_voice_var.voice_status == WL_VOICE_STATUS_START || !wireless_con_handle_get()) {
        return;
    }

    printf("wl voice start\n");
    lowpwr_sleep_auto_dis();
	wireless_latency_applied(wireless_con_handle_get(), false);;
    wl_voice_var.voice_status = WL_VOICE_STATUS_START;
    wl_voice_var.exit_clk = sys_clk_get();
    sys_clk_set(SYS_120M);
    adpcm_sample_idx_reset();
    txpkt_flush(&ble_tx);
    bsp_sdadc_voice_start();
}

AT(.text.prod_test.voice_stop)
void wl_voice_stop(void)
{
    if (wl_voice_var.voice_status == WL_VOICE_STATUS_STOP) {
        return;
    }

    printf("wl voice stop\n");
    lowpwr_sleep_auto_en();
    wl_voice_var.voice_status = WL_VOICE_STATUS_STOP;
    sys_clk_set(wl_voice_var.exit_clk);
    bsp_sdadc_voice_stop();
    utils_ring_buf_clear(&wl_voice_ring_buf);
}

AT(.text.app.proc.wl_voice_proc)
void wl_voice_proc(void)
{
    wl_voice_data_notify_process();
}

AT(.text.prod_test.voice_init)
void wl_voice_init(void)
{
    wl_voice_var_init();
    utils_ring_buf_init(&wl_voice_ring_buf, ring_buf, sizeof(ring_buf));
}
