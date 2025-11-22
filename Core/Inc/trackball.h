#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <stdint.h>
#include "main.h"

void trackball_init(void);
void trackball_task(void);
void trackball_interrupt_x_neg(void);
void trackball_interrupt_x_pos(void);
void trackball_interrupt_y_neg(void);
void trackball_interrupt_y_pos(void);

#endif

