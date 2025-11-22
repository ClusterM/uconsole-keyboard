#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdint.h>
#include <math.h>
#include <limits.h>

uint32_t getDelta(uint32_t prev, uint32_t now);
uint32_t getDeltaMax(uint32_t prev, uint32_t now, uint32_t max);

static inline int8_t sign(int32_t value) {
    if (value > 0) return 1;
    if (value < 0) return -1;
    return 0;
}

static inline int8_t clamp_int8(int32_t value) {
    if (value >= 127) return 127;
    if (value <= -127) return -127;
    return (int8_t)value;
}

static inline int16_t min_int16(int16_t x, int16_t y) {
    return (x < y) ? x : y;
}

static inline float hypot_f(float x, float y) {
    return sqrtf(x * x + y * y);
}

#endif

