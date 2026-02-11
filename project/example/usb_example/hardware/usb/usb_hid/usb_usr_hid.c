#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_usr_hid.h"


#define TRACE_EN            0
#if TRACE_EN
#define TRACE(...)          my_printf("[VENDOR] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

void usb_usr_hid_data_out(uint8_t *buf, uint32_t size);
uint32_t usb_usr_hid_data_in(uint8_t *buf, uint32_t size);


usb_usr_cb_t usb_usr_hid_cb = {
    /* init     */  NULL,
    /* suspend  */  NULL,
    /* resume   */  NULL,
    /* data_out */  usb_usr_hid_data_out,
    /* data_in  */  usb_usr_hid_data_in,
    /* ready    */  NULL,
    /* sof      */  NULL,
};

void usb_usr_hid_data_out(uint8_t *buf, uint32_t size)
{
     printf("hid data out[%d]:\n", size);
     print_r(buf, size);
}

AT(.com_text.usb.hid)
uint32_t usb_usr_hid_data_in(uint8_t *buf, uint32_t size)
{
    static u8 state = 0;
    static hid_rep_keyboard_t hid_keyboard_report_cb;
    uint32_t in_size;

    switch (state) {
        case 0:
            memset((u8 *)&hid_keyboard_report_cb, 0x00, sizeof(hid_rep_keyboard_t));
            /* Ctrl + A */
            hid_keyboard_report_cb.left_ctrl = 1;
            hid_keyboard_report_cb.key_list[0] = 0x04;
            break;

        case 1:
            /* Release */
            memset((u8 *)&hid_keyboard_report_cb, 0x00, sizeof(hid_rep_keyboard_t));
            break;

        default:
            /* finish */
            state = 0;
            return 0;
    }

    memcpy(buf, &hid_keyboard_report_cb, sizeof(hid_rep_keyboard_t));
    in_size = sizeof(hid_rep_keyboard_t);

    state = (state + 1) % 3;

    return in_size;
}

