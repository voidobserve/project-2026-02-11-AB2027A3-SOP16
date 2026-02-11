#ifndef __USB_USR_VENDOR_H
#define __USB_USR_VENDOR_H


typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    struct PACKED {
        uint8_t             report_id;
        hid_rep_mouse_t     mouse;
    } data;
} usb_vendor_rep_mouse_t;

typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    struct PACKED {
        uint8_t             report_id;
        hid_rep_app_t       app;
    } data;
} usb_vendor_rep_app_t;

typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    uint8_t                 data[];
} usb_vendor_rep_t;

extern usb_usr_cb_t usb_usr_vendor_cb;


uint8_t usb_usr_vendor_mouse_write(hid_rep_mouse_t *mouse);
uint8_t usb_usr_vendor_app_write(uint8_t *buf, uint32_t size);


#endif // __USB_USR_VENDOR_H
