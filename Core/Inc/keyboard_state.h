#ifndef KEYBOARD_STATE_H
#define KEYBOARD_STATE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool deing;
    uint32_t de_time;
} KEY_DEB;

typedef struct {
    uint16_t lock;
    uint16_t time;
    uint16_t begin;
} KEYBOARD_LOCK;

typedef struct {
    uint8_t layer;
    uint8_t prev_layer;
    uint8_t fn_on;
    uint8_t select_on;
    uint16_t sf_on;
    
    uint8_t backlight;
    uint8_t lock;
    
    KEYBOARD_LOCK ctrl;
    KEYBOARD_LOCK shift;
    KEYBOARD_LOCK alt;
    KEYBOARD_LOCK fn;
} KEYBOARD_STATE;

extern KEYBOARD_STATE keyboard_state;

#endif

