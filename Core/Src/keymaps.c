#include "keymaps.h"
#include "keyboard_state.h"
#include "hid_keyboard.h"
#include "hid_mouse.h"
#include "hid_consumer.h"
#include "main.h"
#include "stm32f1xx_hal.h"

KEYBOARD_STATE keyboard_state;

const uint16_t keyboard_maps[][MATRIX_KEYS] = {
    [DEF_LAYER] = {
        _SELECT_KEY, _START_KEY, _VOLUME_M, KEY_GRAVE, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_MINUS, KEY_EQUAL,
        KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8,
        KEY_9, KEY_0, KEY_ESC, KEY_TAB, EMP, EMP, EMP, EMP,
        KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I,
        KEY_O, KEY_P, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H,
        KEY_J, KEY_K, KEY_L, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
        KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE,
        KEY_BACKSPACE, KEY_RETURN, _FN_KEY, _FN_KEY, KEY_SPACE, EMP, EMP, EMP
    },
    
    [FN_LAYER] = {
        KEY_PRNT_SCRN, KEY_PAUSE, _VOLUME_MUTE, KEY_GRAVE, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_F11, KEY_F12,
        KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
        KEY_F9, KEY_F10, _FN_LOCK_KEYBOARD, KEY_CAPS_LOCK, EMP, EMP, EMP, EMP,
        KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_PAGE_UP, KEY_INSERT,
        KEY_O, KEY_P, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_HOME,
        KEY_END, KEY_PAGE_DOWN, KEY_L, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
        KEY_N, KEY_M, _FN_BRIGHTNESS_DOWN, _FN_BRIGHTNESS_UP, KEY_SLASH, KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE,
        KEY_DELETE, KEY_RETURN, _FN_KEY, _FN_KEY, _FN_LIGHT_KEYBOARD, EMP, EMP, EMP
    }
};

const uint16_t keys_maps[][KEYS_NUM] = {
    [DEF_LAYER] = {
        KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW,
        KEY_KEYPAD_PLUS, KEY_KEYPAD_MINUS, KEY_KEYPAD_ASTERISK, KEY_KEYPAD_SLASH, // Y, X, B, A -> +, -, *, /
        KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT, KEY_LEFT_CTRL, KEY_RIGHT_CTRL,
        KEY_LEFT_ALT, _MOUSE_LEFT, KEY_RIGHT_ALT, _MOUSE_RIGHT,
        _TRACKBALL_BTN
    },
    
    [FN_LAYER] = {
        KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END,
        KEY_KEYPAD_PLUS, KEY_KEYPAD_MINUS, KEY_KEYPAD_ASTERISK, KEY_KEYPAD_SLASH, // Y, X, B, A -> +, -, *, /
        _FN_SHIFT, _FN_SHIFT, KEY_LEFT_CTRL, KEY_RIGHT_CTRL,
        KEY_LEFT_GUI, _MOUSE_LEFT, KEY_RIGHT_ALT, _MOUSE_RIGHT,
        _TRACKBALL_BTN
    }
};

static uint16_t keyboard_pick_map[MATRIX_KEYS] = {0};
static uint16_t keys_pick_map[KEYS_NUM] = {0};

const uint16_t backlight_vals[3] = {0, 500, 2000};

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



static void keyboard_release_core(uint16_t k)
{
    switch (k) {
        case KEY_CAPS_LOCK:
            hid_keyboard_release(k);
            break;
            
        case _SELECT_KEY:
            // SELECT key - not used in new implementation
            break;
            
        case _START_KEY:
            // START key - not used in new implementation
            break;
            
        case _FN_BRIGHTNESS_UP:
            hid_consumer_release(CONSUMER_BRIGHTNESS_UP);
            break;
            
        case _FN_BRIGHTNESS_DOWN:
            hid_consumer_release(CONSUMER_BRIGHTNESS_DOWN);
            break;
            
        case _VOLUME_P:
            hid_consumer_release(CONSUMER_VOLUME_UP);
            break;
            
        case _VOLUME_M:
            // This can be either VOLUME_UP (with Shift) or VOLUME_DOWN (without Shift)
            // We need to release the one that was actually pressed
            // Since we don't track which one was pressed here, release both
            // (only the one that was actually pressed will have an effect)
            hid_consumer_release(CONSUMER_VOLUME_UP);
            hid_consumer_release(CONSUMER_VOLUME_DOWN);
            break;
            
        case _VOLUME_MUTE:
            hid_consumer_release(CONSUMER_MUTE);
            break;
            
        case _FN_KEY:
            keyboard_state.fn_on = 0;
            keyboard_state.fn.begin = 0;
            keyboard_state.fn.time = 0;
            break;
            
        default:
            // All keys are now HID codes (or special constants >= 0x100)
            // For regular HID codes (< 0x100), release directly
            // For special constants (>= 0x100), they should be handled above
            if (k < 0x100) {
                hid_keyboard_release((uint8_t)k);
            }
            break;
    }
}

