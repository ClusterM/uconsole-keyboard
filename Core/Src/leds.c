#include "main.h"
#include "leds.h"
#include "keyboard_state.h"

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