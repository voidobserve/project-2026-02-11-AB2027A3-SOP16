#include "include.h"
#include "usb_com.h"
#include "usb_vendor.h"
#include "usb_usr_vendor.h"
#include "fota_proc.h"
#include "driver_gpio.h"

#define TRACE_EN            0
#if TRACE_EN
#define TRACE(...)          my_printf("[VENDOR] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#define USB_USR_VENDOR_CACHE_MOUSE                  5
#define USB_USR_VENDOR_CACHE_APP                    2


static void usb_usr_vendor_init(void);
static void usb_usr_vendor_data_out(uint8_t *data, uint32_t size);
static uint32_t usb_usr_vendor_data_in(uint8_t *data, uint32_t size);
static void usb_usr_vendor_ready(bool ready);
static void usb_usr_vendor_sof(void);

static uint8_t usb_usr_vendor_queue_push(void *data, uint32_t length, list_t *owner, uint8_t report_id);
static uint32_t usb_usr_vendor_queue_pop(void *packet);
static void usb_usr_vendor_queue_clear(void);
#if AB_FOT_CHL_USB_EN
static int usb_usr_vendor_app_send(uint8_t *buf, uint16_t size);
static int usb_usr_vendor_app_max_pkt_get(uint32_t *size);
#endif

static list_t usb_usr_vendor_queue_send;
static list_t usb_usr_vendor_queue_mouse;
static list_t usb_usr_vendor_queue_app;

static usb_vendor_rep_mouse_t usb_usr_vendor_pkts_pool_mouse[USB_USR_VENDOR_CACHE_MOUSE];
static usb_vendor_rep_app_t usb_usr_vendor_pkts_pool_app[USB_USR_VENDOR_CACHE_APP];

usb_usr_cb_t usb_usr_vendor_cb = {
    /* init     */  usb_usr_vendor_init,
    /* suspend  */  NULL,
    /* resume   */  NULL,
    /* data_out */  usb_usr_vendor_data_out,
    /* data_in  */  usb_usr_vendor_data_in,
    /* ready    */  usb_usr_vendor_ready,
    /* sof      */  usb_usr_vendor_sof,
};


static void usb_usr_vendor_init(void)
{
    usb_usr_vendor_queue_send.first = NULL;
    usb_usr_vendor_queue_send.last  = NULL;

    list_pool_init(
        &usb_usr_vendor_queue_mouse,
        usb_usr_vendor_pkts_pool_mouse,
        sizeof(usb_usr_vendor_pkts_pool_mouse[0]),
        sizeof(usb_usr_vendor_pkts_pool_mouse) / sizeof(usb_usr_vendor_pkts_pool_mouse[0])
    );

    list_pool_init(
        &usb_usr_vendor_queue_app,
        usb_usr_vendor_pkts_pool_app,
        sizeof(usb_usr_vendor_pkts_pool_app[0]),
        sizeof(usb_usr_vendor_pkts_pool_app) / sizeof(usb_usr_vendor_pkts_pool_app[0])
    );

#if AB_FOT_CHL_USB_EN
    ab_fota_cb_t param;
    param.fota_send_packet_handle = usb_usr_vendor_app_send;
    param.fota_mtu_check_handle = usb_usr_vendor_app_max_pkt_get;
    fota_init(&param, FOTA_CHL_USB);
#endif
}

static void usb_usr_vendor_data_out(uint8_t *data, uint32_t size)
{
    TRACE("data out[%d]:\n", size);
    TRACE_R(data, size);

#if AB_FOT_CHL_USB_EN
    if (data[0] == HID_REP_ID_APP) {
        hid_rep_app_t *pkt = (hid_rep_app_t *)&data[1];
        fota_recv_packet_handle(pkt->data, pkt->length, FOTA_CHL_USB);
    }
#endif
}

static uint32_t usb_usr_vendor_data_in(uint8_t *data, uint32_t size)
{
    uint32_t elt_size = usb_usr_vendor_queue_pop(data);

    if (elt_size) {
        TRACE("data in[%d]:\n", elt_size);
        TRACE_R(data, elt_size);
    }

    #if 0
    if (elt_size > size) {
        printf("[ERR] vendor pkts length no match!\n");
        while (1);
    }
    #endif

    return elt_size;
}

static void usb_usr_vendor_ready(bool ready)
{
    if (ready) {
        usb_usr_vendor_queue_clear();
    }
}

AT(.com_text.usb.vendor.usr)
static void usb_usr_vendor_sof(void)
{
    thread_driver_msg_post(THREAD_DRIVER_MSG_VENDOR_POLL);
}

