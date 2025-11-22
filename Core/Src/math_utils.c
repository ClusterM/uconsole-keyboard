#include "math_utils.h"

uint32_t getDelta(uint32_t prev, uint32_t now)
{
    uint32_t delta;
    if (now >= prev) {
        delta = now - prev;
    } else {
        delta = UINT32_MAX - prev + now + 1;
    }
    return delta;
}

uint32_t getDeltaMax(uint32_t prev, uint32_t now, uint32_t max)
{
    const uint32_t delta = getDelta(prev, now);
    if (delta < max) {
        return delta;
    }
    return max;
}

