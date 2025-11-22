#ifndef HID_CONSUMER_H
#define HID_CONSUMER_H

#include <stdint.h>
#include "usbd_custom_hid_if.h"

// Consumer Control codes
#define CONSUMER_VOLUME_UP          0xE9
#define CONSUMER_VOLUME_DOWN        0xEA
#define CONSUMER_MUTE               0xE2
#define CONSUMER_BRIGHTNESS_UP       0x6F
#define CONSUMER_BRIGHTNESS_DOWN     0x70

int8_t hid_consumer_press(uint16_t code);
int8_t hid_consumer_release(void);

#endif

