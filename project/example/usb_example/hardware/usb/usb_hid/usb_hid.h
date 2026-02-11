#ifndef _USB_HID_H
#define _USB_HID_H

#include "usb_hid_desc.h"

typedef struct _udh_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
    uint8_t idle_time[HID_REPORT_ID_MAX + 1];       //+1,处理没有report id时的情况
} udh_t;


void usb_hid_deinit(void);
void usb_hid_init(void);
void usb_int_ep_reset(void);
void usb_hid_setvalid(bool valid);
void usb_hid_in_callback(void);
void usb_hid_process(void);
void usb_hid_set_idle(uint8_t report_id, uint8_t time);
uint8_t usb_hid_get_idle(uint8_t report_id);

#endif // _USB_HID_H
