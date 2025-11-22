#ifndef HID_KEYBOARD_H
#define HID_KEYBOARD_H

#include <stdint.h>
#include "usbd_custom_hid_if.h"

// HID Keyboard key codes
#define KEY_LEFT_CTRL      0x01
#define KEY_LEFT_SHIFT     0x02
#define KEY_LEFT_ALT       0x04
#define KEY_LEFT_GUI       0x08
#define KEY_RIGHT_CTRL     0x10
#define KEY_RIGHT_SHIFT    0x20
#define KEY_RIGHT_ALT      0x40
#define KEY_RIGHT_GUI      0x80

#define KEY_UP_ARROW       0x52
#define KEY_DOWN_ARROW     0x51
#define KEY_LEFT_ARROW     0x50
#define KEY_RIGHT_ARROW    0x4F
#define KEY_BACKSPACE      0x2A
#define KEY_TAB            0x2B
#define KEY_RETURN         0x28
#define KEY_ESC            0x29
#define KEY_INSERT         0x49
#define KEY_DELETE         0x4C
#define KEY_PAGE_UP        0x4B
#define KEY_PAGE_DOWN      0x4E
#define KEY_HOME           0x4A
#define KEY_END            0x4D
#define KEY_CAPS_LOCK      0x39

#define KEY_F1             0x3A
#define KEY_F2             0x3B
#define KEY_F3             0x3C
#define KEY_F4             0x3D
#define KEY_F5             0x3E
#define KEY_F6             0x3F
#define KEY_F7             0x40
#define KEY_F8             0x41
#define KEY_F9             0x42
#define KEY_F10            0x43
#define KEY_F11            0x44
#define KEY_F12            0x45

int8_t hid_keyboard_press(uint8_t key);
int8_t hid_keyboard_release(uint8_t key);
int8_t hid_keyboard_release_all(void);

#endif

