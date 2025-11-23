#ifndef KEYMAPS_H
#define KEYMAPS_H

#include <stdint.h>
#include "keyboard_matrix.h"
#include "keyboard_non_matrix.h"

#define EMP 0xFFFF

/* Special keys and virtual keys */
enum SKEYS {
  SK_FN_KEY = 0x2000,
  SK_SELECT_KEY,
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
  SK_MOUSE_LEFT,
  SK_MOUSE_MID,
  SK_MOUSE_RIGHT,
  SK_BRIGHTNESS_UP,
  SK_BRIGHTNESS_DOWN,
  SK_VOLUME_M,
  SK_VOLUME_P,
  SK_VOLUME_MUTE,
  SK_KEYBOARD_LOCK,
  SK_KEYBOARD_LIGHT
};

#define DEF_LAYER 0x00
#define FN_LAYER 0x01

extern const uint16_t matrix_maps[][MATRIX_KEYS];
extern const uint16_t keys_maps[][KEYS_NUM];

void matrix_action(uint8_t row, uint8_t col, uint8_t mode);
void non_matrix_action(uint8_t col, uint8_t mode);

#endif

