#ifndef _USB_VENDOR_H
#define _USB_VENDOR_H

#include "usb_vendor_desc.h"

typedef struct _udv_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
    uint8_t idle_time[VENDOR_REPORT_ID_MAX + 1];    //+1,处理没有report id时的情况
} udv_t;

void usb_vendor_init(void);
void usb_vendor_deinit(void);
void usb_vendor_in_kick(void);

void usb_vendor_ep_reset(void);
void usb_vendor_sof_isr(void);
void udv_init(void);
void usb_vendor_set_idle(uint8_t report_id, uint8_t time);
uint8_t usb_vendor_get_idle(uint8_t report_id);
void usb_vendor_setvalid(bool valid);
bool usb_vendor_is_valid(void);
void usb_vendor_in_callback(void);

#endif // _USB_VENDOR_H
