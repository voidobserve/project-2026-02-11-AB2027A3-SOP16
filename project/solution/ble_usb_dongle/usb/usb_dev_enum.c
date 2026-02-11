#include "include.h"
#include "usb_com.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "usb_vendor.h"
#include "usb_audio.h"
#include "usb_cdc.h"

ude_cb_t ude_cb;

u8 cfg_desc_buf[0x100] AT(.usb_buf.device.desc.buf);

void ude_state_reset(ude_cb_t *ude)
{
    memset(ude, 0, sizeof(ude_cb_t));
}


//Get Status
static bool do_get_status(uint status)
{
    u8 buf[2];

    buf[0] = status;
    buf[1] = 0;

    return usb_ep0_start_transfer(buf, 2);
}

//Get Device Status
static bool do_get_device_status(void)
{
    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

    u8 status = 0;

    if (USB_DEV_STA & DS_SELF_PWR) {
        status |= BIT(0);
    }

    if (USB_DEV_STA & DS_REM_WAKEUP) {
        if (ude_cb.rem_wakeup) {
            status |= BIT(1);
        }
    }

    return do_get_status(status);
}

//Get Interface Status
static bool do_get_interface_status(void)
{
    if (0 == ude_cb.cfgval) {                         //判断是否已配置
        return false;
    }
    return do_get_status(0);
}

//Get EndPoint Status
static bool do_get_ep_status(spb_wrap_t *spb)
{
    epcb_t *ep;
    uint req_epnum = BYTE0(spb->index);
    uint status = 0;

    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

    if (0 == ude_cb.cfgval && 0 != req_epnum) {
        return false;
    }
    //查询Endpoint的Stall状态
    uint temp = BYTE0(spb->index);
    uint8_t index = temp & 0x7f;
    uint8_t dir = temp & BIT(7) ? 1:0;
    ep = usb_ep_get_for_index(index, dir);

    status = ep->halt;
    return do_get_status(status);
}

//Clear Feature
static bool do_clear_feature_for_endpoint(spb_wrap_t *spb)
{
    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

    uint req_epnum = BYTE0(spb->index);

    if (0 == ude_cb.cfgval && 0 != req_epnum) {
        return false;
    }

    if (FT_EP_STALL != spb->val) {
        return false;
    }

    uint temp = BYTE0(spb->index);
    uint8_t index = temp & 0x7f;
    uint8_t dir = temp & BIT(7) ? 1:0;
    epcb_t *ep = usb_ep_get_for_index(index, dir);
    usb_ep_clear(ep);

    return true;
}

//Set Feature
static bool do_set_feature_for_endpoint(spb_wrap_t *spb)
{
    printf("-->do_set_feature_for_endpoint\n");

    uint req_epnum = BYTE0(spb->index);
    if (0 == ude_cb.cfgval && 0 != req_epnum) {
        return false;
    }
    if (FT_EP_STALL != spb->val) {
        return false;
    }
    uint temp = BYTE0(spb->index);
    uint8_t index = temp & 0x7f;
    uint8_t dir = temp & BIT(7) ? 1:0;
    epcb_t *ep = usb_ep_get_for_index(index, dir);

    usb_ep_halt(ep);

    return true;
}

static bool do_set_feature_for_device(spb_wrap_t *spb)
{
    printf("-->do_set_feature_for_device\n");

    uint req_epnum = BYTE0(spb->index);
    if (0 == ude_cb.cfgval && 0 != req_epnum) {
        return false;
    }
    if (FT_DEV_REM_WAKEUP != spb->val) {
        return false;
    }

    usb_device_suspend_enable();

    ude_cb.rem_wakeup = true;

    return true;
}

static bool do_clear_feature_for_device(spb_wrap_t *spb)
{
    printf("-->do_clear_feature_for_device\n");

    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

    uint req_epnum = BYTE0(spb->index);

    if (0 == ude_cb.cfgval && 0 != req_epnum) {
        return false;
    }

    if (FT_DEV_REM_WAKEUP != spb->val) {
        return false;
    }

    ude_cb.rem_wakeup = false;

    return true;
}

