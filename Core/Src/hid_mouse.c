#include "hid_mouse.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

int8_t hid_mouse_move(int8_t x, int8_t y, int8_t wheel)
{
    uint8_t mouse_report[5] = {0x02, 0, (uint8_t)x, (uint8_t)y, (uint8_t)wheel}; // Report ID 2 + 4 bytes data
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

int8_t hid_mouse_press(uint8_t button)
{
    uint8_t mouse_report[5] = {0x02, button, 0, 0, 0}; // Report ID 2 + 4 bytes data
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

int8_t hid_mouse_release(uint8_t button)
{
    uint8_t mouse_report[5] = {0x02, 0, 0, 0, 0}; // Report ID 2 + 4 bytes data
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

