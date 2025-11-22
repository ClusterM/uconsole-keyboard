#include "hid_mouse.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

// Static variable to maintain button state
static uint8_t mouse_buttons = 0;

int8_t hid_mouse_move(int8_t x, int8_t y, int8_t wheel)
{
    // Report ID (1 byte) + buttons (1 byte) + X (1 byte) + Y (1 byte) + wheel (1 byte) = 5 bytes
    uint8_t mouse_report[5] = {0x02, mouse_buttons, (uint8_t)x, (uint8_t)y, (uint8_t)wheel};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

int8_t hid_mouse_press(uint8_t button)
{
    mouse_buttons |= button;
    // Report ID (1 byte) + buttons (1 byte) + X (1 byte) + Y (1 byte) + wheel (1 byte) = 5 bytes
    uint8_t mouse_report[5] = {0x02, mouse_buttons, 0, 0, 0};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

int8_t hid_mouse_release(uint8_t button)
{
    mouse_buttons &= ~button;
    // Report ID (1 byte) + buttons (1 byte) + X (1 byte) + Y (1 byte) + wheel (1 byte) = 5 bytes
    uint8_t mouse_report[5] = {0x02, mouse_buttons, 0, 0, 0};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

int8_t hid_mouse_release_all(void)
{
    mouse_buttons = 0;
    uint8_t mouse_report[5] = {0x02, 0, 0, 0, 0};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, mouse_report, 5);
}

