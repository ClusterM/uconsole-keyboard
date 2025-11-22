#ifndef HID_MOUSE_H
#define HID_MOUSE_H

#include <stdint.h>
#include "usbd_custom_hid_if.h"

#define MOUSE_LEFT     0x01
#define MOUSE_RIGHT    0x02
#define MOUSE_MIDDLE   0x04

int8_t hid_mouse_move(int8_t x, int8_t y, int8_t wheel);
int8_t hid_mouse_press(uint8_t button);
int8_t hid_mouse_release(uint8_t button);
int8_t hid_mouse_release_all(void);

#endif

