#ifndef __BLE_CLIENT_H
#define __BLE_CLIENT_H

void ble_client_init(void);
void ble_app_proc(void);
void ble_enter_sleep_proc(void);
void ble_exit_sleep_proc(void);

void ble_scan_en_test(void);
void ble_write_test(void);
void ble_read_test(void);
#endif
