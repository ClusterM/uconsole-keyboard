#include "trackball.h"
#include "keyboard_state.h"
#include "hid_mouse.h"
#include "ratemeter.h"
#include "glider.h"
#include "math_utils.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include <math.h>

#define WHEEL_DENOM 2

typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_NUM
} Axis;

static volatile int8_t distances[AXIS_NUM];
static RateMeter rateMeter[AXIS_NUM];
static Glider glider[AXIS_NUM];
static int8_t wheelBuffer;
static bool asWheel = false;

static float rateToVelocityCurve(float input)
{
    return fabsf(input) / 30.0f;
}

// Interrupt handlers
void trackball_interrupt_x_neg(void)
{
    __disable_irq();
    distances[AXIS_X] -= 1;
    if (!asWheel) {
        ratemeter_onInterrupt(&rateMeter[AXIS_X]);
        glider_setDirection(&glider[AXIS_X], -1);
        
        const float rx = ratemeter_rate(&rateMeter[AXIS_X]);
        const float ry = ratemeter_rate(&rateMeter[AXIS_Y]);
        const float rate = hypot_f(rx, ry);
        const float ratio = rateToVelocityCurve(rate) / (rate > 0.001f ? rate : 1.0f);
        const float vx = rx * ratio;
        const float vy = ry * ratio;
        
        glider_update(&glider[AXIS_X], vx, (uint16_t)sqrtf(ratemeter_delta(&rateMeter[AXIS_X])));
        glider_updateSpeed(&glider[AXIS_Y], vy);
    }
    __enable_irq();
}

void trackball_interrupt_x_pos(void)
{
    __disable_irq();
    distances[AXIS_X] += 1;
    if (!asWheel) {
        ratemeter_onInterrupt(&rateMeter[AXIS_X]);
        glider_setDirection(&glider[AXIS_X], 1);
        
        const float rx = ratemeter_rate(&rateMeter[AXIS_X]);
        const float ry = ratemeter_rate(&rateMeter[AXIS_Y]);
        const float rate = hypot_f(rx, ry);
        const float ratio = rateToVelocityCurve(rate) / (rate > 0.001f ? rate : 1.0f);
        const float vx = rx * ratio;
        const float vy = ry * ratio;
        
        glider_update(&glider[AXIS_X], vx, (uint16_t)sqrtf(ratemeter_delta(&rateMeter[AXIS_X])));
        glider_updateSpeed(&glider[AXIS_Y], vy);
    }
    __enable_irq();
}

void trackball_interrupt_y_neg(void)
{
    __disable_irq();
    distances[AXIS_Y] -= 1;
    if (!asWheel) {
        ratemeter_onInterrupt(&rateMeter[AXIS_Y]);
        glider_setDirection(&glider[AXIS_Y], -1);
        
        const float rx = ratemeter_rate(&rateMeter[AXIS_X]);
        const float ry = ratemeter_rate(&rateMeter[AXIS_Y]);
        const float rate = hypot_f(rx, ry);
        const float ratio = rateToVelocityCurve(rate) / (rate > 0.001f ? rate : 1.0f);
        const float vx = rx * ratio;
        const float vy = ry * ratio;
        
        glider_updateSpeed(&glider[AXIS_X], vx);
        glider_update(&glider[AXIS_Y], vy, (uint16_t)sqrtf(ratemeter_delta(&rateMeter[AXIS_Y])));
    }
    __enable_irq();
}

void trackball_interrupt_y_pos(void)
{
    __disable_irq();
    distances[AXIS_Y] += 1;
    if (!asWheel) {
        ratemeter_onInterrupt(&rateMeter[AXIS_Y]);
        glider_setDirection(&glider[AXIS_Y], 1);
        
        const float rx = ratemeter_rate(&rateMeter[AXIS_X]);
        const float ry = ratemeter_rate(&rateMeter[AXIS_Y]);
        const float rate = hypot_f(rx, ry);
        const float ratio = rateToVelocityCurve(rate) / (rate > 0.001f ? rate : 1.0f);
        const float vx = rx * ratio;
        const float vy = ry * ratio;
        
        glider_updateSpeed(&glider[AXIS_X], vx);
        glider_update(&glider[AXIS_Y], vy, (uint16_t)sqrtf(ratemeter_delta(&rateMeter[AXIS_Y])));
    }
    __enable_irq();
}

void trackball_task(void)
{
    int8_t x = 0, y = 0, w = 0;
    
    __disable_irq();
    asWheel = keyboard_state.select_on == 1;
    
    if (asWheel) {
        ratemeter_expire(&rateMeter[AXIS_X]);
        ratemeter_expire(&rateMeter[AXIS_Y]);
        wheelBuffer = 0;
    } else {
        ratemeter_tick(&rateMeter[AXIS_X], 1);
        ratemeter_tick(&rateMeter[AXIS_Y], 1);
    }
    
    if (asWheel) {
        wheelBuffer += distances[AXIS_Y];
        w = wheelBuffer / WHEEL_DENOM;
        wheelBuffer -= w * WHEEL_DENOM;
    } else {
        GlideResult rX = glider_glide(&glider[AXIS_X], 1);
        GlideResult rY = glider_glide(&glider[AXIS_Y], 1);
        x = rX.value;
        y = rY.value;
        if (rX.stopped) {
            glider_stop(&glider[AXIS_Y]);
        }
        if (rY.stopped) {
            glider_stop(&glider[AXIS_Y]);
        }
    }
    
    distances[AXIS_X] = 0;
    distances[AXIS_Y] = 0;
    __enable_irq();
    
    if (x != 0 || y != 0 || w != 0) {
        hid_mouse_move(x, y, -w);
    }
}

void trackball_init(void)
{
    // Hall sensors are already configured as EXTI in MX_GPIO_Init
    ratemeter_init(&rateMeter[AXIS_X]);
    ratemeter_init(&rateMeter[AXIS_Y]);
    glider_init(&glider[AXIS_X]);
    glider_init(&glider[AXIS_Y]);
    
    distances[AXIS_X] = 0;
    distances[AXIS_Y] = 0;
    wheelBuffer = 0;
    asWheel = false;
}

