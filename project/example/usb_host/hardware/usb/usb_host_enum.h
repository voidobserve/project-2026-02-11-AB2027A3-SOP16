#ifndef _USB_HOST_ENUM_H
#define _USB_HOST_ENUM_H

#include "include.h"

typedef struct {
    epcb_t in_ep;
    epcb_t out_ep;
} uhe_hid_t;

void usb_host_enter_process(void);
void usb_host_exit_process(void);
bool usb_host_enum_process(void);

#endif
