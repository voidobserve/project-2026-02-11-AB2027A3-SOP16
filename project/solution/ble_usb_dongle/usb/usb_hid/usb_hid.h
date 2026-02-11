#ifndef _USB_HID_H
#define _USB_HID_H

#include "usb_hid_desc.h"

typedef struct _udh_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
    uint8_t idle_time[HID_REPORT_ID_MAX + 1];       //+1,处理没有report id时的情况
} udh_t;


/* Note: HID each packet length should be according with HID_EP_IN_MAX_SIZE */
typedef struct PACKED {
    uint8_t btn_left    : 1;
    uint8_t btn_right   : 1;
    uint8_t btn_central : 1;
    uint8_t reserved    : 5;
    int8_t  wheel;
    int8_t  x;
    int8_t  y;
} hid_report_info_typedef;


void usb_hid_init(void);
void usb_hid_deinit(void);
uint32_t usb_hid_buffer_push(uint8_t *buf, uint32_t size);
void usb_hid_enable(bool sta);
void udh_init(void);
void usb_int_ep_reset(void);
void ude_hid_setvalid(bool valid);
void usb_hid_send_prepare(uint8_t *buf, uint32_t *size);
void usb_hid_in_callback(void);
void usb_hid_receive_callback(uint8_t *buf, uint32_t size);
void usb_hid_set_idle(uint8_t report_id, uint8_t time);
uint8_t usb_hid_get_idle(uint8_t report_id);

#endif // _USB_HID_H
