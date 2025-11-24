#ifndef KEYBOARD_STATE_H
#define KEYBOARD_STATE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool deing;
    uint32_t de_time;
} KEY_DEB;

// Sticky keys functionality - currently not used/implemented
// typedef struct {
//     uint16_t lock;
//     uint16_t time;
//     uint16_t begin;
// } KEYBOARD_LOCK;

typedef struct {
    uint8_t layer;
    uint16_t mod_keys_on;
    uint8_t backlight;
    uint8_t lock;
    uint32_t last_activity_time;
} KEYBOARD_STATE;

extern KEYBOARD_STATE keyboard_state;

#endif

