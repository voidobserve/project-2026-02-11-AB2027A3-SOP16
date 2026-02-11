#ifndef _BLE_BATTERY_SERVICE_H
#define _BLE_BATTERY_SERVICE_H


/*
PRIMARY_SERVICE, 180f
CHARACTERISTIC, 2a19, READ | NOTIFY | DYNAMIC,
CHARACTERISTIC, 2a1a, READ | NOTIFY | DYNAMIC,
*/

void ble_battery_service_init(void);
void ble_battery_service_proc(void);

#endif // _BLE_BATTERY_SERVICE_H