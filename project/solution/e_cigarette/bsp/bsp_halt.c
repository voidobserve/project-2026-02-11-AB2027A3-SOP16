#include "include.h"

#if GUI_SELECT

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
AT(.com_rodata.halt)
const char str_halt[] = "<<< Halt >>>: %X\n";
#else
#define TRACE(...)
#endif

AT(.com_text.halt)
void halt(u32 halt_no)
{
    int reset_cnt = 30000000;
    TRACE(str_halt, halt_no);
    PICCON = 0;
    gui_halt(halt_no);
    while (1) {
        WDT_CLR();
        if (reset_cnt-- <= 0) {
            WDT_RST_DELAY();
        }
    }
}

#else
AT(.com_text.halt)
void halt(u32 halt_no){}
#endif  // GUI_SELECT