static void keyboard_release(uint8_t addr, uint16_t k)
{
    if (keyboard_pick_map[addr] == 0) {
        // No stored value, use provided HID code
        keyboard_release_core(k);
    } else {
        // Use stored value (already HID code)
        uint16_t stored_key = keyboard_pick_map[addr];
        keyboard_pick_map[addr] = 0;
        keyboard_release_core(stored_key);
    }
}

void keyboard_action(uint8_t row, uint8_t col, uint8_t mode)
{
    uint16_t k;
    uint8_t addr = row * MATRIX_COLS + col;
    
    if (keyboard_state.fn_on > 0) {
        k = keyboard_maps[keyboard_state.fn_on][addr];
    } else {
        k = keyboard_maps[keyboard_state.layer][addr];
    }
    
    if (k == EMP) {
        return;
    }
    
    if (k != _FN_KEY && k != _FN_LOCK_KEYBOARD && keyboard_state.lock == 1) {
        return;
    }
    
    if (mode == KEY_PRESSED) {
        if (keyboard_pick_map[addr] == 0) {
            // Store key code (all keys are now HID codes, no ASCII conflicts)
            keyboard_pick_map[addr] = k;
        }
    }
    
    switch (k) {
        case KEY_CAPS_LOCK:
            if (mode == KEY_PRESSED) {
                hid_keyboard_press(k);
            } else if (mode == KEY_RELEASED) {
                keyboard_release(addr, k);
            }
            break;
            
        case _SELECT_KEY:
            // SELECT key - not used in new implementation
            // Keep select_on state for trackball wheel mode
            if (mode == KEY_PRESSED) {
                keyboard_state.select_on = 1;
            } else if (mode == KEY_RELEASED) {
                keyboard_state.select_on = 0;
            }
            break;
            
        case _START_KEY:
            // START key - not used in new implementation
            break;
            
        case _FN_BRIGHTNESS_UP:
            if (mode == KEY_PRESSED) {
                hid_consumer_press(CONSUMER_BRIGHTNESS_UP);
            } else {
                keyboard_release(addr, k);
            }
            break;
            
        case _FN_BRIGHTNESS_DOWN:
            if (mode == KEY_PRESSED) {
                hid_consumer_press(CONSUMER_BRIGHTNESS_DOWN);
            } else {
                keyboard_release(addr, k);
            }
            break;
            
        case _VOLUME_P:
            if (mode == KEY_PRESSED) {
                hid_consumer_press(CONSUMER_VOLUME_UP);
            } else {
                keyboard_release(addr, k);
            }
            break;
            
        case _VOLUME_M:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.sf_on > 0) {
                    // Shift was pressed - increase volume
                    // Release both shifts in HID report (so they don't affect other keys)
                    // But keep sf_on set (so subsequent presses will also increase volume)
                    // Release both shifts in one operation to avoid overwriting reports
                    // hid_keyboard_release_both_shifts() already waits for USB idle
                    hid_keyboard_release_both_shifts();
                    hid_consumer_press(CONSUMER_VOLUME_UP);
                } else {
                    // No shift - decrease volume
                    hid_consumer_press(CONSUMER_VOLUME_DOWN);
                }
            } else {
                keyboard_release(addr, k);
            }
            break;
            
        case _VOLUME_MUTE:
            if (mode == KEY_PRESSED) {
                hid_consumer_press(CONSUMER_MUTE);
            } else {
                keyboard_release(addr, k);
            }
            break;
            
        case _FN_LOCK_KEYBOARD:
            if (mode == KEY_PRESSED) {
                keyboard_state.lock = keyboard_state.lock ^ 1;
            }
            break;
            
        case _FN_LIGHT_KEYBOARD:
            if (mode == KEY_PRESSED) {
                keyboard_state.backlight++;
                if (keyboard_state.backlight >= 3) {
                    keyboard_state.backlight = 0;
                }
                // PWM will be set in main loop
            }
            break;
            
        case _FN_KEY:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.fn.lock == 0) {
                    keyboard_state.fn_on = FN_LAYER;
                    keyboard_state.fn.begin = k;
                }
            } else if (mode == KEY_RELEASED) {
                keyboard_state.fn_on = 0;
                keyboard_state.fn.begin = 0;
                keyboard_state.fn.time = 0;
            }
            break;
            
        default:
            // All keys are now HID codes, no conversion needed
            if (mode == KEY_PRESSED) {
                hid_keyboard_press((uint8_t)k);
            } else if (mode == KEY_RELEASED) {
                keyboard_release(addr, k);
            }
            break;
    }
}

