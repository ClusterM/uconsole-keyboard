#include "main.h"
#include "leds.h"
#include "keyboard_state.h"

static void reboot_magic_check(uint8_t state)
{
    if (state & (KB_LED_COMPOSE | KB_LED_KANA)) {
        jump_to_bootloader();
    }
}

void leds_set_state(uint8_t state)
{
    reboot_magic_check(state);
}