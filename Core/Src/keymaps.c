#include "keymaps.h"
#include "keyboard_state.h"
#include "hid_keyboard.h"
#include "hid_mouse.h"
#include "hid_consumer.h"
#include "main.h"
#include "stm32f1xx_hal.h"

KEYBOARD_STATE keyboard_state;

// Mappings for the keyboard matrix
const uint16_t matrix_maps[][MATRIX_KEYS] = {
    [DEF_LAYER] = {
        KEY_NONE,              KEY_NONE,              CONSUMER_VOLUME_DOWN,     KEY_GRAVE,              KEY_LEFT_BRACE,    KEY_RIGHT_BRACE,   KEY_MINUS,        KEY_EQUAL,
        KEY_1,                 KEY_2,                 KEY_3,                    KEY_4,                  KEY_5,             KEY_6,             KEY_7,            KEY_8,
        KEY_9,                 KEY_0,                 KEY_ESC,                  KEY_TAB,                KEY_NONE,          KEY_NONE,          KEY_NONE,         KEY_NONE,
        KEY_Q,                 KEY_W,                 KEY_E,                    KEY_R,                  KEY_T,             KEY_Y,             KEY_U,            KEY_I,
        KEY_O,                 KEY_P,                 KEY_A,                    KEY_S,                  KEY_D,             KEY_F,             KEY_G,            KEY_H,
        KEY_J,                 KEY_K,                 KEY_L,                    KEY_Z,                  KEY_X,             KEY_C,             KEY_V,            KEY_B,
        KEY_N,                 KEY_M,                 KEY_COMMA,                KEY_DOT,                KEY_SLASH,         KEY_BACKSLASH,     KEY_SEMICOLON,    KEY_APOSTROPHE,
        KEY_BACKSPACE,         KEY_RETURN,            SK_FN_KEY,                SK_FN_KEY,              KEY_SPACE,         KEY_NONE,          KEY_NONE,         KEY_NONE
    },
    [FN_LAYER] = {
        KEY_PRNT_SCRN,         KEY_PAUSE,             CONSUMER_MUTE,            0,                      0,                 0,                 KEY_F11,          KEY_F12,
        KEY_F1,                KEY_F2,                KEY_F3,                   KEY_F4,                 KEY_F5,            KEY_F6,            KEY_F7,           KEY_F8,
        KEY_F9,                KEY_F10,               SK_KEYBOARD_LOCK,         KEY_CAPS_LOCK,          0,                 0,                 0,                0,
        0,                     0,                     0,                        0,                      0,                 0,                 0,                0,
        0,                     0,                     0,                        0,                      0,                 0,                 0,                0,
        0,                     0,                     0,                        0,                      0,                 0,                 0,                0,
        0,                     0,                     CONSUMER_BRIGHTNESS_DOWN, CONSUMER_BRIGHTNESS_UP, 0,                 0,                 0,                0,
        KEY_DELETE,            0,                     0,                        0,                      SK_KEYBOARD_LIGHT, 0,                 0,                0
    }
};

// Mappings for the non-matrix keys
const uint16_t keys_maps[][KEYS_NUM] = {
    [DEF_LAYER] = {
        MOUSE_MIDDLE,           // Trackball button
        KEY_UP_ARROW,           // Up
        KEY_DOWN_ARROW,         // Down
        KEY_LEFT_ARROW,         // Left
        KEY_RIGHT_ARROW,        // Right
        KEY_RETURN,             // Gamepad A
        KEY_LEFT_GUI,           // Gamepad B
        MOUSE_RIGHT,            // Gamepad X
        MOUSE_LEFT,             // Gamepad Y
        KEY_LEFT_SHIFT,         // Left shift
        KEY_RIGHT_SHIFT,        // Right shift
        KEY_LEFT_CTRL,          // Left ctrl
        KEY_RIGHT_CTRL,         // Right ctrl
        KEY_LEFT_ALT,           // Left alt
        MOUSE_LEFT,             // Gamepad L
        KEY_RIGHT_ALT,          // Right alt
        MOUSE_RIGHT,            // Gamepad R
    },
    [FN_LAYER] = {
        0,                      // Trackball button
        KEY_PAGE_UP,            // Up
        KEY_PAGE_DOWN,          // Down
        KEY_HOME,               // Left
        KEY_END,                // Right
        KEY_KEYPAD_PLUS,        // Gamepad A
        KEY_KEYPAD_MINUS,       // Gamepad B
        MOUSE_FORWARD,          // Gamepad X
        MOUSE_BACK,             // Gamepad Y
        0,                      // Left shift
        0,                      // Right shift
        0,                      // Left ctrl
        0,                      // Right ctrl
        0,                      // Left alt
        0,                      // Gamepad L
        0,                      // Right alt
        0,                      // Gamepad R
    }
};

