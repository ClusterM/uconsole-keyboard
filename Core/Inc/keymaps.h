#ifndef KEYMAPS_H
#define KEYMAPS_H

#include "main.h"
#include <stdint.h>
#include "keyboard_matrix.h"
#include "keyboard_non_matrix.h"

/* Special keys and virtual keys */
enum SKEYS {
  SK_FN_KEY = SPECIAL_KEY_FLAG,
  SK_KEYBOARD_LOCK,
  SK_KEYBOARD_LIGHT
};

/* Layers */

#define DEF_LAYER 0x00
#define FN_LAYER 0x01
#define GAME_LAYER 0x02

extern const uint16_t matrix_maps[][MATRIX_KEYS];
extern const uint16_t keys_maps[][KEYS_NUM];

void matrix_action(uint8_t row, uint8_t col, uint8_t mode);
void non_matrix_action(uint8_t col, uint8_t mode);

#endif

