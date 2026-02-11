#ifndef _THREAD_GUI_H
#define _THREAD_GUI_H

// Thread Driver Message
enum {
    THREAD_DRIVER_MSG_GUI_DRAW=0x80,

};

void thread_driver_gui_event_callback(uint8_t msg);

#endif