static uint16_t matrix_pick_map[MATRIX_KEYS] = {0};
static uint16_t non_matrix_pick_map[KEYS_NUM] = {0};

static void do_the_key(uint16_t k, uint8_t mode)
{
    switch (k) {
        case SK_FN_KEY:
            keyboard_state.layer = mode == KEY_PRESSED ? FN_LAYER : DEF_LAYER;
            break;

        case CONSUMER_VOLUME_DOWN:
            /* Override volume down button, so shift+vol_down = vol_up */
            if (mode == KEY_PRESSED) {
                if (keyboard_state.mod_keys_on & (KEY_LEFT_SHIFT | KEY_RIGHT_SHIFT)) {
                    // Shift was pressed - increase volume, but release the shift first
                    hid_keyboard_modifier(KEY_LEFT_SHIFT | KEY_RIGHT_SHIFT, KEY_RELEASED);
                    hid_consumer_button(CONSUMER_VOLUME_UP, KEY_PRESSED);
                } else {
                    // No shift - decrease volume
                    hid_consumer_button(CONSUMER_VOLUME_DOWN, KEY_PRESSED);
                }
            } else {
                hid_consumer_button(CONSUMER_VOLUME_UP, KEY_RELEASED);
                hid_consumer_button(CONSUMER_VOLUME_DOWN, KEY_RELEASED);
            }
            break;

        case SK_KEYBOARD_LOCK:
            if (mode == KEY_PRESSED) {
                keyboard_state.lock ^= 1;
            }
            break;

        case SK_KEYBOARD_LIGHT:
            if (mode == KEY_PRESSED) {
                keyboard_state.backlight++;
                if (keyboard_state.backlight >= (sizeof(backlight_vals) / sizeof(backlight_vals[0]))) {
                    keyboard_state.backlight = 0;
                }
                // PWM will be set in main loop
            }
            break;

        default:
            if (k == KEY_NONE) {
                break;
            } else if (k & CONSUMER_KEY_FLAG) {
                hid_consumer_button(k, mode);
            } else if (k & MODIFIER_KEY_FLAG) {
                hid_keyboard_modifier(k, mode);
                if (mode == KEY_PRESSED) {
                    keyboard_state.mod_keys_on |= (uint8_t)k;
                } else {
                    keyboard_state.mod_keys_on &= ~(uint8_t)k;
                }                
            } else if (k & MOUSE_BUTTON_FLAG) {
                hid_mouse_button(k, mode);
            } else if (k < 0x100) {
                hid_keyboard_button(k, mode);
            }
            break;
    }
}

void matrix_action(uint8_t row, uint8_t col, uint8_t mode)
{
    uint16_t k;
    uint8_t addr = row * MATRIX_COLS + col;

    k = matrix_maps[keyboard_state.layer][addr];
    if (!k) {
        k = matrix_maps[DEF_LAYER][addr];
    }

    if (k == KEY_NONE) {
        return;
    }
    
    if (k != SK_FN_KEY && k != SK_KEYBOARD_LOCK && keyboard_state.lock == 1) {
        return;
    }
    
    if (mode == KEY_PRESSED) {
        if (matrix_pick_map[addr] == 0) {
            matrix_pick_map[addr] = k;
        }
        do_the_key(k, KEY_PRESSED);
    } else {
        if (matrix_pick_map[addr] == 0) {
            // No stored value, use provided HID code
            do_the_key(k, KEY_RELEASED);
        } else {
            // Use stored value (already HID code)
            uint16_t stored_key = matrix_pick_map[addr];
            matrix_pick_map[addr] = 0;
            do_the_key(stored_key, KEY_RELEASED);
        }
    }
}

void non_matrix_action(uint8_t col, uint8_t mode)
{
    uint16_t k;
    
    k = keys_maps[keyboard_state.layer][col];
    if (!k) {
        k = keys_maps[DEF_LAYER][col];
    }
    
    if (k == KEY_NONE) {
        return;
    }

    if (k != SK_FN_KEY && k != SK_KEYBOARD_LOCK && keyboard_state.lock == 1) {
        return;
    }
    
    if (mode == KEY_PRESSED) {
        if (non_matrix_pick_map[col] == 0) {
            non_matrix_pick_map[col] = k;
        }
        do_the_key(k, KEY_PRESSED);
    } else {
        if (non_matrix_pick_map[col] == 0) {
            do_the_key(k, KEY_RELEASED);
        } else {
            do_the_key(non_matrix_pick_map[col], KEY_RELEASED);
            non_matrix_pick_map[col] = 0;
        }
    }
}
