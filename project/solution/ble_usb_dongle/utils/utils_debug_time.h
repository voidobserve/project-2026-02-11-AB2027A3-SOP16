#ifndef _UTILS_DEBUG_TIME_H
#define _UTILS_DEBUG_TIME_H


#define COM_TEXT_UTILS_DEBUG            .com_text.utils.debug


void utils_debug_time_init(void);
uint32_t utils_debug_time_get(void);
void utils_debug_time_alarm(uint32_t delay, void (* callback)(void));

#endif // _UTILS_DEBUG_TIME_H