//Get Descriptor通用
static bool do_transfer_descriptor(spb_wrap_t *spb, const void *buf, uint len)
{
    uint inlen = spb->len;
    if (inlen > len) {
        inlen = len;
    }
    return usb_ep0_start_transfer((uint8_t*)buf, inlen);
}

//Get String Descriptor
static bool do_get_string_descriptor(spb_wrap_t *spb)
{
    uint index = BYTE0(spb->val);
    u8 *buf = NULL;
    u8 length = 0;

    switch (index) {
        case STR_LANGUAGE_ID:
            buf = usb_get_lang_id_str_descriptor(&length);
            return do_transfer_descriptor(spb, buf, length);

        case STR_MANUFACTURER:
            buf = usb_get_manufacturer_str_descriptor(&length);
            return do_transfer_descriptor(spb, buf, length);

        case STR_PRODUCT:
            buf = usb_get_product_str_descriptor(&length);
            return do_transfer_descriptor(spb, buf, length);

        case STR_SERIAL_NUM:
            buf = usb_get_serial_str_descriptor(&length);
            return do_transfer_descriptor(spb, buf, length);

        default:
            return false;
    }
}

//获取配置描述符，并自动裁剪
AT(.usbdev.com)
void get_config_descriptor(void)
{
    u16 cfg_desc_len;
    u8 *desc_ptr = &cfg_desc_buf[0];
    u8 *buf = NULL;
    u8 length = 0;

    //copy general config descriptor
    buf = usb_get_cfg_descriptor(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    } else {
        // unexpected case
        printf("[Error] you must be rewrite 'usb_get_cfg_descriptor' and return valid cfg desc.\n");
        return;
    }

#if USB_AUDIO_EN
    buf = usb_audio_itf_header_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }

#if USB_SPEAKER_EN
    buf = usb_audio_itf_speaker_stream_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }
#endif // USB_SPEAKER_EN

#if USB_MIC_EN
    buf = usb_audio_itf_mic_stream_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }
#endif // USB_MIC_EN
#endif

#if USB_HID_EN
    buf = usb_hid_itf_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }
#endif // USB_HID_EN

#if USB_VENDOR_EN
    buf = usb_vendor_itf_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }
#endif

#if USB_CDC_EN
    buf = usb_cdc_itf_desc_get(&length);
    if (length) {
        memcpy(desc_ptr, buf, length);
        desc_ptr += length;
    }
#endif

    cfg_desc_len = (u32)desc_ptr - (u32)cfg_desc_buf;
    cfg_desc_buf[2] = (u8)cfg_desc_len;                     //update total size of config descriptor
    cfg_desc_buf[3] = (u8)(cfg_desc_len >> 8);
    printf("desc: %d\n", cfg_desc_len);

    if (cfg_desc_len > sizeof(cfg_desc_buf)) {
        printf("--->err: cfg_desc_buf overflow!\n");
        while(1);
    }
}

