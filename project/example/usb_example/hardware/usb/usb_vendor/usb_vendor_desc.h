#ifndef _USB_VENDOR_DESC_H
#define _USB_VENDOR_DESC_H


#define USB_VENDOR_EP_IN_INDEX          1
#define USB_VENDOR_EP_OUT_INDEX         1

#define VENDOR_REPORT_DESC_SIZE         100
#define VENDOR_EP_OUT_MAX_SIZE          64
#define VENDOR_EP_IN_MAX_SIZE           64

#define VENDOR_REPORT_ID_MAX            0       //report id总个数，get idle和set idle需要,没有report id时填0


uint8_t *usb_vendor_itf_desc_get(uint8_t *length);
uint8_t *usb_vendor_hid_desc_get(uint8_t *length);
uint8_t *usb_vendor_report_desc_get(uint8_t *length);

#endif