/////////////////////////////////////////////////////////////////////////////////////

AT(.com_text.usb.vendor.queue)
static uint8_t usb_usr_vendor_queue_push(void *data, uint32_t length, list_t *owner, uint8_t report_id)
{
    /* example only polling when usb connected */
    #if 0
    if (!ude_cb.connected) {
        return USB_ERR_NO_CONN;
    }
    #endif

    if (!usb_vendor_is_valid()) {
        return USB_ERR_BUSY;
    }

    GLOBAL_INT_DISABLE();

    usb_vendor_rep_t *free = (usb_vendor_rep_t *)list_pop_front(owner);
    uint8_t status = USB_ERR_NO_CAP;

    if (free != NULL) {
        free->owner   = owner;
        free->length  = length + 1;
        free->data[0] = report_id;
        memcpy(&free->data[1], data, length);
        list_push_back(&usb_usr_vendor_queue_send, (list_hdr_t *)free);
        status = USB_ERR_OK;

        thread_driver_msg_post(THREAD_DRIVER_MSG_VENDOR_IN);
    }

    GLOBAL_INT_RESTORE();

    return status;
}

AT(.com_text.usb.vendor.queue)
static uint32_t usb_usr_vendor_queue_pop(void *packet)
{
    GLOBAL_INT_DISABLE();

    usb_vendor_rep_t *elt = (usb_vendor_rep_t *)list_pop_front(&usb_usr_vendor_queue_send);
    uint32_t result = 0;

    if (elt != NULL) {
        if (packet != NULL) {
            memcpy(packet, &elt->data, elt->length);
        }
        result = elt->length;
        list_push_back(elt->owner, (list_hdr_t *)elt);
    }

    GLOBAL_INT_RESTORE();

    return (result);
}

AT(.com_text.usb.vendor.queue)
static void usb_usr_vendor_queue_clear(void)
{
    GLOBAL_INT_DISABLE();
    while (usb_usr_vendor_queue_pop(NULL));
    GLOBAL_INT_RESTORE();
}

#if AB_FOT_CHL_USB_EN

static int usb_usr_vendor_app_send(uint8_t *buf, uint16_t size)
{
    return (USB_ERR_OK == usb_usr_vendor_app_write(buf, size))? 0: -1;
}

static int usb_usr_vendor_app_max_pkt_get(uint32_t *size)
{
    *size = sizeof(hid_rep_app_t) - offsetof(hid_rep_app_t, data);

    return 0/* No error */;
}

#endif // AB_FOT_CHL_USB_EN

/////////////////////////////////////////////////////////////////////////////////////

AT(.com_text.usb.vendor.user)
uint8_t usb_usr_vendor_mouse_write(hid_rep_mouse_t *mouse)
{
    return usb_usr_vendor_queue_push(mouse, sizeof(hid_rep_mouse_t), &usb_usr_vendor_queue_mouse, HID_REP_ID_MOUSE);
}

uint8_t usb_usr_vendor_app_write(uint8_t *buf, uint32_t size)
{
    hid_rep_app_t pdu;

    if (size > sizeof(pdu)) {
        return USB_ERR_PKT_TOO_LONG;
    }

    pdu.length = size;
    memcpy(pdu.data, buf, size);
    memset(pdu.data + size, 0x00, sizeof(pdu.data) - size);

    return usb_usr_vendor_queue_push(&pdu, sizeof(pdu), &usb_usr_vendor_queue_app, HID_REP_ID_APP);
}

void usb_usb_vendor_sof_poll(void)
{
    static u32 dir_cnt = 0;
    static hid_rep_mouse_t mouse;

    #define TEST_PERIOD     80
    #define TEST_STEP       5

    if (gpio_read_bit(GPIOB_REG, GPIO_PIN_6)) {
        return;
    }

    dir_cnt++;
    if (dir_cnt >= (TEST_PERIOD * 4)) {
        dir_cnt = 0;
    }

    if (dir_cnt < (TEST_PERIOD * 1)) {
        mouse.x = TEST_STEP;
        mouse.y = 0;
    } else if (dir_cnt < (TEST_PERIOD * 2)) {
        mouse.x = 0;
        mouse.y = -TEST_STEP;
    } else if (dir_cnt < (TEST_PERIOD * 3)) {
        mouse.x = -TEST_STEP;
        mouse.y = 0;
    } else {
        mouse.x = 0;
        mouse.y = TEST_STEP;
    }

    usb_usr_vendor_mouse_write(&mouse);
}
