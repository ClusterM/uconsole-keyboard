#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// The time of inactivity in seconds after which the keyboard backlight will turn off
#define KEYBOARD_BACKLIGHT_OFF_TIME 15

// The duration of the keyboard backlight dim out effect in milliseconds
#define KEYBOARD_BACKLIGHT_DIM_OUT_DURATION 5000

// The value of the keyboard backlight when it is dimmed out
#define KEYBOARD_BACKLIGHT_DIMMED_OUT_VALUE 0

// The values for the keyboard backlight PWM (maximum value is 2000)
static const uint16_t backlight_vals[3] = {0, 500, 2000};

// The initial value of the keyboard backlight (index of the backlight_vals array)
#define KEYBOARD_INITIAL_BACKLIGHT_VALUE_ID 0

#endif