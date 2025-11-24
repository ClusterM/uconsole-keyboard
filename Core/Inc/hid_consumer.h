#ifndef HID_CONSUMER_H
#define HID_CONSUMER_H

#include "main.h"
#include <stdint.h>
#include "usbd_custom_hid_if.h"

// Consumer Control codes
#define CONSUMER_VOLUME_UP          (CONSUMER_KEY_FLAG | 0xE9)
#define CONSUMER_VOLUME_DOWN        (CONSUMER_KEY_FLAG | 0xEA)
#define CONSUMER_MUTE               (CONSUMER_KEY_FLAG | 0xE2)
#define CONSUMER_BRIGHTNESS_UP      (CONSUMER_KEY_FLAG | 0x6F)
#define CONSUMER_BRIGHTNESS_DOWN    (CONSUMER_KEY_FLAG | 0x70)
#define CONSUMER_SCAN_NEXT_TRACK    (CONSUMER_KEY_FLAG | 0xB5)
#define CONSUMER_SCAN_PREV_TRACK    (CONSUMER_KEY_FLAG | 0xB6)
#define CONSUMER_STOP               (CONSUMER_KEY_FLAG | 0xB7)
#define CONSUMER_PLAY_PAUSE         (CONSUMER_KEY_FLAG | 0xCD)

int8_t hid_consumer_button(uint16_t code, uint8_t mode);

#endif

