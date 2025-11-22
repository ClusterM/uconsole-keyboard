#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define MATRIX_ROWS 8
#define MATRIX_COLS 8
#define MATRIX_KEYS 64

#ifndef DEBOUNCE
#   define DEBOUNCE 20
#endif

#define KEY_PRESSED 1
#define KEY_RELEASED 0

#define KEY_PRNT_SCRN 0xCE
#define KEY_PAUSE  0xd0

#define KEY_VOLUME_UP 0x108
#define KEY_VOLUME_DOWN 0x109

void keyboard_init(void);
void keyboard_task(void);

#endif

