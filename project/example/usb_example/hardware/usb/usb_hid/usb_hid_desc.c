#include "include.h"
#include "usb_desc.h"
#include "usb_hid_desc.h"

#if USB_HID_EN

//Config descriptor: HID
AT(.usbdev.com.table)
const u8 desc_config_hid[] = {
    //Interface Descriptor:

 #if USB_ASSN_EN
    //IAD Descriptor:
    0x08,
    0x0B,
    USB_HID_ITF_INDEX,
    0x01,
    0x03,
    0x00,
    0x00,
    0x00,
#endif
    9,                          // Num bytes of this descriptor
    INTERFACE_DESCRIPTOR,       // Interface descriptor type
    USB_HID_ITF_INDEX,          // Interface Number
    0,                          // Alternate interface number
    2,                          // Num endpoints of this interface
    0x03,                       // Interface Sub Class: Human Interface Device
    0,                          // Interface Sub Class:
    0,                          // Class specific protocol:
    0,                          // Index of Interface string descriptor

    //HID Descriptor:
    9,                          // Num bytes of this descriptor
    HID_DESCRIPTOR,             // HID Type
    0x11, 0x01,                 // HID Class Specification compliance
    0x00,                       // Country Code: none
    0x01,                       // Number of descriptors to follow
    HID_REPORT_DESCRIPTOR,      // Report descriptor type
    BYTE0(HID_REPORT_DESC_SIZE),// Len of Report descriptor ,sizeof(desc_hid_report)
    BYTE1(HID_REPORT_DESC_SIZE),

    //EndpointDescriptor:
    7,                          // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,        // Endpoint descriptor type
    USB_HID_EP_OUT_INDEX,       // Endpoint number: HID OUT
    EP_TYPE_INTR,               // Interrupt Endpoint
    BYTE0(HID_EP_OUT_MAX_SIZE), // Maximum packet size
    BYTE1(HID_EP_OUT_MAX_SIZE),
    0x01,                       // Poll every 1 msec seconds

    //EndpointDescriptor:
    7,                          // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,        // Endpoint descriptor type
    USB_HID_EP_IN_INDEX | 0x80, // Endpoint number: HID IN
    EP_TYPE_INTR,               // Interrupt Endpoint
    BYTE0(HID_EP_IN_MAX_SIZE),  // Maximum packet size
    BYTE1(HID_EP_IN_MAX_SIZE),
    0x01,                       // Poll every 1 msec seconds
};

//HID report
AT(.usbdev.com.table)
const u8 desc_hid_report[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x05,        //   Report Count (5)
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x05,        //   Usage Maximum (Kana)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3)
    0x91, 0x03,        //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (0x65)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x29, 0x65,        //   Usage Maximum (0x65)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

uint8_t *usb_hid_itf_desc_get(uint8_t *length)
{
    if (HID_REPORT_DESC_SIZE != sizeof(desc_hid_report)){
        printf("--->desc_hid_report_size_err: %d,%d\n", HID_REPORT_DESC_SIZE, sizeof(desc_hid_report));
        while(1);
    }

    *length = sizeof(desc_config_hid);
    return (u8 *)desc_config_hid;
}

uint8_t *usb_hid_desc_get(uint8_t *length)
{
   u8* desc = (u8*)desc_config_hid;

   while (desc[1] != HID_DESCRIPTOR) {
        desc += desc[0];
   }

    *length = desc[0];
    return (u8 *)desc;
}

uint8_t *usb_hid_report_desc_get(uint8_t *length)
{
    *length = sizeof(desc_hid_report);
    return (u8 *)desc_hid_report;
}

#endif