//Get Descriptor
AT(.usbdev.com)
static bool do_get_descriptor(spb_wrap_t *spb)
{
    uint desc_type = BYTE1(spb->val);
    u8 *buf = NULL;
    u8 length = 0;

    switch (desc_type) {
        case DEVICE_DESCRIPTOR:
            //1: 设备描述符
            buf = usb_get_device_descriptor(&length);
            return do_transfer_descriptor(spb, buf, length);

        case CONFIGURATION_DESCRIPTOR:
            //2: 配置描述符
            get_config_descriptor();
            return do_transfer_descriptor(spb, cfg_desc_buf, ((u16)cfg_desc_buf[3] << 8) | cfg_desc_buf[2]);

        case STRING_DESCRIPTOR:
            //3: 字符串描述符
            return do_get_string_descriptor(spb);

        case INTERFACE_DESCRIPTOR: {
            printf("INTERFACE_DESCRIPTOR\n");
            //4: 接口描述符
            u8 req_index = BYTE0(spb->index); // 获取请求的接口索引
            u8 *ptr = cfg_desc_buf;
            u16 total_len = (u16)cfg_desc_buf[3] << 8 | cfg_desc_buf[2]; // 配置描述符总长度

            // 跳过配置描述符头（9字节）
            ptr += cfg_desc_buf[0]; // cfg_desc_buf[0] 为配置描述符长度

            // 遍历配置描述符内容
            while ((ptr - cfg_desc_buf) < total_len) {
                u8 desc_len = ptr[0];
                u8 desc_type = ptr[1];
                u8 interface_index = ptr[2];

                if (desc_type == INTERFACE_DESCRIPTOR) {
                    if (interface_index == req_index) {
                        // 找到目标接口描述符，返回数据
                        return do_transfer_descriptor(spb, ptr, desc_len);
                    }
                }
                ptr += desc_len; // 移动到下一个描述符
            }
        } break;

        case ENDPOINT_DESCRIPTOR:
            printf("ENDPOINT_DESCRIPTOR\n");
            //5: 端点描述符
            break;

        case HID_DESCRIPTOR:
            printf("usb_hid_desc_get\n");
#if USB_HID_EN
            if (spb->index == USB_HID_ITF_INDEX) {
                buf = usb_hid_desc_get(&length);
                return do_transfer_descriptor(spb, buf, length);
            }
#endif

#if USB_VENDOR_EN
            if (spb->index == USB_VENDOR_ITF_INDEX) {
                buf = usb_vendor_hid_desc_get(&length);
                return do_transfer_descriptor(spb, buf, length);
            }
#endif
            break;

        case HID_REPORT_DESCRIPTOR:
            printf("usb_hid_report_desc_get:%x\n",spb->index);
#if USB_HID_EN
            if (spb->index == USB_HID_ITF_INDEX) {
                buf = usb_hid_report_desc_get(&length);
                return do_transfer_descriptor(spb, buf, length);
            }
#endif

#if USB_VENDOR_EN
            if (spb->index == USB_VENDOR_ITF_INDEX) {
                buf = usb_vendor_report_desc_get(&length);
                return do_transfer_descriptor(spb, buf, length);
            }
#endif
            break;

        default:
            break;
    }
    return false;
}

//Get Configuration
static bool do_get_configuration(void)
{
    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }
    return usb_ep0_start_transfer(&ude_cb.cfgval, 1);
}

//Set Configuration
static bool do_set_configuration(spb_wrap_t *spb)
{
    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

    if (spb->val > 1) {
        return false;
    }

    ude_cb.cfgval = spb->val;

#if USB_HID_EN
    ude_hid_setvalid(true);
#endif

#if USB_VENDOR_EN
    usb_vendor_setvalid(true);
#endif

    return true;
}

//Get Interface
static bool do_get_interface(spb_wrap_t *spb)
{
    u8 buf[1] = {0};

    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

#if USB_MIC_EN
    if (USB_AUDIO_STREAM_MIC_ITF_INDEX == (u8)spb->index) {
        buf[0] = uda_get_isocin_flag();
    }
#endif

#if USB_SPEAKER_EN
    if (USB_AUDIO_STREAM_SPEAKER_ITF_INDEX == (u8)spb->index) {
        buf[0] = uda_get_isocout_flag();
    }
#endif

    return usb_ep0_start_transfer(buf, 1);
}

//Set Interface
static bool do_set_interface(spb_wrap_t *spb)
{
    if (0 == ude_cb.devaddr) {                       //判断地址是否设置
        return false;
    }

#if USB_MIC_EN
    if (USB_AUDIO_STREAM_MIC_ITF_INDEX == spb->index) {
        //每次开启和关闭MIC之前，都会set interface
        if (spb->val) {
            //start mic
            printf("start mic\n");
            uda_set_isocin_flag(1);
        } else {
            //stop mic
            printf("stop mic\n");
            uda_set_isocin_flag(0);
        }
    }
#endif

#if USB_SPEAKER_EN
    if (USB_AUDIO_STREAM_SPEAKER_ITF_INDEX == spb->index) {
        //每次开启和关闭speaker之前，都会set interface
        if (spb->val) {
            printf("start speaker\n");
            uda_set_isocout_flag(1);
        } else {
            printf("stop speaker\n");
            uda_set_isocout_flag(0);
        }
    }
#endif

    return true;
}

static bool usb_set_address(spb_wrap_t *spb)
{
    ude_cb.devaddr = BYTE0(spb->val);

    return true;
}

