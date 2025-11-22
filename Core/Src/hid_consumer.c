#include "hid_consumer.h"
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

// Convert 16-bit usage code to bit position in report
// This is a simplified mapping - only supports specific codes
static uint8_t usage_to_bit(uint16_t code)
{
    switch (code) {
        case CONSUMER_VOLUME_UP:      return 0;   // Bit 0
        case CONSUMER_VOLUME_DOWN:    return 1;   // Bit 1
        case CONSUMER_MUTE:           return 2;   // Bit 2
        case CONSUMER_BRIGHTNESS_UP:  return 3;   // Bit 3
        case CONSUMER_BRIGHTNESS_DOWN: return 4;  // Bit 4
        default:                      return 255; // Invalid
    }
}

int8_t hid_consumer_press(uint16_t code)
{
    uint8_t bit = usage_to_bit(code);
    if (bit == 255) return USBD_FAIL; // Invalid code
    
    // Report ID (1 byte) + 16-bit bit field (2 bytes) = 3 bytes total
    uint8_t consumer_report[3] = {0x03, (uint8_t)(1 << bit), 0x00};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, consumer_report, 3);
}

int8_t hid_consumer_release(void)
{
    // Report ID (1 byte) + all bits cleared (2 bytes) = 3 bytes total
    uint8_t consumer_report[3] = {0x03, 0x00, 0x00};
    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, consumer_report, 3);
}

