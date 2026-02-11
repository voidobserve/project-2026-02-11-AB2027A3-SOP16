#ifndef _WIRELESS_H
#define _WIRELESS_H

#define wireless_set_work_mode              ble_set_work_mode
#define wireless_get_work_mode              ble_get_work_mode

void wireless_module_init(void);
void wireless_module_deinit(void);
void wireless_client_init(void);
void wireless_client_process(void);

void wireless_enter_sleep_hook(void);
void wireless_exit_sleep_hook(void);

#endif // _WIRELESS_H
