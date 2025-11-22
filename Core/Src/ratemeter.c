#include "ratemeter.h"
#include "stm32f1xx_hal.h"

void ratemeter_init(RateMeter* rm)
{
    rm->lastTime = 0;
    rm->averageDelta = 0;
    rm->timeout = 0;
}

void ratemeter_onInterrupt(RateMeter* rm)
{
    const uint32_t now = HAL_GetTick();
    if (rm->timeout == 0) {
        rm->averageDelta = CUTOFF_MS;
    } else {
        const uint32_t delta = getDeltaMax(rm->lastTime, now, CUTOFF_MS);
        rm->averageDelta = (rm->averageDelta + delta) / 2;
    }
    rm->lastTime = now;
    rm->timeout = CUTOFF_MS;
}

void ratemeter_tick(RateMeter* rm, uint8_t delta)
{
    if (rm->timeout > delta) {
        rm->timeout -= delta;
    } else {
        rm->timeout = 0;
    }
    if (rm->timeout != 0) {
        rm->averageDelta += delta;
    }
}

void ratemeter_expire(RateMeter* rm)
{
    rm->timeout = 0;
}

uint16_t ratemeter_delta(const RateMeter* rm)
{
    return rm->averageDelta;
}

float ratemeter_rate(const RateMeter* rm)
{
    if (rm->timeout == 0) {
        return 0.0f;
    } else if (rm->averageDelta == 0) {
        return 1000.0f;
    } else {
        return 1000.0f / (float)rm->averageDelta;
    }
}

