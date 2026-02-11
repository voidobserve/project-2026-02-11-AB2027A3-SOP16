#include "include.h"
#include "usb_com.h"
#include "usb_host_enum.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define UHE_TIMEOUT_FIRST       800                 //第一包命令800ms超时
#define UHE_TIMEOUT             300                 //300ms超时

#define SETUP_LEN               8                   //Setup包长度为8


///Get Conf State
enum {
    GET_CONF_LEN = 0,
    GET_CONF_TYPE,
};

///Enum Step
enum {
    ENUM_STEP_START = 0,
    ENUM_STEP_RESET_1,
    ENUM_STEP_RESET_2,
    ENUM_STEP_GET_DEVDESC_1,
    ENUM_STEP_RESET_3,
    ENUM_STEP_SET_ADDR,
    ENUM_STEP_GET_DEVDESC_2,
    ENUM_STEP_GET_CONFDESC_1,
    ENUM_STEP_GET_CONFDESC_2,
    ENUM_STEP_SET_CONF_2,

    ENUM_STEP_HID_SET_IDLE,
    ENUM_STEP_GET_REPORT_DESC,

    ENUM_STEP_DONE,
};

static const u8 std_get_device_descriptor_1[]       = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00}; //获取设备描述符
static const u8 std_get_device_descriptor_2[]       = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00}; //获取设备描述符
static const u8 std_set_addr[]                      = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //设置设备地址
static const u8 std_get_configuration_descriptor[]  = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00}; //获取配置描述符
static const u8 std_set_configuration[]             = {0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //设置配置描述符
static const u8 std_clear_feature[]                 = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //Clear Feature

static const u8 std_get_report_descriptor[]         = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x7F, 0x00}; //获取HID报表描述符
static const u8 hid_set_idle_cmd[]                  = {0x21, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //HID Set Idle

uhe_t uhe_0;
uhe_hid_t uhe_hid;

u8 hid_in_buf[64];
u8 hid_out_buf[64];


///获取数据
static u8 do_get_rxdata(uhe_t *uhe)
{
    u8 rdata = 0;

    if (uhe->xlen) {
        rdata = *uhe->xptr++;
        uhe->xlen--;
        if (uhe->xlen == 0) {
            usb_host_ep0_in_transfer(uhe);
        }
    }

    return rdata;
}

///跳过数据
static void do_skip_rxdata(uhe_t *uhe, uint len)
{
    while (uhe->xlen && len) {
        if (uhe->error) {
            break;
        }
        if (uhe->xlen > len) {
            uhe->xptr += len;
            uhe->xlen -= len;
            len = 0;
        } else {
            len -= uhe->xlen;
            uhe->xlen = 0;
            if (uhe->xlen == 0) {
                usb_host_ep0_in_transfer(uhe);
            }
        }
    }
}

void usb_host_hid_in_callback(void)
{
    printf("usb_host_hid_in_callback:");

    u16 size = usb_ep_get_rx_len(&uhe_hid.in_ep);

    printf("size:%d\n",size);

    print_r(uhe_hid.in_ep.buf, size);

    usb_ep_clear_rx_fifo(&uhe_hid.in_ep);
}

void usb_host_hid_out_callback(void)
{
     printf("usb_host_hid_out_callback:");
}

///Get Device Descriptor
static bool do_get_device_descriptor(uhe_t *uhe)
{
    u8 *buf = uhe->buf;
    uhe->spb.len = buf[0];
    uhe->epsize = buf[7];

    if (uhe->epsize != 8 && uhe->epsize != 16 &&uhe->epsize != 32 && uhe->epsize != 64) {
        return false;
    }
    return true;
}

///Get Configuration Descriptor
static bool do_get_configuration_descriptor(uhe_t *uhe)
{
    uint get_conf_state;
    uint rxdata, desc_len;
    uint ep_addr, ep_attr;

    get_conf_state = GET_CONF_LEN;

    while (uhe->xlen) {
        if (uhe->error) {
            return false;
        }
        rxdata = do_get_rxdata(uhe);

        switch (get_conf_state) {
        case GET_CONF_LEN:
            desc_len = rxdata;
            get_conf_state = GET_CONF_TYPE;
            break;

        case GET_CONF_TYPE:
            switch (rxdata) {
            case CONFIGURATION_DESCRIPTOR:
                if (desc_len != 9) {
                    TRACE("Configuration descriptor len err\n");
                    return false;
                }
                uhe->spb.len = GET_LE16(uhe->xptr);
                uhe->cfg_val = uhe->xptr[3];
                do_skip_rxdata(uhe, desc_len - 2);
                get_conf_state = GET_CONF_LEN;
                break;

            case INTERFACE_DESCRIPTOR:
                if (desc_len != 9) {
                    TRACE("Interface descriptor len err\n");
                    return false;
                }
                do_skip_rxdata(uhe, desc_len - 2);
                get_conf_state = GET_CONF_LEN;
                break;

            case ENDPOINT_DESCRIPTOR:
                if (desc_len != 7) {
                    TRACE("Endpoint descriptor len err\n");
                    return false;
                }
                ep_addr = uhe->xptr[0];
                ep_attr = uhe->xptr[1];
                if (ep_addr & 0x80) {
                    uhe_hid.in_ep.index = ep_addr & 0x7F;
                    uhe_hid.in_ep.dir = EP_DIR_IN;
                    uhe_hid.in_ep.type = ep_attr;
                    uhe_hid.in_ep.epsize = GET_LE16(&uhe->xptr[2]);
                } else {
                    uhe_hid.out_ep.index = ep_addr;
                    uhe_hid.out_ep.dir = EP_DIR_OUT;
                    uhe_hid.out_ep.type = ep_attr;
                    uhe_hid.out_ep.epsize = GET_LE16(&uhe->xptr[2]);
                }
                do_skip_rxdata(uhe, desc_len - 2);
                get_conf_state = GET_CONF_LEN;
                TRACE("EP %02x: %02x\n", ep_addr, ep_attr);
                break;

            case HID_DESCRIPTOR:
                printf("-->HID_DESCRIPTOR\n");
                if (desc_len != 9) {
                    TRACE("Hid descriptor len err\n");
                    return false;
                }
                do_skip_rxdata(uhe, desc_len - 2);
                get_conf_state = GET_CONF_LEN;
                break;

            default:
                TRACE("Descriptor type err:0x%x\n",rxdata);
                return false;
            }
            break;

        default:
            return false;
        }
    }
    return true;
}

static bool do_get_hid_report_descriptor(uhe_t *uhe)
{
    print_r(uhe->buf, uhe->xlen);

    return true;
}

///Get Descriptor
static bool do_get_descriptor_for_device(uhe_t *uhe)
{
    spb_wrap_t *spb = &uhe->spb;
    uint desc_type = BYTE1(spb->val);

    switch (desc_type) {
        case DEVICE_DESCRIPTOR:
            printf("DEVICE_DESCRIPTOR\n");
            //1: 设备描述符
            return do_get_device_descriptor(uhe);

        case CONFIGURATION_DESCRIPTOR:
            printf("CONFIGURATION_DESCRIPTOR\n");
            //2: 配置描述符
            return do_get_configuration_descriptor(uhe);

        default:
            break;
    }
    return false;
}

static bool do_get_descriptor_for_interface(uhe_t *uhe)
{
    spb_wrap_t *spb = &uhe->spb;
    uint desc_type = BYTE1(spb->val);

    switch (desc_type) {
        case HID_REPORT_DESCRIPTOR:
            printf("--->HID_REPORT_DESCRIPTOR\n");
            return do_get_hid_report_descriptor(uhe);

        default:
            break;
    }
    return false;
}

///Get Configuration
static bool do_get_configuration(uhe_t *uhe)
{
    return true;
}


///处理标准命令，主机数据为向为in
static bool do_standard_in(uhe_t *uhe)
{
    spb_wrap_t *spb = &uhe->spb;

    switch (spb->recipient) {
    case ST_REQ_DEVICE:
        switch (spb->req) {
        case UR_GET_DESCRIPTOR:
            return do_get_descriptor_for_device(uhe);

        case UR_GET_CONFIGURATION:
            return do_get_configuration(uhe);

        default:
            break;
        }
        break;

    case ST_REQ_INTERFACE:
         switch (spb->req) {
            case UR_GET_DESCRIPTOR:
                return do_get_descriptor_for_interface(uhe);
         }
        break;

    case ST_REQ_ENDPOINT:
    default:
        break;
    }
    return false;
}

///处理标准命令，主机数据方向为out
static bool do_standard_out(uhe_t *uhe)
{
    spb_wrap_t *spb = &uhe->spb;

    switch (spb->recipient) {
    case ST_REQ_DEVICE:
        switch (spb->req) {
        case UR_SET_ADDRESS:
            //05: SetAddress
            uhe->set_faddr = 1;
            return true;

        case UR_SET_CONFIGURATION:
            //09: SetConfiguration
            return true;

        default:
            break;
        }
        break;

    case ST_REQ_INTERFACE:
        break;

    case ST_REQ_ENDPOINT:
        switch (spb->req) {
        case UR_CLEAR_FEATURE:
            //01: ClearFeature
            return true;

        default:
            break;
        }
        break;

    default:
        break;
    }
    return false;
}

///处理类命令，主机数据为向为in
static bool do_class_in(uhe_t *uhe)
{
    spb_wrap_t *spb = &uhe->spb;

    switch (spb->recipient) {
    case ST_REQ_DEVICE:
        break;

    case ST_REQ_INTERFACE:
        break;

    case ST_REQ_ENDPOINT:
        break;

    default:
        break;
    }
    return false;
}

///处理类命令，主机数据方向为out
static bool do_class_out(uhe_t *uhe)
{
    return false;
}

///发送一次完成的请求
static bool do_request(uhe_t *uhe)
{
    uint type;
    spb_wrap_t *spb = &uhe->spb;

    usb_host_ep0_setup_transfer(uhe);

    type = spb->type;

    if (spb->dir == EP_DIR_IN) {
        //Host In
        bool res;

        usb_host_ep0_in_transfer(uhe);

        if (type == ST_TYPE_STANDARD) {
            res = do_standard_in(uhe);             //Standard in
        } else if (type == ST_TYPE_CLASS) {
            res = do_class_in(uhe);                //Class in
        } else {
            return false;
        }
        if (res && uhe->stall == 0) {
            uhe->xlen = 0;
            usb_host_ep0_out_transfer(uhe);                       //OUT 一个空包，结束Setup
        }
        return res;
    } else {
        //Host Out
        bool res;
        if (type == ST_TYPE_STANDARD) {
            res = do_standard_out(uhe);             //Standard out
        } else if (type == ST_TYPE_CLASS) {
            res = do_class_out(uhe);                //Class out
        } else {
            res = true;                             //Vendor out
        }
        if (res) {
            usb_host_ep0_in_transfer(uhe);                        //接收一个空包
        }
        return res;
    }
}

///Clear Feature
void usb_host_clear_feature(uhe_t *uhe, u8 epaddr)
{
    spb_wrap_t *spb = &uhe->spb;
    memcpy(spb, std_clear_feature, SETUP_LEN);
    spb->index = epaddr;
    do_request(uhe);
}

///USB Host Enum 开始枚举
bool uhe_start(uhe_t *uhe)
{
    printf("-->uhe_start\n");

    static u8 again_enum_flag;
    uint again_enum_delay = 20;
    uint retry_cnt = 3;
    uint start_delay = 5;
    uint enum_step = 0;

    spb_wrap_t *spb = &uhe->spb;

    uhe->error = 0;


    while (!uhe->error) {
        if (!dev_is_online(DEV_USB)) {
            return false;
        }
        if (uhe->set_faddr) {
            uhe->set_faddr = 0;
            usb_device_addr_set(uhe->devaddr);
        }
        printf("enum_step = %d\n",enum_step);
        switch (enum_step++) {
        case ENUM_STEP_START:
            uhe->timeout = UHE_TIMEOUT_FIRST;               //第一包命令超时时间
            if (again_enum_flag) {
                usb_init_2(true);                               //复位USB模块
                delay_5ms(start_delay);                         //重试时，延长Idle时间
            }
            usb_host_session_start();
            again_enum_flag++;
            continue;

        case ENUM_STEP_RESET_1:
        case ENUM_STEP_RESET_2:
        case ENUM_STEP_RESET_3:
            if (again_enum_flag >= 2) {
                delay_5ms(again_enum_delay);
            }
            usb_host_reset();
            continue;

        case ENUM_STEP_SET_ADDR:
            uhe->timeout = UHE_TIMEOUT;                     //后续命令超时时间
            memcpy(spb, std_set_addr, SETUP_LEN);
            if (uhe->devaddr >= 127) {
                uhe->devaddr = 0;
            }
            spb->val = ++uhe->devaddr;
            break;

        case ENUM_STEP_GET_DEVDESC_1:
            memcpy(spb, std_get_device_descriptor_1, SETUP_LEN);
            break;

        case ENUM_STEP_GET_DEVDESC_2:
            delay_5ms(10);
            memcpy(spb, std_get_device_descriptor_2, SETUP_LEN);
            again_enum_flag = 0;
            break;

        case ENUM_STEP_GET_CONFDESC_1:
            uhe->timeout = UHE_TIMEOUT;                     //后续命令超时时间
            memcpy(spb, std_get_configuration_descriptor, SETUP_LEN);
            break;

        case ENUM_STEP_GET_CONFDESC_2:
            //更新长度信息后，再次请求
            break;

        case ENUM_STEP_SET_CONF_2:
            memcpy(spb, std_set_configuration, SETUP_LEN);
            spb->val = uhe->cfg_val;                        //根据配置号设置配置描述符
            break;

        case ENUM_STEP_HID_SET_IDLE:
            memcpy(spb, hid_set_idle_cmd, SETUP_LEN);
            break;

        case ENUM_STEP_GET_REPORT_DESC:
            memcpy(spb, std_get_report_descriptor, SETUP_LEN);
            break;

        default:
            return true;
        }

        do_request(uhe);

        if ((uhe->error || uhe->stall) && retry_cnt) {
            printf("retry: %d, %d, %d\n", retry_cnt, uhe->error, uhe->stall);
            uhe->error = 0;
            retry_cnt--;
            start_delay += 20;                          //重试时，延长Idle时间，最大为500ms
             if (ENUM_STEP_DONE == enum_step) {
                return true;
             } else {
                enum_step = ENUM_STEP_START;
             }
        }
    }
    return false;
}


bool usb_host_enum_process(void)
{
    printf("usb_host_enum_process\n");

    if (!uhe_start(&uhe_0)) {
        return false;
    }

    uhe_hid.in_ep.buf = hid_in_buf;
    uhe_hid.out_ep.buf = hid_out_buf;

    usb_ep_init(&uhe_hid.in_ep);
    usb_ep_init(&uhe_hid.out_ep);

    usb_ep_reset(&uhe_hid.in_ep);
    usb_ep_reset(&uhe_hid.out_ep);

    usb_host_ep_type_set(&uhe_hid.in_ep);
    usb_host_ep_type_set(&uhe_hid.out_ep);

    usb_ep_callback_register(usb_host_hid_in_callback, uhe_hid.in_ep.index, uhe_hid.in_ep.dir);
    usb_ep_callback_register(usb_host_hid_out_callback, uhe_hid.out_ep.index, uhe_hid.out_ep.dir);

    usb_interrupt_init();

    uhe_0.valid = 1;

    return true;
}

void usb_host_enter_process(void)
{
    uhe_t *uhe = &uhe_0;

    uhe->valid = 0;
    uhe->error = 0;

    usb_host_enter(uhe);
}

void usb_host_exit_process(void)
{
    uhe_t *uhe = &uhe_0;

    uhe->valid = 0;
    uhe->suspend = 0;
    uhe->error = 1;

    usb_host_exit();
}

void usb_host_in_test(void)
{
    if (uhe_0.valid) {
        //usb_host_ep_in_transfer(&uhe_hid.in_ep);
        //uhe_hid.out_ep.buf[0] = 1;
        //usb_ep_start_transfer(&uhe_hid.out_ep, 1);
    }
}

