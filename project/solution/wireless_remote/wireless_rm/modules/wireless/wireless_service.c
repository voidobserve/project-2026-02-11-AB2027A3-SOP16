#include "include.h"
#include "api_wireless.h"
#include "wireless.h"

#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

struct {
    uint16_t        con_handle;
} wireless_cb;

uint16_t wireless_con_handle_get(void)
{
    return wireless_cb.con_handle;
}

static void wireless_event_packet_handler(uint8_t event_type, uint8_t *packet, uint8_t size)
{
    switch (event_type) {
        case WIRELESS_EVENT_CONNECTED: {
            TRACE("connected - con_handle: 0x%04x\n", GET_LE16(&packet[0]));
            wireless_cb.con_handle = GET_LE16(&packet[0]);

            wireless_addr_info_typedef wireless_addr_info;
            wireless_addr_info.valid = true;
            memcpy(&wireless_addr_info.address_type, &packet[2], 7);
            bsp_wireless_link_info_write(&wireless_addr_info);
        } break;

        case WIRELESS_EVENT_DISCONNECTED: {
            TRACE("disconnected - con_handle: 0x%04x  reason: 0x%02x\n", GET_LE16(&packet[0]), packet[2]);
            wireless_addr_info_typedef wireless_addr_info;
            bsp_wireless_link_info_read(&wireless_addr_info);
            wireless_cb.con_handle = 0;
            if (wireless_addr_info.valid) {
                wireless_adv_ctrl(true);
            } else {
                //按键操作重新配对的断连不自动发广播
            }
        } break;

        default:
            break;
    }
}

AT(.com_text.wireless.transmit)
static void wireless_receive_handler(uint16_t con_handle, uint8_t *packet, uint8_t size)
{
    if (con_handle == wireless_cb.con_handle) {
        // TRACE("rx(%d):\n", size);
        // TRACE_R(packet, size);
    }

    wireless_rxbuf_release(packet);
}

#if SYS_SLEEP_EN
void wireless_enter_sleep_hook(void)
{
    if (wireless_cb.con_handle) {
        wireless_latency_applied(wireless_cb.con_handle, true);
    }

    if (wireless_adv_state_get()) {
        /* Nothing to do */
    }
}

void wireless_exit_sleep_hook(void)
{
    if (wireless_cb.con_handle) {
        wireless_latency_applied(wireless_cb.con_handle, false);
    }

    if (wireless_adv_state_get()) {
        /* Nothing to do */
    }
}
#endif

void wireless_service_init(void)
{
    printf("%s\n", __func__);

    memset(&wireless_cb, 0x00, sizeof(wireless_cb));

    wireless_event_handler_register(wireless_event_packet_handler);
    wireless_receive_handler_register(wireless_receive_handler);
    wl_voice_init();
}

void wireless_service_process(void)
{
    wl_adv_proc();
    wl_hid_proc();
    wl_voice_proc();
}
