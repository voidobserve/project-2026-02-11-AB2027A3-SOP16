#ifndef _USB_COM_H
#define _USB_COM_H

#include "hid_defines.h"

//USB ERROR CODE
#define USB_ERR_OK                          0x01
#define USB_ERR_BUSY                        0x02
#define USB_ERR_PARSE                       0x03
#define USB_ERR_NO_CAP                      0x04
#define USB_ERR_NO_CONN                     0x05
#define USB_ERR_PKT_TOO_LONG                0x06
#define USB_ERR_UNKNOW                      0xff


//USB MSG
#define THREAD_DRIVER_MSG_VENDOR_IN         0x66
#define THREAD_DRIVER_MSG_VENDOR_POLL       0x67
#define THREAD_DRIVER_MSG_HID_IN            0x68
#define THREAD_DRIVER_MSG_CDC_BULK_IN       0x69


typedef void (*ep_handler)(void);

typedef struct {
    ep_handler eptx_handler[EP_CNT];           //EP TX
    ep_handler eprx_handler[EP_CNT];           //EP RX
} ep_cb_t;

extern ep_cb_t ep_cb;

typedef struct _ude_t {
    u8      devaddr;                     //当前设备地址
    u8      cfgval;                      //当前配置
    bool    rem_wakeup;

    volatile u8 error       : 1;    //出错
} ude_cb_t;

extern ude_cb_t ude_cb;

typedef struct {
    void        (*init)         (void);
    void        (*suspend)      (void);
    void        (*resume)       (void);
    void        (*data_out)     (uint8_t *data, uint32_t size);
    uint32_t    (*data_in)      (uint8_t *data, uint32_t size);
    void        (*ready)        (bool ready);
    void        (*sof)          (void);
} usb_usr_cb_t;

void usb_ep_callback_register(ep_handler callback, uint8_t ep_index, uint8_t ep_dir);
void usb_device_init(void);
void usb_ep_reset_deal(void);
void usb_device_process(void);

#endif // _USB_COM_H
