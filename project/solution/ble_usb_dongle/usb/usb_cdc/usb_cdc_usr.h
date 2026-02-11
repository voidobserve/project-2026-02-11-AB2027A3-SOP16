#ifndef __USB_CDC_USR_H
#define __USB_CDC_USR_H


typedef struct {
    void        (*init)         (void);
    void        (*data_out)     (uint8_t *data, uint32_t size);
    uint32_t    (*data_in)      (uint8_t *data, uint32_t size);
} usb_usr_cdc_cb_t;

extern usb_usr_cdc_cb_t usb_usr_cdc_cb;

#endif
