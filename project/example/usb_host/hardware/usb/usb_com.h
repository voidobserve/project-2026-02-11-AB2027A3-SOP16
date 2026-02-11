#ifndef _USB_COM_H
#define _USB_COM_H

typedef void (*ep_handler)(void);

typedef struct {
    ep_handler eptx_handler[EP_CNT];           //EP TX
    ep_handler eprx_handler[EP_CNT];           //EP RX
} ep_cb_t;

extern ep_cb_t ep_cb;

void usb_ep_callback_register(ep_handler callback, uint8_t ep_index, uint8_t ep_dir);
void usb_host_init(void);
void usb_host_process(void);

#endif // _USB_COM_H
