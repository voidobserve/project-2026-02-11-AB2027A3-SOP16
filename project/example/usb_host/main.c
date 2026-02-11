#include "include.h"
#include "bsp_uart_debug.h"
#include "usb_com.h"
#include "usb_host_enum.h"



#define EVT_USB_DEV_INSERT                       0x7f7
#define EVT_USB_DEV_REMOVE                       0x7f6


u16 tmr5ms_cnt;
u8 is_usb_mode;

void sys_ram_info_dump(void)
{
    extern u32 __comm_size;
    extern u32 __bss_size;
    extern u32 __bram_size;
    extern u32 __heap_size;

    extern u32 __bram_vma;
    extern u32 __bram_ram_size;
    extern u32 __data_vma;
    extern u32 __data_ram_size;
    extern u32 __comm_vma;
    extern u32 __comm_ram_size;

    u32 comm_free  = (u32)&__comm_ram_size - (u32)&__comm_size;
    u32 bss_free = (u32)&__data_ram_size - (u32)&__bss_size - (u32)&__heap_size;
    u32 bram_free = (u32)&__bram_ram_size - (u32)&__bram_size;
    u32 ram_total = comm_free + bss_free + bram_free;

    printf("Ram_info:\n");
    printf("bss:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", bss_free,  (u32)&__data_ram_size,  (u32)&__data_vma, (u32)&__data_vma + (u32)&__data_ram_size);
    printf("comm: remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", comm_free, (u32)&__comm_ram_size, (u32)&__comm_vma, (u32)&__comm_vma + (u32)&__comm_ram_size);
    printf("bram: remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", bram_free, (u32)&__bram_ram_size, (u32)&__bram_vma, (u32)&__bram_vma + (u32)&__bram_ram_size);
    printf("remain_total: %dK\n",ram_total / 1024);
}


AT(.com_text.detect)
void usb_detect(void)
{
    u8 usb_sta = usbchk_device_connect();

    if (usb_sta == USB_DEV_CONNECTED) {
        if (dev_online_filter(DEV_USB)) {
            msg_enqueue(EVT_USB_DEV_INSERT);
        }
    } else {
        if (dev_offline_filter(DEV_USB)) {
            is_usb_mode = 0;
            msg_enqueue(EVT_USB_DEV_REMOVE);
        }
    }
}

AT(.com_text.timer)
void usr_tmr5ms_thread_callback(void)
{
    tmr5ms_cnt++;

    usb_detect();
}

int main(void)
{
    WDT_DIS();

    sys_clk_set(SYS_120M);

    bsp_uart_debug_init();
    printf("-------------->usb example\n");

    sys_ram_info_dump();

    dev_init(40, 40);
    usb_init_2(true);

    usb_host_init();

	sys_set_tmr_enable(1, 0);

    while (1) {
        switch (msg_dequeue()) {
            case EVT_USB_DEV_INSERT:
                if (dev_is_online(DEV_USB)) {
                    printf("usb insert\n");
                    is_usb_mode = 1;
                    usb_host_enter_process();
                }
                break;

            case EVT_USB_DEV_REMOVE:
                printf("usb remove\n");
                usb_host_exit_process();
                break;

            default:
                break;
        }

        if (is_usb_mode) {
            usb_host_process();
        }
    }

    return 0;
}