//处理标准命令，主机数据方向为in
AT(.usbdev.com)
static bool do_standard_in(spb_wrap_t *spb)
{
    switch (spb->recipient) {
        case ST_REQ_DEVICE:
            switch (spb->req) {
                case UR_GET_STATUS:
                    //00: GetStatus
                    return do_get_device_status();

                case UR_GET_DESCRIPTOR:
                    //06: GetDescriptor
                    return do_get_descriptor(spb);

                case UR_GET_CONFIGURATION:
                    //08: GetConfiguration
                    return do_get_configuration();

                default:
                    break;
            }
            break;

        case ST_REQ_INTERFACE:
            switch (spb->req) {
                case UR_GET_STATUS:
                    if (spb->req == UR_GET_STATUS) {
                        return do_get_interface_status();
                    }
                    break;

                case UR_GET_INTERFACE:
                    //0a: GetInterface
                    return do_get_interface(spb);

                case UR_GET_DESCRIPTOR:
                    //06: GetDescriptor
                    return do_get_descriptor(spb);

                default:
                    break;
            }
            break;

        case ST_REQ_ENDPOINT:
            switch (spb->req) {
                case UR_GET_STATUS:
                    if (spb->req == UR_GET_STATUS) {
                        return do_get_ep_status(spb);
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return false;
}

//处理类命令，主机数据为向为in
AT(.usbdev.com)
static bool do_class_in(spb_wrap_t *spb)
{
    switch (spb->recipient) {
        case ST_REQ_DEVICE:
            break;

        case ST_REQ_INTERFACE:
            switch (spb->req) {
#if USB_AUDIO_EN
                case UA_GET_CUR:
                case UA_GET_MIN:
                case UA_GET_MAX:
                case UA_GET_RES:

                if (BYTE1(spb->index) == UA_SPEAKER_UNIT_ID) {
#if USB_SPEAKER_EN
                    //Speaker Get Feature Unit Control Request
                    if (BYTE1(spb->val) == UA_MUTE_CTL) {
                        //Mute Control
                        printf("do_get_spk_mute\n");
                        return uda_get_spk_mute();
                    } else if (BYTE1(spb->val) == UA_VOL_CTL){
                        //Volume Control
                        printf("do_get_spk_volume\n");
                        return uda_get_spk_volume(spb->req);
                    }
#endif
                } else if(BYTE1(spb->index) == UA_MIC_UNIT_ID){
#if USB_MIC_EN
                    //MIC Get Feature Unit Control Request
                    if (BYTE1(spb->val) == UA_MUTE_CTL) {
                        //Mute Control
                        printf("do_get_mic_mute\n");
                        return uda_get_mic_mute();
                    } else if (BYTE1(spb->val) == UA_VOL_CTL) {
                        //Volume Control
                        printf("do_get_mic_volume\n");
                        return uda_get_mic_volume(spb->req);
                    }
#endif
                }
                break;
            #endif

#if USB_CDC_EN
                case CDC_GET_LINE_CODING:
                    return usb_cdc_get_line_coding();
#endif

#if (USB_HID_EN || USB_VENDOR_EN)
                case HR_GET_IDLE:{
                    uint8_t interface = BYTE0(spb->index);
                    uint8_t report_id = BYTE0(spb->val);
                    uint8_t idle_time = 0;
#if USB_HID_EN
                    if(interface == USB_HID_ITF_INDEX) {
                        idle_time = usb_hid_get_idle(report_id);
                    }
#endif

#if USB_VENDOR_EN
                    if(interface == USB_VENDOR_ITF_INDEX) {
                        idle_time = usb_vendor_get_idle(report_id);
                    }
#endif
                    return usb_ep0_start_transfer(&idle_time, 1);
                }
#endif

                default:
                    break;
            }
            break;

        case ST_REQ_ENDPOINT:
            break;

        default:
            break;
    }
    return false;
}

//处理标准命令，主机数据方向为out
AT(.usbdev.com)
static bool do_standard_out(spb_wrap_t *spb)
{

    switch (spb->recipient) {
        case ST_REQ_DEVICE:
            switch (spb->req) {
                case UR_SET_ADDRESS:
                    //05: SetAddress
                    return usb_set_address(spb);

                case UR_SET_CONFIGURATION:
                    //09: SetConfiguration
                    return do_set_configuration(spb);

                case UR_CLEAR_FEATURE:
                    return do_clear_feature_for_device(spb);

                case UR_SET_FEATURE:
                    return do_set_feature_for_device(spb);

                default:
                    break;
            }
            break;

        case ST_REQ_INTERFACE:
            switch (spb->req) {
                case UR_SET_INTERFACE:
                    //0b: SetInterface
                    return do_set_interface(spb);

                default:
                    break;
            }
            break;


        case ST_REQ_ENDPOINT:
            switch (spb->req) {
                case UR_CLEAR_FEATURE:
                    return do_clear_feature_for_endpoint(spb);

                case UR_SET_FEATURE:
                    return do_set_feature_for_endpoint(spb);

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return false;
}

static bool do_class_out(spb_wrap_t *spb, uint8_t *data, uint16_t data_len)
{
    switch (spb->recipient) {
        case ST_REQ_DEVICE:
            break;

        case ST_REQ_INTERFACE:
#if USB_CDC_EN
            if (BYTE0(spb->index) == USB_CDC_INTR_ITF_INDEX) {
                if (spb->req == CDC_SET_LINE_CODING) {
                    printf("-->usb_cdc: set line coding\n");
                    print_r(data, data_len);
                    return true;
                } else if (spb->req == CDC_SET_LINE_STATE) {
                    printf("-->usb_cdc: set control line state:%d\n",spb->val);
                    return true;
                }
            }
#endif

#if USB_AUDIO_EN
            if ((spb->req == UA_SET_CUR) && (BYTE0(spb->index) == USB_AUDIO_CTL_ITF_INDEX)) {
#if USB_SPEAKER_EN
                if (BYTE1(spb->index) == UA_SPEAKER_UNIT_ID) {
                    if (BYTE1(spb->val) == UA_MUTE_CTL) {
                        //Speaker SET MUTE
                        uda_set_spk_mute(data[0]);
                    } else if (BYTE1(spb->val) == UA_VOL_CTL) {
                        //Speaker SET VOLUME
                        uda_set_spk_volume(GET_LE16(&data[0]));
                    }
                }
#endif

#if USB_MIC_EN
                if (BYTE1(spb->index) == UA_MIC_UNIT_ID) {
                    if (BYTE1(spb->val) == UA_MUTE_CTL) {
                        //MIC SET MUTE
                        uda_set_mic_mute(data[0]);
                    } else if (BYTE1(spb->val) == UA_VOL_CTL) {
                        //MIC SET VOLUME
                        uda_set_mic_volume(GET_LE16(&data[0]));
                    }
                }
#endif
            }
#endif

#if USB_HID_EN
            if ((spb->req == HR_SET_IDLE) && (BYTE0(spb->index) == USB_HID_ITF_INDEX)) {
                printf("-->hid_set_idle\n");
                usb_hid_set_idle(BYTE0(spb->val), BYTE1(spb->val));
            }
#endif

#if USB_VENDOR_EN
            if ((spb->req == HR_SET_IDLE) && (BYTE0(spb->index) == USB_VENDOR_ITF_INDEX)) {
                printf("-->vendor_hid_set_idle\n");
                usb_vendor_set_idle(BYTE0(spb->val), BYTE1(spb->val));
            }
#endif
            break;


        case ST_REQ_ENDPOINT:
            break;

        default:
            break;
    }

    return true;
}

AT(.usbdev.com)
bool usb_ep0_ctl_callback(spb_wrap_t *spb, uint8_t *data, uint16_t data_len)
{
    //printf("ep0_rx: \n");
    //print_r(spb, sizeof(spb_wrap_t));

    if (spb->dir == EP_DIR_IN) {
        if (spb->type == ST_TYPE_STANDARD) {
            return do_standard_in(spb);             //Standard in
        } else if (spb->type == ST_TYPE_CLASS) {
            return do_class_in(spb);                //Class in
        } else {
            return false;
        }
    } else {
        bool res;
        if (spb->type == ST_TYPE_STANDARD) {
            res = do_standard_out(spb);             //Standard out
        } else if (spb->type == ST_TYPE_CLASS) {
            res = do_class_out(spb, data, data_len);                //Class out
        } else {
            return false;
        }
        return res;
    }

    return false;
}

