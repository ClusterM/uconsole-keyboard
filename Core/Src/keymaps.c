#include "keymaps.h"
#include "keyboard_state.h"
#include "hid_keyboard.h"
#include "hid_mouse.h"
#include "hid_consumer.h"
#include "main.h"
#include "stm32f1xx_hal.h"

KEYBOARD_STATE keyboard_state;

// Character to HID key code mapping
static uint8_t char_to_hid(uint8_t c)
{
    // Lowercase letters
    if (c >= 'a' && c <= 'z') {
        return 0x04 + (c - 'a');
    }
    // Numbers
    if (c >= '1' && c <= '9') {
        return 0x1E + (c - '1');
    }
    if (c == '0') {
        return 0x27;
    }
    // Special characters
    switch (c) {
        case ' ': return 0x2C;
        case '-': return 0x2D;
        case '=': return 0x2E;
        case '[': return 0x2F;
        case ']': return 0x30;
        case '\\': return 0x31;
        case ';': return 0x33;
        case '\'': return 0x34;
        case '`': return 0x35;
        case ',': return 0x36;
        case '.': return 0x37;
        case '/': return 0x38;
        default: return c;
    }
}

const uint16_t keyboard_maps[][MATRIX_KEYS] = {
    [DEF_LAYER] = {
        _SELECT_KEY, _START_KEY, _VOLUME_M, '`', '[', ']', '-', '=',
        '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', KEY_ESC, KEY_TAB, EMP, EMP, EMP, EMP,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
        'o', 'p', 'a', 's', 'd', 'f', 'g', 'h',
        'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b',
        'n', 'm', ',', '.', '/', '\\', ';', '\'',
        KEY_BACKSPACE, KEY_RETURN, _FN_KEY, _FN_KEY, ' ', EMP, EMP, EMP
    },
    
    [FN_LAYER] = {
        KEY_PRNT_SCRN, KEY_PAUSE, _VOLUME_MUTE, '`', '[', ']', KEY_F11, KEY_F12,
        KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8,
        KEY_F9, KEY_F10, _FN_LOCK_KEYBOARD, KEY_CAPS_LOCK, EMP, EMP, EMP, EMP,
        'q', 'w', 'e', 'r', 't', 'y', KEY_PAGE_UP, KEY_INSERT,
        'o', 'p', 'a', 's', 'd', 'f', 'g', KEY_HOME,
        KEY_END, KEY_PAGE_DOWN, 'l', 'z', 'x', 'c', 'v', 'b',
        'n', 'm', _FN_BRIGHTNESS_DOWN, _FN_BRIGHTNESS_UP, '/', '\\', ';', '\'',
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
        KEY_RIGHT_GUI, _MOUSE_LEFT, KEY_RIGHT_ALT, _MOUSE_RIGHT,
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

static uint8_t char_to_hid_release(uint16_t k)
{
    if (k < 128) {
        return char_to_hid((uint8_t)k);
    }
    return (uint8_t)k;
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
        case _FN_BRIGHTNESS_DOWN:
        case _VOLUME_P:
        case _VOLUME_M:
        case _VOLUME_MUTE:
            hid_consumer_release();
            break;
            
        case _FN_KEY:
            keyboard_state.fn_on = 0;
            keyboard_state.fn.begin = 0;
            keyboard_state.fn.time = 0;
            break;
            
        default:
            hid_keyboard_release(char_to_hid_release(k));
            break;
    }
}

static void keyboard_release(uint8_t addr, uint16_t k)
{
    if (keyboard_pick_map[addr] == 0) {
        keyboard_release_core(k);
    } else {
        keyboard_release_core(keyboard_pick_map[addr]);
        keyboard_pick_map[addr] = 0;
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
                    hid_keyboard_release(KEY_LEFT_SHIFT);
                    hid_keyboard_release(KEY_RIGHT_SHIFT);
                    hid_consumer_press(CONSUMER_VOLUME_UP);
                } else {
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
            if (mode == KEY_PRESSED) {
                // Convert character to HID code if needed
                uint8_t hid_code = (k < 128) ? char_to_hid(k) : k;
                hid_keyboard_press(hid_code);
            } else if (mode == KEY_RELEASED) {
                uint8_t hid_code = (k < 128) ? char_to_hid(k) : k;
                keyboard_release(addr, hid_code);
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
                hid_keyboard_release(k);
                keyboard_state.ctrl.begin = 0;
                keyboard_state.ctrl.time = 0;
            }
            break;
            
        case KEY_LEFT_ALT:
        case KEY_RIGHT_ALT:
            if (keyboard_state.alt.lock == 0) {
                hid_keyboard_release(k);
                keyboard_state.alt.begin = 0;
                keyboard_state.alt.time = 0;
            }
            break;
            
        case KEY_RIGHT_GUI:
            hid_keyboard_release(k);
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
                    hid_keyboard_press(k);
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
                    hid_keyboard_press(k);
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
                    hid_keyboard_press(k);
                    keyboard_state.alt.begin = k;
                }
            } else {
                keypad_release(col, k);
            }
            break;
            
        case KEY_RIGHT_GUI:
            if (mode == KEY_PRESSED) {
                hid_keyboard_press(k);
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
            if (mode == KEY_PRESSED) {
                // Convert character to HID code if needed
                uint8_t hid_code = (k < 128) ? char_to_hid(k) : k;
                hid_keyboard_press(hid_code);
            } else if (mode == KEY_RELEASED) {
                keypad_release(col, k);
            }
            break;
    }
}

