#ifndef RATEMETER_H
#define RATEMETER_H

#include <stdint.h>
#include "math_utils.h"

#define CUTOFF_MS 1000

typedef struct {
    uint32_t lastTime;
    uint32_t averageDelta;
    uint32_t timeout;
} RateMeter;

void ratemeter_init(RateMeter* rm);
void ratemeter_onInterrupt(RateMeter* rm);
void ratemeter_tick(RateMeter* rm, uint8_t delta);
void ratemeter_expire(RateMeter* rm);
uint16_t ratemeter_delta(const RateMeter* rm);
float ratemeter_rate(const RateMeter* rm);

#endif

