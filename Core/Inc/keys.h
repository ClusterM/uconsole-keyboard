#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#ifndef KEY_DEBOUNCE
#   define KEY_DEBOUNCE 5
#endif

#define KEYS_NUM 17

void keys_init(void);
void keys_task(void);

#endif

