#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// The values for the keyboard backlight PWM (maximum value is 2000)
static const uint16_t backlight_vals[3] = {0, 500, 2000};

// The initial value of the keyboard backlight (index of the backlight_vals array)
#define KEYBOARD_INITIAL_BACKLIGHT_VALUE_ID 1

// The time of inactivity in seconds after which the keyboard backlight will turn off
// 0 = never turn off
#define KEYBOARD_BACKLIGHT_OFF_TIME 15

// The duration of the keyboard backlight dim out effect in milliseconds
#define KEYBOARD_BACKLIGHT_DIM_OUT_DURATION 5000

// The value of the keyboard backlight when it is dimmed out
#define KEYBOARD_BACKLIGHT_DIMMED_OUT_VALUE 0

// If defined, the keyboard backlight will resume when the trackball is moved
#define KEYBOARD_BACKLIGHT_RESUME_BY_TRACKBALL

// Trackball pointer acceleration curve parameters
#define TRACKBALL_ACCELERATION_EXPONENT 1.2f
#define TRACKBALL_ACCELERATION_DIVISOR 80.0f

// Trackball scroll acceleration curve parameters
#define TRACKBALL_SCROLL_VERTICAL_EXPONENT 1.30f
#define TRACKBALL_SCROLL_VERTICAL_DIVISOR 100.0f
#define TRACKBALL_SCROLL_HORIZONTAL_EXPONENT 1.20f
#define TRACKBALL_SCROLL_HORIZONTAL_DIVISOR 150.0f

// Trackball scroll step denominators (higher = larger physical movement per scroll tick)
#define TRACKBALL_SCROLL_VERTICAL_DENOM 2
#define TRACKBALL_SCROLL_HORIZONTAL_DENOM 3

#endif