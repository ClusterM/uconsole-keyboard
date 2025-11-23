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

void matrix_init(void);
void matrix_task(void);

#endif

