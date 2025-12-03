#include "hid_gamepad.h"
#include "hid_keyboard.h"  // For hid_wait_for_usb_idle()
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

// Static variable to maintain gamepad state
static uint8_t gamepad_buttons = 0;
static int8_t gamepad_x = 0;
static int8_t gamepad_y = 0;

// Directional buttons state
static uint8_t dir_up = 0;
static uint8_t dir_down = 0;
static uint8_t dir_left = 0;
static uint8_t dir_right = 0;

int8_t hid_gamepad_set_axes(int8_t x, int8_t y)
{
    // Clamp values to valid range
    if (x < GAMEPAD_AXIS_MIN) x = GAMEPAD_AXIS_MIN;
    if (x > GAMEPAD_AXIS_MAX) x = GAMEPAD_AXIS_MAX;
    if (y < GAMEPAD_AXIS_MIN) y = GAMEPAD_AXIS_MIN;
    if (y > GAMEPAD_AXIS_MAX) y = GAMEPAD_AXIS_MAX;
    
    gamepad_x = x;
    gamepad_y = y;
    
    // Report ID (1 byte) + X (1 byte) + Y (1 byte) + Buttons (1 byte) = 4 bytes
    uint8_t gamepad_report[4] = {0x04, (uint8_t)gamepad_x, (uint8_t)gamepad_y, gamepad_buttons};
    int8_t result = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, gamepad_report, 4);
    hid_wait_for_usb_idle();
    return result;
}

int8_t hid_gamepad_set_x(int8_t x)
{
    return hid_gamepad_set_axes(x, gamepad_y);
}

int8_t hid_gamepad_set_y(int8_t y)
{
    return hid_gamepad_set_axes(gamepad_x, y);
}

static void hid_gamepad_update_axes_from_directions(void)
{
    // Calculate X axis based on left/right
    if (dir_left && dir_right) {
        gamepad_x = GAMEPAD_AXIS_CENTER; // Both pressed = center
    } else if (dir_left) {
        gamepad_x = GAMEPAD_AXIS_MIN;
    } else if (dir_right) {
        gamepad_x = GAMEPAD_AXIS_MAX;
    } else {
        gamepad_x = GAMEPAD_AXIS_CENTER;
    }
    
    // Calculate Y axis based on up/down
    if (dir_up && dir_down) {
        gamepad_y = GAMEPAD_AXIS_CENTER; // Both pressed = center
    } else if (dir_up) {
        gamepad_y = GAMEPAD_AXIS_MIN;
    } else if (dir_down) {
        gamepad_y = GAMEPAD_AXIS_MAX;
    } else {
        gamepad_y = GAMEPAD_AXIS_CENTER;
    }
}

static int8_t hid_gamepad_send_report(void)
{
    // Report ID (1 byte) + X (1 byte) + Y (1 byte) + Buttons (1 byte) = 4 bytes
    uint8_t gamepad_report[4] = {0x04, (uint8_t)gamepad_x, (uint8_t)gamepad_y, gamepad_buttons};
    int8_t result = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, gamepad_report, 4);
    hid_wait_for_usb_idle();
    return result;
}

int8_t hid_gamepad_button(uint16_t button, uint8_t mode)
{
    uint8_t is_pressed = (mode == KEY_PRESSED) ? 1 : 0;    
    int button_bit = -1;

    switch (button) {
        // Directional buttons (affect axes)
        case GAMEPAD_UP: // GAMEPAD_UP
            dir_up = is_pressed;
            hid_gamepad_update_axes_from_directions();
            break;
        case GAMEPAD_DOWN: // GAMEPAD_DOWN
            dir_down = is_pressed;
            hid_gamepad_update_axes_from_directions();
            break;
        case GAMEPAD_LEFT: // GAMEPAD_LEFT
            dir_left = is_pressed;
            hid_gamepad_update_axes_from_directions();
            break;            
        case GAMEPAD_RIGHT: // GAMEPAD_RIGHT
            dir_right = is_pressed;
            hid_gamepad_update_axes_from_directions();
            break;        
        case GAMEPAD_BUTTON_1: // GAMEPAD_BUTTON_1
            button_bit = 0; break;
        case GAMEPAD_BUTTON_2: // GAMEPAD_BUTTON_2
            button_bit = 1; break;
        case GAMEPAD_BUTTON_3: // GAMEPAD_BUTTON_3
            button_bit = 2; break;
        case GAMEPAD_BUTTON_4: // GAMEPAD_BUTTON_4
            button_bit = 3; break;
        case GAMEPAD_BUTTON_5: // GAMEPAD_BUTTON_5
            button_bit = 4; break;
        case GAMEPAD_BUTTON_6: // GAMEPAD_BUTTON_6
            button_bit = 5; break;
        case GAMEPAD_BUTTON_7: // GAMEPAD_BUTTON_7
            button_bit = 6; break;
        case GAMEPAD_BUTTON_8: // GAMEPAD_BUTTON_8
            button_bit = 7; break;
    }

    if (button_bit != -1) {
        if (is_pressed) {
                gamepad_buttons |= (1 << button_bit);
        } else {
            gamepad_buttons &= ~(1 << button_bit);
        }
    }

    return hid_gamepad_send_report();
}

int8_t hid_gamepad_release_all(void)
{
    gamepad_buttons = 0;
    gamepad_x = GAMEPAD_AXIS_CENTER;
    gamepad_y = GAMEPAD_AXIS_CENTER;
    dir_up = 0;
    dir_down = 0;
    dir_left = 0;
    dir_right = 0;
    return hid_gamepad_send_report();
}

