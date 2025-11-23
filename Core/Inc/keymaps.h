#ifndef KEYMAPS_H
#define KEYMAPS_H

#include <stdint.h>
#include "keyboard.h"
#include "keys.h"

#define EMP 0xFFFF

enum SKEYS {
  SK_SELECT_KEY = 0xe8,
  SK_START_KEY,
  SK_JOYSTICK_UP,
  SK_JOYSTICK_DOWN,
  SK_JOYSTICK_LEFT,
  SK_JOYSTICK_RIGHT,
  SK_JOYSTICK_A,
  SK_JOYSTICK_B,
  SK_JOYSTICK_X,
  SK_JOYSTICK_Y,
  SK_JOYSTICK_L,
  SK_JOYSTICK_R,
  SK_FN_KEY,
  SK_MOUSE_LEFT,
  SK_MOUSE_MID,
  SK_MOUSE_RIGHT,
  SK_FN_BRIGHTNESS_UP,
  SK_FN_BRIGHTNESS_DOWN,
  SK_VOLUME_M,
  SK_VOLUME_P,
  SK_VOLUME_MUTE,
  SK_FN_LOCK_KEYBOARD,
  SK_FN_LIGHT_KEYBOARD
};

#define DEF_LAYER 0x00
#define FN_LAYER 0x01

extern const uint16_t keyboard_maps[][MATRIX_KEYS];
extern const uint16_t keys_maps[][KEYS_NUM];

void keyboard_action(uint8_t row, uint8_t col, uint8_t mode);
void keypad_action(uint8_t col, uint8_t mode);

#endif

