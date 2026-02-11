#ifndef _WIRELESS_H
#define _WIRELESS_H
#include "utils_ring_buffer.h"

#define wireless_set_work_mode              ble_set_work_mode
#define wireless_get_work_mode              ble_get_work_mode

/*  Wireless Packet Format
 *  __________________________________________________________
 *      TYPE(1B)    |    LENGTH(1B)    |    VALUE(0 or nB)
 *  __________________________________________________________
 */

#define WIRELESS_PACKET_TYPE_HID                     0x01
#define WIRELESS_PACKET_TYPE_AUDIO                   0x02
#define WIRELESS_PACKET_TYPE_CTRL                    0x04

void wireless_module_init(void);
void wireless_module_deinit(void);
void wireless_client_init(void);
void wireless_client_process(void);

void wireless_enter_sleep_hook(void);
void wireless_exit_sleep_hook(void);

#endif // _WIRELESS_H
