#include "hid_consumer.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

int8_t hid_consumer_press(uint16_t code)
{
    uint8_t consumer_report[2] = {0x03, (uint8_t)code}; // Report ID 3
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, consumer_report, 2);
}

int8_t hid_consumer_release(void)
{
    uint8_t consumer_report[2] = {0x03, 0x00}; // Report ID 3
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, consumer_report, 2);
}

