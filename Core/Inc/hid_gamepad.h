#ifndef HID_GAMEPAD_H
#define HID_GAMEPAD_H

#include "main.h"
#include <stdint.h>
#include "usbd_custom_hid_if.h"

/* HID Gamepad button codes */
#define GAMEPAD_BUTTON_1   (GAMEPAD_BUTTON_FLAG | 1)
#define GAMEPAD_BUTTON_2   (GAMEPAD_BUTTON_FLAG | 2)
#define GAMEPAD_BUTTON_3   (GAMEPAD_BUTTON_FLAG | 3)
#define GAMEPAD_BUTTON_4   (GAMEPAD_BUTTON_FLAG | 4)
#define GAMEPAD_BUTTON_5   (GAMEPAD_BUTTON_FLAG | 5)
#define GAMEPAD_BUTTON_6   (GAMEPAD_BUTTON_FLAG | 6)
#define GAMEPAD_BUTTON_7   (GAMEPAD_BUTTON_FLAG | 7)
#define GAMEPAD_BUTTON_8   (GAMEPAD_BUTTON_FLAG | 8)

/* Directional buttons (affect axes) */
#define GAMEPAD_UP         (GAMEPAD_BUTTON_FLAG | 9)
#define GAMEPAD_DOWN       (GAMEPAD_BUTTON_FLAG | 10)
#define GAMEPAD_LEFT       (GAMEPAD_BUTTON_FLAG | 11)
#define GAMEPAD_RIGHT      (GAMEPAD_BUTTON_FLAG | 12)

/* Gamepad axis range: -127 to 127 */
#define GAMEPAD_AXIS_MIN   -127
#define GAMEPAD_AXIS_MAX   127
#define GAMEPAD_AXIS_CENTER 0

int8_t hid_gamepad_set_axes(int8_t x, int8_t y);
int8_t hid_gamepad_set_x(int8_t x);
int8_t hid_gamepad_set_y(int8_t y);
int8_t hid_gamepad_button(uint16_t button, uint8_t mode);
int8_t hid_gamepad_release_all(void);

#endif

