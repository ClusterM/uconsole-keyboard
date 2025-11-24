#ifndef LEDS_H
#define LEDS_H

#define KB_LED_NUM_LOCK 0x01
#define KB_LED_CAPS_LOCK 0x02
#define KB_LED_SCROLL_LOCK 0x04
#define KB_LED_COMPOSE 0x08
#define KB_LED_KANA 0x10

void leds_set_state(uint8_t state);

#endif