static void keypad_release_core(uint16_t k)
{
    switch (k) {
        case _FN_SHIFT:
            break;
            
        case KEY_LEFT_SHIFT:
        case KEY_RIGHT_SHIFT:
            if (keyboard_state.shift.lock == 0) {
                hid_keyboard_release(k);
                keyboard_state.shift.begin = 0;
                keyboard_state.shift.time = 0;
                keyboard_state.sf_on = 0;
            }
            break;
            
        case _MOUSE_LEFT:
            hid_mouse_release(MOUSE_LEFT);
            break;
            
        case _MOUSE_MID:
            hid_mouse_release(MOUSE_MIDDLE);
            break;
            
        case _MOUSE_RIGHT:
            hid_mouse_release(MOUSE_RIGHT);
            break;
            
        case KEY_LEFT_CTRL:
        case KEY_RIGHT_CTRL:
            if (keyboard_state.ctrl.lock == 0) {
                hid_keyboard_clear_modifier((uint8_t)k);  // Use clear_modifier for explicit modifier handling
                keyboard_state.ctrl.begin = 0;
                keyboard_state.ctrl.time = 0;
            }
            break;
            
        case KEY_LEFT_ALT:
        case KEY_RIGHT_ALT:
            if (keyboard_state.alt.lock == 0) {
                hid_keyboard_clear_modifier((uint8_t)k);  // Use clear_modifier for explicit modifier handling
                keyboard_state.alt.begin = 0;
                keyboard_state.alt.time = 0;
            }
            break;
            
        case KEY_LEFT_GUI:
        case KEY_RIGHT_GUI:
            hid_keyboard_clear_modifier((uint8_t)k);  // Use clear_modifier for explicit modifier handling
            break;
            
        case _TRACKBALL_BTN:
            hid_mouse_release(MOUSE_MIDDLE);
            break;
            
        default:
            hid_keyboard_release(k);
            break;
    }
}

static void keypad_release(uint8_t addr, uint16_t k)
{
    if (keys_pick_map[addr] == 0) {
        keypad_release_core(k);
    } else {
        keypad_release_core(keys_pick_map[addr]);
        keys_pick_map[addr] = 0;
    }
}

void keypad_action(uint8_t col, uint8_t mode)
{
    uint16_t k;
    
    if (keyboard_state.fn_on > 0) {
        k = keys_maps[keyboard_state.fn_on][col];
    } else {
        k = keys_maps[keyboard_state.layer][col];
    }
    
    if (k == EMP) {
        return;
    }
    
    if (keyboard_state.lock == 1) {
        return;
    }
    
    if (mode == KEY_PRESSED) {
        if (keys_pick_map[col] == 0) {
            keys_pick_map[col] = k;
        }
    }
    
    switch (k) {
        case _FN_SHIFT:
            keypad_release(col, k);
            break;
            
        case KEY_LEFT_SHIFT:
        case KEY_RIGHT_SHIFT:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.shift.lock == 0) {
                    hid_keyboard_set_modifier((uint8_t)k);  // Use set_modifier for explicit modifier handling
                    keyboard_state.shift.begin = k;
                    keyboard_state.sf_on = k;
                }
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
            
        case _MOUSE_LEFT:
            if (mode == KEY_PRESSED) {
                hid_mouse_press(MOUSE_LEFT);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
            
        case _MOUSE_MID:
            if (mode == KEY_PRESSED) {
                hid_mouse_press(MOUSE_MIDDLE);
            } else {
                keypad_release(col, k);
            }
            break;
            
        case _MOUSE_RIGHT:
            if (mode == KEY_PRESSED) {
                hid_mouse_press(MOUSE_RIGHT);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
            
        case KEY_LEFT_CTRL:
        case KEY_RIGHT_CTRL:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.ctrl.lock == 0) {
                    hid_keyboard_set_modifier((uint8_t)k);  // Use set_modifier for explicit modifier handling
                    keyboard_state.ctrl.begin = k;
                }
            } else {
                keypad_release(col, k);
            }
            break;
            
        case KEY_LEFT_ALT:
        case KEY_RIGHT_ALT:
            if (mode == KEY_PRESSED) {
                if (keyboard_state.alt.lock == 0) {
                    hid_keyboard_set_modifier((uint8_t)k);  // Use set_modifier for explicit modifier handling
                    keyboard_state.alt.begin = k;
                }
            } else {
                keypad_release(col, k);
            }
            break;
            
        case KEY_LEFT_GUI:
        case KEY_RIGHT_GUI:
            if (mode == KEY_PRESSED) {
                hid_keyboard_set_modifier((uint8_t)k);  // Use set_modifier for explicit modifier handling
            } else {
                keypad_release(col, k);
            }
            break;
            
        case _TRACKBALL_BTN:
            if (mode == KEY_PRESSED) {
                hid_mouse_press(MOUSE_MIDDLE);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
            
        case KEY_KEYPAD_ASTERISK:
            // Check if Fn is pressed (Fn + * = bootloader)
            if (mode == KEY_PRESSED && keyboard_state.fn_on > 0) {
                // Jump to bootloader
                jump_to_bootloader();
            } else if (mode == KEY_PRESSED) {
                // Normal keypad * key
                hid_keyboard_press(KEY_KEYPAD_ASTERISK);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
            
        default:
            // All keys are now HID codes, no conversion needed
            if (mode == KEY_PRESSED) {
                hid_keyboard_press((uint8_t)k);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
    }
}

