#include "include.h"
#include "driver_lowpwr.h"
#include "driver_gpio.h"

static const char help_info[] = \
"""\
\n******************************************************************************\
\nIn the USB debugging phase, you are advised to use PB9 (VUSB) as the debugging \
\ninformation output port, because the commonly used PB3 is occupied by the USB \
\nmodule. When debugging, the VUSB serves as the logical communication port, and \
\nthe bypass filter capacitor cannot be added to its external port.\
\n\
\nIf you want to restore VUSB as the power supply port and VBAT is not connected \
\nto battery, you need to perform the following operations:\
\n        pmu_init(pmu_cfg | PMU_CFG_VUSB_TO_VDDIO);\
\n\
\n[Tips] *When you change the usb interface descriptor, you may need to update the \
\ncached device driver in the PC, or update the PID/VID of the device, so that the \
\nPC automatically matches the driver again.\
\n******************************************************************************\
\n\
\n\
""";

int main(void)
{
    sys_cb.rst_reason = sys_rst_init(WK0_10S_RESET);
    printf("Hello AB202X: %08x\n", sys_cb.rst_reason);
    sys_rst_dump(sys_cb.rst_reason);

    sys_cb.wakeup_reason  = lowpwr_get_wakeup_source();

    printf("SW_VERSION: %s\n",SW_VERSION);

    sys_ram_info_dump();

    printf(help_info);

    bsp_sys_init();

    prod_test_init();

    func_run();

    return 0;
}
