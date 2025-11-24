#!/bin/sh

DEVICE_NAME="ClockworkPI uConsole Keyboard"
DFU_VID_PID="1eaf:0003"
DEVICE_VID_PID="1eaf:0024"

find_led_file() {
    for led in $(find /sys/class/leds -name "*$1"); do
        device_name=$(cat $led/device/name)
        if [ "$device_name" = "$DEVICE_NAME" ]; then
            echo $led
            break
        fi
    done
    return 1
}

bootloader_via_leds() {
    echo "Rebootting to the bootloader via LEDs..."
    led_compose=$(find_led_file "compose")
    led_kana=$(find_led_file "kana")
    if [ -z "$led_compose" ] || [ -z "$led_kana" ]; then
        echo "LED files not found"
        return 1
    fi

    echo 0 > "$led_compose/brightness"
    echo 0 > "$led_kana/brightness"
    echo 1 > "$led_compose/brightness"
    echo 1 > "$led_kana/brightness"

    return 0
}

wait_for_bootloader() {
    TIMEOUT=30; \
    while ! lsusb | grep -q "$DFU_VID_PID"; do
        echo -n "."
        sleep 1
        TIMEOUT=$((TIMEOUT - 1))
        if [ $TIMEOUT -eq 0 ]; then
            echo
            echo "Timeout waiting for DFU mode"
            exit 1
        fi
    done
    echo
    exit 0
}

# If not Linux or lsusb is not installed, exit
if [ "$(uname -s)" != "Linux" ] || ! command -v lsusb &> /dev/null; then
    echo "Can't put the keyboard into DFU mode automatically on this platform, please put it into DFU mode manually"
    exit 0
fi

# Check if the keyboard is preset
if lsusb | grep -q "$DEVICE_VID_PID"; then
    bootloader_via_leds
    wait_for_bootloader
else
    echo "Connect the keyboard to the computer and put it into DFU mode"
    wait_for_bootloader
fi
