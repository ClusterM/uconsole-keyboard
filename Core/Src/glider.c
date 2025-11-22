#include "glider.h"
#include "math_utils.h"
#include <math.h>

void glider_init(Glider* g)
{
    g->speed = 0;
    g->sustain = 0;
    g->release = 0;
    g->error = 0;
    g->direction = 0;
}

void glider_setDirection(Glider* g, int8_t direction)
{
    if (g->direction != direction) {
        glider_stop(g);
    }
    g->direction = direction;
}

void glider_update(Glider* g, float speed, uint16_t sustain)
{
    g->speed = speed;
    g->sustain = sustain;
    g->release = sustain;
}

void glider_updateSpeed(Glider* g, float velocity)
{
    g->speed = velocity;
}

void glider_stop(Glider* g)
{
    g->speed = 0;
    g->sustain = 0;
    g->release = 0;
    g->error = 0;
}

GlideResult glider_glide(Glider* g, uint8_t delta)
{
    const bool alreadyStopped = (g->speed == 0);
    
    g->error += g->speed * delta;
    int8_t distance = 0;
    if (g->error > 0) {
        distance = clamp_int8((int32_t)ceilf(g->error));
    }
    g->error -= distance;
    
    if (g->sustain > 0) {
        const uint16_t sustained = min_int16(g->sustain, (uint16_t)delta);
        g->sustain -= sustained;
    } else if (g->release > 0) {
        const uint16_t released = min_int16(g->release, (uint16_t)delta);
        g->speed = g->speed * (g->release - released) / g->release;
        g->release -= released;
    } else {
        g->speed = 0;
    }
    
    const int8_t result = g->direction * distance;
    GlideResult res = { result, !alreadyStopped && (g->speed == 0) };
    return res;
}

