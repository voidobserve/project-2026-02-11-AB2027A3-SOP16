#ifndef _WIRELESS_H
#define _WIRELESS_H


#define wireless_set_work_mode              ble_set_work_mode
#define wireless_get_work_mode              ble_get_work_mode


typedef struct PACKED {
    uint8_t         address_type;
    uint8_t         addr[6];
} wireless_addr_info_typedef;

void wireless_module_init(void);
void wireless_module_deinit(void);
void wireless_service_init(void);
void wireless_service_process(void);

void wireless_enter_sleep_hook(void);
void wireless_exit_sleep_hook(void);

#endif // _WIRELESS_H
