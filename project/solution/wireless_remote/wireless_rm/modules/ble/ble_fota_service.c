#include "include.h"
#include "fota_proc.h"

#if AB_FOT_CHL_BLE_EN

#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          my_printf("[BLE] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


static att_service_handler_t    fota_service;
static uint16_t                 fota_client_config;


static void fota_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            uint8_t status = param[10];
            if (status) {
                printf("%s, connection failed\n", __func__);
                return;
            }

            memcpy(&ble_cb.con_handle, &param[7], 2);
            fota_connected_handle(FOTA_CHL_BLE);
        } break;

        case BLE_EVT_DISCONNECT:{
            ble_cb.con_handle = 0;
            fota_client_config = CCCD_DFT;
            fota_disconnected_handle(FOTA_CHL_BLE);
        } break;

        default:
            break;
    }
}

static uint16_t fota_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if(attribute_handle == ATT_CHARACTERISTIC_FF15_01_VALUE_HANDLE){
        u8 read_data[] = "FOTA";
        if(buffer){
            memcpy(buffer, read_data, sizeof(read_data));
        }
        return sizeof(read_data);
    }

	return 0;
}

static int fota_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    if(attribute_handle == ATT_CHARACTERISTIC_FF14_01_CLIENT_CONFIGURATION_HANDLE){
        fota_client_config = GET_LE16(&buffer[0]);
    }else if(attribute_handle == ATT_CHARACTERISTIC_FF15_01_VALUE_HANDLE){
        fota_recv_packet_handle(buffer, buffer_size, FOTA_CHL_BLE);
    }

	return ATT_ERR_NO_ERR;
}

AT(.text.fot.cache)
static int fota_service_notify(uint8_t *buf, uint16_t size)
{
    int res = 0;

    if (fota_client_config) {
        TRACE("fot tx:");
        TRACE_R(buf, size);
        res = ble_notify_for_handle(ble_cb.con_handle, ATT_CHARACTERISTIC_FF14_01_VALUE_HANDLE, buf, size);
    }

    return res;
}

static int fota_service_mtu_get(uint32_t *mtu)
{
    int res;

    res = att_server_get_mtu(ble_cb.con_handle, (u16 *)mtu);
    *mtu -= 3;

    return res;
}

void ble_fota_service_init(void)
{
    TRACE("ble_fota_service_init\n");

    // get service handle range
	uint16_t start_handle = ATT_SERVICE_FF12_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_FF12_END_HANDLE;
    ab_fota_cb_t param;

	fota_client_config = CCCD_DFT;

    // register service with ATT Server
	fota_service.start_handle   = start_handle;
	fota_service.end_handle     = end_handle;
	fota_service.read_callback  = &fota_read_callback;
	fota_service.write_callback = &fota_write_callback;
	fota_service.event_handler  = &fota_event_packet_handle;
	att_server_register_service_handler(&fota_service);

    param.fota_send_packet_handle = fota_service_notify;
    param.fota_mtu_check_handle = fota_service_mtu_get;
    fota_init(&param, FOTA_CHL_BLE);
}

#endif // AB_FOT_CHL_BLE_EN
