#ifndef KEYMAPS_H
#define KEYMAPS_H

#include <stdint.h>
#include "keyboard.h"
#include "keys.h"

#define EMP 0xFFFF

enum SKEYS {
  _SELECT_KEY = 0xe8,
  _START_KEY,
  _JOYSTICK_UP,
  _JOYSTICK_DOWN,
  _JOYSTICK_LEFT,
  _JOYSTICK_RIGHT,
  _JOYSTICK_A,
  _JOYSTICK_B,
  _JOYSTICK_X,
  _JOYSTICK_Y,
  _JOYSTICK_L,
  _JOYSTICK_R,
  _FN_KEY,
  _MOUSE_LEFT,
  _MOUSE_MID,
  _MOUSE_RIGHT,
  _FN_BRIGHTNESS_UP,
  _FN_BRIGHTNESS_DOWN,
  _VOLUME_M,
  _VOLUME_P,
  _VOLUME_MUTE,
  _TRACKBALL_BTN,
  _FN_LOCK_KEYBOARD,
  _FN_LIGHT_KEYBOARD,
  _FN_SHIFT
};

#define DEF_LAYER 0x00
#define FN_LAYER 0x01

extern const uint16_t keyboard_maps[][MATRIX_KEYS];
extern const uint16_t keys_maps[][KEYS_NUM];

void keyboard_action(uint8_t row, uint8_t col, uint8_t mode);
void keypad_action(uint8_t col, uint8_t mode);

#endif

