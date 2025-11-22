#include "keys.h"
#include "keyboard_state.h"
#include "keymaps.h"
#include "main.h"
#include "stm32f1xx_hal.h"

static uint32_t keys;
static uint32_t keys_debouncing;
static uint32_t keys_prev;

KEY_DEB keypad_debouncing;

// Pin definitions for keys
static const GPIO_TypeDef* keys_port[KEYS_NUM] = {
    KEY1_GPIO_Port, KEY2_GPIO_Port, KEY3_GPIO_Port, KEY4_GPIO_Port,
    KEY5_GPIO_Port, KEY6_GPIO_Port, KEY7_GPIO_Port, KEY8_GPIO_Port,
    KEY9_GPIO_Port, KEY10_GPIO_Port, KEY11_GPIO_Port, KEY12_GPIO_Port,
    KEY13_GPIO_Port, KEY14_GPIO_Port, KEY15_GPIO_Port, KEY16_GPIO_Port,
    KEY1_GPIO_Port  // KEY0 - using KEY1 port for now, need to check actual pin
};

static const uint16_t keys_pin[KEYS_NUM] = {
    KEY1_Pin, KEY2_Pin, KEY3_Pin, KEY4_Pin,
    KEY5_Pin, KEY6_Pin, KEY7_Pin, KEY8_Pin,
    KEY9_Pin, KEY10_Pin, KEY11_Pin, KEY12_Pin,
    KEY13_Pin, KEY14_Pin, KEY15_Pin, KEY16_Pin,
    KEY1_Pin  // KEY0
};

static uint8_t read_io(uint8_t idx)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(keys_port[idx], keys_pin[idx]);
    return (state == GPIO_PIN_RESET) ? 1 : 0; // Inverted because of pullup
}

uint8_t scan_keys(void)
{
    uint32_t data = 0;
    uint8_t s;
    
    for (int i = 0; i < KEYS_NUM; i++) {
        s = read_io(i);
        data |= s << i;
    }
    
    if (keys_debouncing != data) {
        keys_debouncing = data;
        keypad_debouncing.deing = true;
        keypad_debouncing.de_time = HAL_GetTick();
    }
    
    if (keypad_debouncing.deing == true && 
        ((HAL_GetTick() - keypad_debouncing.de_time) > KEY_DEBOUNCE)) {
        keys = keys_debouncing;
        keypad_debouncing.deing = false;
    }
    
    return 1;
}

void keys_task(void)
{
    scan_keys();
    
    uint32_t mask = 1;
    uint32_t change = 0;
    uint32_t pressed = 0;
    
    change = keys ^ keys_prev;
    
    if (change) {
        for (uint8_t c = 0; c < KEYS_NUM; c++, mask <<= 1) {
            if (change & mask) {
                pressed = keys & mask;
                if (pressed) {
                    keypad_action(c, KEY_PRESSED);
                } else {
                    keypad_action(c, KEY_RELEASED);
                }
                keys_prev ^= mask;
            }
        }
    }
}

void keys_init(void)
{
    // Keys are already configured as inputs with pullup in MX_GPIO_Init
    keypad_debouncing.deing = false;
    keypad_debouncing.de_time = 0;
}

