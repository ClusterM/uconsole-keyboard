#include "hid_keyboard.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

static uint8_t keyboard_report[9] = {0x01, 0, 0, 0, 0, 0, 0, 0, 0}; // Report ID 1 + 8 bytes data

int8_t hid_keyboard_press(uint8_t key)
{
    // Check if it's a modifier key (Ctrl, Shift, Alt, GUI)
    if (key == KEY_LEFT_CTRL || key == KEY_RIGHT_CTRL) {
        keyboard_report[1] |= KEY_LEFT_CTRL;
    } else if (key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT) {
        keyboard_report[1] |= KEY_LEFT_SHIFT;
    } else if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT) {
        keyboard_report[1] |= KEY_LEFT_ALT;
    } else if (key == KEY_LEFT_GUI || key == KEY_RIGHT_GUI) {
        keyboard_report[1] |= KEY_LEFT_GUI;
    } else {
        // Regular keys - find empty slot
        for (int i = 3; i < 9; i++) {
            if (keyboard_report[i] == 0) {
                keyboard_report[i] = key;
                break;
            }
        }
    }
    
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, keyboard_report, 9);
}

int8_t hid_keyboard_release(uint8_t key)
{
    // Check if it's a modifier key
    if (key == KEY_LEFT_CTRL || key == KEY_RIGHT_CTRL) {
        keyboard_report[1] &= ~KEY_LEFT_CTRL;
    } else if (key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT) {
        keyboard_report[1] &= ~KEY_LEFT_SHIFT;
    } else if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT) {
        keyboard_report[1] &= ~KEY_LEFT_ALT;
    } else if (key == KEY_LEFT_GUI || key == KEY_RIGHT_GUI) {
        keyboard_report[1] &= ~KEY_LEFT_GUI;
    } else {
        // Regular keys
        for (int i = 3; i < 9; i++) {
            if (keyboard_report[i] == key) {
                keyboard_report[i] = 0;
                break;
            }
        }
    }
    
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, keyboard_report, 9);
}

int8_t hid_keyboard_release_all(void)
{
    keyboard_report[0] = 0x01; // Report ID
    for (int i = 1; i < 9; i++) {
        keyboard_report[i] = 0;
    }
    
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, keyboard_report, 9);
}

