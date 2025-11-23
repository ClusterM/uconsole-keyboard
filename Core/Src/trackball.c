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
static int8_t hWheelBuffer;  // Horizontal wheel buffer
static bool asWheel = false;
static bool lastWheelMode = false;

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
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();
    uint8_t delta = 1;
    
    if (last_time != 0) {
        uint32_t time_delta = current_time - last_time;
        // Clamp delta to reasonable range (1-255 ms, uint8_t limit)
        if (time_delta == 0) {
            time_delta = 1; // Minimum 1 ms
        } else if (time_delta > 255) {
            time_delta = 255; // Maximum uint8_t
        }
        delta = (uint8_t)time_delta;
    } else {
        // First call - initialize last_time
        last_time = current_time;
        return; // Skip first iteration to get proper delta next time
    }
    last_time = current_time;
    
    int8_t x = 0, y = 0, w = 0;
    
    __disable_irq();
    // Use fn_on instead of select_on for wheel mode (Fn + trackball)
    asWheel = keyboard_state.layer > 0;
    
    // Reset wheel buffers only when switching modes
    if (asWheel != lastWheelMode) {
        if (asWheel) {
            ratemeter_expire(&rateMeter[AXIS_X]);
            ratemeter_expire(&rateMeter[AXIS_Y]);
            wheelBuffer = 0;
            hWheelBuffer = 0;
        }
        lastWheelMode = asWheel;
    }
    
    if (asWheel) {
        // Vertical scroll (wheel) - Y axis
        wheelBuffer += distances[AXIS_Y];
        w = wheelBuffer / WHEEL_DENOM;
        wheelBuffer -= w * WHEEL_DENOM;
        
        // Horizontal scroll (pan) - X axis
        hWheelBuffer += distances[AXIS_X];
        x = 0;  // No X movement in wheel mode
        y = 0;  // No Y movement in wheel mode
    } else {
        ratemeter_tick(&rateMeter[AXIS_X], delta);
        ratemeter_tick(&rateMeter[AXIS_Y], delta);
        GlideResult rX = glider_glide(&glider[AXIS_X], delta);
        GlideResult rY = glider_glide(&glider[AXIS_Y], delta);
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
    
    if (asWheel) {
        // In wheel mode, use pan for horizontal scroll
        int8_t hw = 0;
        if (hWheelBuffer != 0) {
            hw = hWheelBuffer / WHEEL_DENOM;
            hWheelBuffer -= hw * WHEEL_DENOM;
        }
        if (w != 0 || hw != 0) {
            hid_mouse_move_with_pan(0, 0, -w, hw);  // Invert horizontal scroll direction
        }
    } else {
        if (x != 0 || y != 0 || w != 0) {
            hid_mouse_move(x, y, -w);
        }
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
    hWheelBuffer = 0;
    asWheel = false;
    lastWheelMode = false;
}

