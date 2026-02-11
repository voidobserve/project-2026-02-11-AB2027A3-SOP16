#include "include.h"
#include "usb_com.h"
#include "usb_host_enum.h"

extern void usb_host_in_test(void);

ep_cb_t ep_cb;

usb_handler_cb_t usb_handler_cb;

void usb_ep_callback_register(ep_handler callback, uint8_t ep_index, uint8_t ep_dir)
{
    if (ep_index < EP_CNT) {
        if (ep_dir) {
            ep_cb.eptx_handler[ep_index] = callback;
        } else {
            ep_cb.eprx_handler[ep_index] = callback;
        }
    }
}

AT(.usbdev.com)
void usb_host_process(void)
{
    static u32 tick = 0;

    if (tick_check_expire(tick,1)) {
        tick = tick_get();
        usb_host_in_test();
    }
}

bool usb_event_callback(uint8_t event, spb_wrap_t *spb, uint8_t *data, uint8_t data_len)
{
    printf("usb_event_callback:0x%x\n",event);

    switch (event){

        case MSG_USB_EP1_IN:
            if (ep_cb.eptx_handler[1]) {
                ep_cb.eptx_handler[1]();
            }
            break;

        case MSG_USB_EP1_OUT:
            if (ep_cb.eprx_handler[1]) {
                ep_cb.eprx_handler[1]();
            }
            break;

        case MSG_USB_EP2_IN:
            if (ep_cb.eptx_handler[2]) {
                ep_cb.eptx_handler[2]();
            }
            break;

        case MSG_USB_EP2_OUT:
            if (ep_cb.eprx_handler[2]) {
                ep_cb.eprx_handler[2]();
            }
            break;

        case MSG_USB_EP3_IN:
            if (ep_cb.eptx_handler[3]) {
                ep_cb.eptx_handler[3]();
            }
            break;

        case MSG_USB_EP3_OUT:
            if (ep_cb.eprx_handler[3]) {
                ep_cb.eprx_handler[3]();
            }
            break;

        default:
            return false;
    }

    return true;
}

bool usb_host_init_handler(void)
{
    return usb_host_enum_process();
}

void usb_host_init(void)
{
    usb_handler_cb.init = usb_host_init_handler;
    usb_handler_cb.evt = usb_event_callback;

    usb_handler_register(&usb_handler_cb);

}

void usb_host_deinit(void)
{

}
