#include "include.h"
#include "ble_service_atvv.h"

#if (LE_REOMTE_PRODUCT_ID == LE_REMOTE_ATVV)

#define DEBUG_EN 1

#if DEBUG_EN
#define DEBUG(...)                  printf(__VA_ARGS__)
#define DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUG_R(...)
#endif

static att_service_handler_t    ble_generic_att_service;

static void ble_generic_att_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
   switch (event_type) {
        case BLE_EVT_CONNECT:{
            uint8_t status = param[10];
            if (status) {
                printf("%s, connection failed\n", __func__);
                return;
            }

            memcpy(&ble_cb.con_handle, &param[7], 2);
            DEBUG("-->BLE_GENERIC_ATT_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cb.con_handle, &param[0], 2);
            DEBUG("-->BLE_GENERIC_ATT_EVENT_DISCONNECTED:%x\n",ble_cb.con_handle);
            ble_cb.con_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_generic_att_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    DEBUG("ble generic att read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A19_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }
    // DEBUG("len:%d\n", read_len);
    // DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_generic_att_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    uint16_t write_len = 0;
    DEBUG("ble generic att write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        default:
            break;
    }

	return write_len;
}

void ble_generic_att_service_init(void)
{
    DEBUG("ble_generic_att_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_1801_01_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_1801_01_END_HANDLE;

    // register service with ATT Server
	ble_generic_att_service.start_handle   = start_handle;
	ble_generic_att_service.end_handle     = end_handle;
	ble_generic_att_service.read_callback  = &ble_generic_att_service_read_callback;
	ble_generic_att_service.write_callback = &ble_generic_att_service_write_callback;
	ble_generic_att_service.event_handler  = &ble_generic_att_service_event_packet_handle;
	att_server_register_service_handler(&ble_generic_att_service);

    ble_cb.con_handle = 0;
}

void ble_generic_att_service_proc(void)
{
    if (ble_cb.con_handle == 0) {
        return;
    }
}

#endif  //LE_REOMTE_PRODUCT_ID