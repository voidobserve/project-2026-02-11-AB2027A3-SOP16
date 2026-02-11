#include "include.h"


#define TRACE_EN                            1
#define TRACE_RATE_EN                       1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


static struct {
    uint16_t        con_handle;
    #if TRACE_RATE_EN
    uint32_t        stat_poll_tick;
    uint32_t        stat_pkt_cnt;
    #endif
} wireless_cb;


static void wireless_event_packet_handler(uint8_t event_type, uint8_t *packet, uint8_t size)
{
    switch (event_type) {
#if WIRELESS_CONN_EN
        case WIRELESS_EVENT_CONNECTED:
            TRACE("connected - con_handle: 0x%04x\n", GET_LE16(&packet[0]));
            wireless_cb.con_handle = GET_LE16(&packet[0]);
            break;

        case WIRELESS_EVENT_DISCONNECTED:
            TRACE("disconnected - con_handle: 0x%04x  reason: 0x%02x\n", GET_LE16(&packet[0]), packet[2]);
            wireless_cb.con_handle = 0;
            wireless_adv_ctrl(true);
            break;
#endif

#if WIRELESS_SCAN_EN
        case WIRELESS_EVENT_ADV_REPORT: {
            wireless_adv_report_t *adv_report_p = (wireless_adv_report_t *)&packet[0];

            TRACE("adv report - type: 0x%02x rssi: %d addr: ", adv_report_p->addr_type, adv_report_p->rssi);
            TRACE_R(adv_report_p->addr, 6);

            /* adv_report_p->report equ PeripheralDevice::wl_adv_data */
            TRACE("adv data[%d]: ", adv_report_p->report_len);
            TRACE_R(adv_report_p->report, adv_report_p->report_len);
        } break;

        case WIRELESS_EVENT_SCAN_TIMEOUT:
            TRACE("-->WIRELESS_EVENT_SCAN_TIMEOUT\n");
            wireless_scan_ctrl_2(false, WIRELESS_SCAN_TIMEOUT);
            break;
#endif

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

        #if TRACE_RATE_EN
        wireless_cb.stat_pkt_cnt++;
        #endif
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
}

void wireless_service_process(void)
{
    #if 0 /* TEST CODE */
    {
        static uint32_t test_tick;
        if (tick_check_expire(test_tick, 1000)) {
            test_tick = tick_get();
            if (wireless_cb.con_handle) {
                wireless_send_for_con(wireless_cb.con_handle, (u8 *)0x18000, 16);
            }
        }
    }
    #endif

    #if TRACE_RATE_EN
    {
        if (tick_check_expire(wireless_cb.stat_poll_tick, 1000)) {
            wireless_cb.stat_poll_tick = tick_get();
            if (wireless_cb.con_handle) {
                TRACE("rate: %dpkts/s\n", wireless_cb.stat_pkt_cnt);
                wireless_cb.stat_pkt_cnt = 0;
            }
        }
    }
    #endif
}
