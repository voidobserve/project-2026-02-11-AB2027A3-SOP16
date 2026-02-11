#include "include.h"
#include "ble_user_service.h"


static att_service_handler_t    generic_access_service;
static uint16_t                 generic_access_client_config;

static att_service_handler_t    generic_attribute_service;

static att_service_handler_t    device_information_service;

static uint16_t connect_handle;

static void user_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            uint8_t status = param[10];
            if (status) {
                printf("%s, connection failed\n", __func__);
                return;
            }

            memcpy(&connect_handle, &param[7], 2);
            //printf("-->BLE_EVENT_CONNECTED:%x\n",connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            //uint16_t con_handle;
            //memcpy(&con_handle, &param[0], 2);
            //printf("-->BLE_EVENT_DISCONNECTED:%x\n",con_handle);
            connect_handle = 0;
            generic_access_client_config = CCCD_DFT;
        } break;
        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            u8 status = param[0];
            u16 handle = param[1] | (param[2] << 8);
            u16 interval = param[3] | (param[4] << 8);
            u16 latency = param[5] | (param[6] << 8);
            u16 timeout = param[7] | (param[8] << 8);
            printf("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
        } break;

        default:
            break;
    }
}

static uint16_t user_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("user_service_read_callback\n");

    if (attribute_handle == ATT_CHARACTERISTIC_2a00_01_VALUE_HANDLE) {
        u8 read_data[] = BLE_DEVICE_NAME;
        u8 read_data_len = strlen(xcfg_cb.le_name);
        if (read_data_len > 0) {
            if (buffer) {
                memcpy(buffer, xcfg_cb.le_name, read_data_len);
            }
        } else {
            read_data_len = sizeof(read_data) - 1;
            if (buffer){
                memcpy(buffer, read_data, read_data_len);
            }
        }
        return read_data_len;
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a29_01_VALUE_HANDLE) {
        u8 read_data[] = BLE_MANUFACTURER;
        if (buffer){
            memcpy(buffer, read_data, sizeof(read_data) - 1);
        }
        return sizeof(read_data) - 1;
    }

	return 0;
}

static int user_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("user_service_write_callback\n");

    if (attribute_handle == ATT_CHARACTERISTIC_2a05_01_CLIENT_CONFIGURATION_HANDLE){
        generic_access_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a05_01_CLIENT_CONFIGURATION_HANDLE){
        generic_access_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a05_01_CLIENT_CONFIGURATION_HANDLE){
        generic_access_client_config = GET_LE16(&buffer[0]);
    }

	return ATT_ERR_NO_ERR;
}

void ble_user_service_init(void)
{
    printf("ble_user_service_init\n");

    // register service with ATT Server
	generic_access_service.start_handle   = ATT_SERVICE_1800_START_HANDLE;
	generic_access_service.end_handle     = ATT_SERVICE_1800_END_HANDLE;
	generic_access_service.read_callback  = &user_service_read_callback;
	generic_access_service.write_callback = &user_service_write_callback;
	generic_access_service.event_handler  = &user_service_event_callback;
	att_server_register_service_handler(&generic_access_service);

    // register service with ATT Server
	generic_attribute_service.start_handle   = ATT_SERVICE_1801_START_HANDLE;
	generic_attribute_service.end_handle     = ATT_SERVICE_1801_END_HANDLE;
	generic_attribute_service.read_callback  = &user_service_read_callback;
	generic_attribute_service.write_callback = &user_service_write_callback;
	generic_attribute_service.event_handler  = &user_service_event_callback;
	att_server_register_service_handler(&generic_attribute_service);

    // register service with ATT Server
	device_information_service.start_handle   = ATT_SERVICE_180a_START_HANDLE;
	device_information_service.end_handle     = ATT_SERVICE_180a_END_HANDLE;
	device_information_service.read_callback  = &user_service_read_callback;
	device_information_service.write_callback = &user_service_write_callback;
	device_information_service.event_handler  = &user_service_event_callback;
	att_server_register_service_handler(&device_information_service);
}

void ble_user_service_proc(void)
{
}
