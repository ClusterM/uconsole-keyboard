#ifndef GLIDER_H
#define GLIDER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int8_t direction;
    float speed;
    uint16_t sustain;
    uint16_t release;
    float error;
} Glider;

typedef struct {
    int8_t value;
    bool stopped;
} GlideResult;

void glider_init(Glider* g);
void glider_setDirection(Glider* g, int8_t direction);
void glider_update(Glider* g, float velocity, uint16_t sustain);
void glider_updateSpeed(Glider* g, float velocity);
void glider_stop(Glider* g);
GlideResult glider_glide(Glider* g, uint8_t delta);

#endif

