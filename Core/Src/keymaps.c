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
        SK_SELECT_KEY, SK_START_KEY, SK_VOLUME_M, KEY_GRAVE, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_MINUS, KEY_EQUAL,
        KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8,
        KEY_9, KEY_0, KEY_ESC, KEY_TAB, EMP, EMP, EMP, EMP,
        KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I,
        KEY_O, KEY_P, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H,
        KEY_J, KEY_K, KEY_L, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
        KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE,
        KEY_BACKSPACE, KEY_RETURN, SK_FN_KEY, SK_FN_KEY, KEY_SPACE, EMP, EMP, EMP
    },
    
    [FN_LAYER] = {
        KEY_PRNT_SCRN, KEY_PAUSE, SK_VOLUME_MUTE, KEY_GRAVE, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_F11, KEY_F12,
        KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
        KEY_F9, KEY_F10, SK_KEYBOARD_LOCK, KEY_CAPS_LOCK, EMP, EMP, EMP, EMP,
        KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_PAGE_UP, KEY_INSERT,
        KEY_O, KEY_P, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_HOME,
        KEY_END, KEY_PAGE_DOWN, KEY_L, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
        KEY_N, KEY_M, SK_BRIGHTNESS_DOWN, SK_BRIGHTNESS_UP, KEY_SLASH, KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE,
        KEY_DELETE, KEY_RETURN, SK_FN_KEY, SK_FN_KEY, SK_KEYBOARD_LIGHT, EMP, EMP, EMP
    }
};

// Mappings for the non-matrix keys
const uint16_t keys_maps[][KEYS_NUM] = {
    [DEF_LAYER] = {
        SK_MOUSE_MID,           // Trackball button
        KEY_UP_ARROW,           // Up
        KEY_DOWN_ARROW,         // Down
        KEY_LEFT_ARROW,         // Left
        KEY_RIGHT_ARROW,        // Right
        KEY_KEYPAD_PLUS,        // Gamepad Y
        KEY_KEYPAD_MINUS,       // Gamepad X
        KEY_KEYPAD_ASTERISK,    // Gamepad B
        KEY_KEYPAD_SLASH,       // Gamepad A
        KEY_LEFT_SHIFT,         // Left shift
        KEY_RIGHT_SHIFT,        // Right shift
        KEY_LEFT_CTRL,          // Left ctrl
        KEY_RIGHT_CTRL,         // Right ctrl
        KEY_LEFT_ALT,           // Left alt
        SK_MOUSE_LEFT,          // Gamepad L
        KEY_RIGHT_ALT,          // Right alt
        SK_MOUSE_RIGHT          // Gamepad R
    },
    
    [FN_LAYER] = {
        SK_MOUSE_MID,           // Trackball button
        KEY_PAGE_UP,            // Up
        KEY_PAGE_DOWN,          // Down
        KEY_HOME,               // Left
        KEY_END,                // Right
        KEY_KEYPAD_PLUS,        // Gamepad Y
        KEY_KEYPAD_MINUS,       // Gamepad X
        KEY_KEYPAD_ASTERISK,    // Gamepad B
        KEY_KEYPAD_SLASH,       // Gamepad A
        KEY_LEFT_SHIFT,         // Left shift
        KEY_RIGHT_SHIFT,        // Right shift
        KEY_LEFT_CTRL,          // Left ctrl
        KEY_RIGHT_CTRL,         // Right ctrl
        KEY_LEFT_GUI,           // Left alt
        SK_MOUSE_LEFT,          // Gamepad L
        KEY_RIGHT_ALT,          // Right alt
        SK_MOUSE_RIGHT          // Gamepad R
    }
};

static uint16_t matrix_pick_map[MATRIX_KEYS] = {0};
static uint16_t non_matrix_pick_map[KEYS_NUM] = {0};
static void jump_to_bootloader(void);

static void do_the_key(uint16_t k, uint8_t mode)
{
    switch (k) {
        case EMP:
            break;

        case SK_FN_KEY:
            keyboard_state.layer = mode == KEY_PRESSED ? FN_LAYER : DEF_LAYER;
            break;

        case KEY_LEFT_CTRL:
        case KEY_RIGHT_CTRL:
        case KEY_LEFT_ALT:
        case KEY_RIGHT_ALT:
        case KEY_LEFT_SHIFT:
        case KEY_RIGHT_SHIFT:
        case KEY_LEFT_GUI:
        case KEY_RIGHT_GUI:
            k &= ~MODIFIER_FLAG; // Clear internal code flag
            if (mode == KEY_PRESSED) {
                keyboard_state.mod_keys_on |= k;
            } else {
                keyboard_state.mod_keys_on &= ~k;
            }
            hid_keyboard_modifier(k, mode);
            break;

        case SK_MOUSE_LEFT:
        case SK_MOUSE_MID:
        case SK_MOUSE_RIGHT:
            hid_mouse_button(k, mode);
            break;            
                        
        case SK_SELECT_KEY:
        case SK_START_KEY:
            // TODO?
            break;

        case SK_BRIGHTNESS_UP:
            hid_consumer_button(CONSUMER_BRIGHTNESS_UP, mode);
            break;
            
        case SK_BRIGHTNESS_DOWN:
            hid_consumer_button(CONSUMER_BRIGHTNESS_DOWN, mode);
            break;

        case SK_VOLUME_P:
            hid_consumer_button(CONSUMER_VOLUME_UP, mode);
            break;

        case SK_VOLUME_M:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.mod_keys_on & (KEY_LEFT_SHIFT | KEY_RIGHT_SHIFT)) {
                    // Shift was pressed - increase volume, but release the shift first
                    hid_keyboard_modifier((uint8_t)(KEY_LEFT_SHIFT | KEY_RIGHT_SHIFT), KEY_RELEASED);
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

        case SK_VOLUME_MUTE:
            hid_consumer_button(CONSUMER_MUTE, mode);
            break;

        case SK_KEYBOARD_LOCK:
            if (mode == KEY_PRESSED) {
                keyboard_state.lock ^= 1;
            }
            break;

        case SK_KEYBOARD_LIGHT:
            if (mode == KEY_PRESSED) {
                keyboard_state.backlight++;
                if (keyboard_state.backlight >= 3) {
                    keyboard_state.backlight = 0;
                }
                // PWM will be set in main loop
            }
            break;

        case KEY_KEYPAD_ASTERISK:
            // Special case for keypad * key - update mode
            // Check if Fn is pressed (Fn + * = bootloader)
            if (mode == KEY_PRESSED && keyboard_state.layer == FN_LAYER) {
                // Jump to bootloader
                jump_to_bootloader();
            }
            hid_keyboard_button(KEY_KEYPAD_ASTERISK, mode);
            break;

        default:
            if (k < 0x100) {
                hid_keyboard_button((uint8_t)k, mode);
            }
            break;
    }
}

void matrix_action(uint8_t row, uint8_t col, uint8_t mode)
{
    uint16_t k;
    uint8_t addr = row * MATRIX_COLS + col;
    
    k = matrix_maps[keyboard_state.layer][addr];
    
    if (k == EMP) {
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
    
    if (k == EMP) {
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

/**
  * @brief  Reset to bootloader via watchdog
  * @retval None (never returns)
  */
static void jump_to_bootloader(void)
{
    IWDG->KR = 0x5555;
    IWDG->PR = 0;
    IWDG->RLR = 1;
    IWDG->KR = 0xCCCC;
    while (1);  
}
