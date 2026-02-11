#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_usr_hid.h"

#if USB_HID_EN

//udh: usb device hid
udh_t udh_0 AT(.usb_buf.usb);
epcb_t ep_hid_tx AT(.usb_buf.usb);
epcb_t ep_hid_rx AT(.usb_buf.usb);

u8 ep_hid_buf_out[HID_EP_OUT_MAX_SIZE + 4] AT(.usb_buf.hid);          // CRC
u8 ep_hid_buf_in[HID_EP_IN_MAX_SIZE] AT(.usb_buf.hid);


AT(.usbdev.com)
void usb_hid_setvalid(bool valid)
{
    udh_t *udh = &udh_0;
    udh->valid = valid;

    if (usb_usr_hid_cb.ready) {
        usb_usr_hid_cb.ready(valid);
    }
}

bool usb_hid_is_valid(void)
{
    return udh_0.valid;
}

void usb_hid_set_idle(uint8_t report_id, uint8_t time)
{
    if (report_id <= HID_REPORT_ID_MAX) {
        udh_0.idle_time[report_id] = time;
    }
}

uint8_t usb_hid_get_idle(uint8_t report_id)
{
    if (report_id > HID_REPORT_ID_MAX) {
        return 0;
    } else {
        return udh_0.idle_time[report_id];
    }
}

AT(.usbdev.com)
void usb_hid_out_callback(void)
{
    udh_t *udh = &udh_0;

    u16 size = usb_ep_get_rx_len(udh->int_out);

    if (usb_usr_hid_cb.data_out) {
        usb_usr_hid_cb.data_out(udh->int_out->buf, size);
    }

    usb_ep_clear_rx_fifo(udh->int_out);
}


///USB hid endpoint reset
AT(.usbdev.com)
void usb_int_ep_reset(void)
{
    udh_t *udh = &udh_0;

    /* Any usb class can't always use ep0, so it't un-init when index is zere */

    if (udh->int_in->index) {
        usb_ep_reset(udh->int_in);
    }

    if (udh->int_out->index) {
        usb_ep_reset(udh->int_out);
    }

    usb_hid_setvalid(false);
}

///USB HID 初始化
void usb_hid_init(void)
{
    epcb_t *epcb;
    udh_t *udh = &udh_0;

    memset(udh, 0, sizeof(udh_t));

    udh_0.int_in = &ep_hid_tx;
    udh_0.int_out = &ep_hid_rx;

    memset(udh->int_in, 0x00, sizeof(epcb_t));
    memset(udh->int_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(usb_hid_in_callback, USB_HID_EP_IN_INDEX, 1);
    usb_ep_callback_register(usb_hid_out_callback, USB_HID_EP_OUT_INDEX, 0);

    memset(ep_hid_buf_in,  0, sizeof(ep_hid_buf_in));
    epcb = udh->int_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_HID_EP_IN_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_IN_MAX_SIZE;
    epcb->buf = ep_hid_buf_in;
    usb_ep_init(epcb);

    memset(ep_hid_buf_out,  0, sizeof(ep_hid_buf_out));
    epcb = udh->int_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_HID_EP_OUT_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_OUT_MAX_SIZE;
    epcb->buf = ep_hid_buf_out;
    usb_ep_init(epcb);

    usb_hid_setvalid(false);

    if (usb_usr_hid_cb.init) {
        usb_usr_hid_cb.init();
    }
}


AT(.usbdev.com)
void usb_hid_in_callback(void)
{
    udh_t *udh = &udh_0;
    u32 size = 0;

    if (!udh->valid) {
        return;
    }

    if (usb_device_is_suspend()) {
        usb_device_resume();
    }

    if (usb_ep_transfer(udh->int_in)) {
        if (usb_usr_hid_cb.data_in) {
            size = usb_usr_hid_cb.data_in(udh->int_in->buf, HID_EP_IN_MAX_SIZE);
        }
        if (size) {
            usb_ep_start_transfer(udh->int_in, size);
        }
    }
}


void usb_hid_process(void)
{
#if 0
    static u32 delay_cnt = 0;

    /* Change trigger condiction, by key or other */
    if (tick_check_expire(delay_cnt, 5000)) {
        delay_cnt = tick_get();
        thread_driver_msg_post(THREAD_DRIVER_MSG_HID_IN);
        printf("usb_hid_in_callback\n");
    }
#endif
}

void usb_hid_deinit(void)
{
    usb_hid_setvalid(false);
}

#endif
