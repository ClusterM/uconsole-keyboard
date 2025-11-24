# uConsole Keyboard Firmware - Clean STM32 HAL Port

This is a clean port of the official uConsole keyboard firmware from [ClockworkPi's repository](https://github.com/clockworkpi/uConsole/tree/master/Code/uconsole_keyboard), rewritten to use **pure STM32 HAL** instead of the messy Arduino/STM32duino framework.


## Why This Exists

The original firmware relies on STM32duino and various Arduino libraries with complex dependencies. This makes it difficult to:
- Edit and compile the firmware directly on the uConsole itself
- Customize key mappings and functionality
- Understand what's actually happening under the hood
- Maintain and debug the code

This port completely eliminates all that Arduino nonsense and uses clean STM32 HAL code. Now you can easily edit, compile, and flash the keyboard firmware right on your uConsole, remapping keys and customizing functionality however you want.


## What's Included

✅ **Fully ported features:**
- Full keyboard matrix scanning (8×8 matrix)
- All key mappings and layouts
- Individual button handling (17 buttons)
- Fn key functionality and all Fn combinations
- Trackball operation with complete acceleration calculations (glider, ratemeter)
- Backlight brightness control via PWM
- USB HID keyboard and mouse support
- USB Consumer controls (volume, brightness)

❌ **Removed features:**
- Gamepad/joystick functionality (does anyone actually use this?)

🔧 **What's changed/added:**
- **Horizontal scrolling!** You can now use Fn + Trackball to scroll horizontally as well as vertically.
- **Keyboard backlight dimming after inactivity** - The keyboard backlight brightness automatically decreases after a period of inactivity (configurable via `Core/Inc/config.h`)
- **Y/X/B/A buttons** now serve desktop-friendly actions: Y = **mouse left click** (Fn+Y = **keypad /**), X = **Left GUI/Cmd** (Fn+X = **keypad \*** + bootloader shortcut), B = **mouse Back** (Fn+B = **keypad -**), A = **mouse Forward** (Fn+A = **keypad +**).
- **Fn + Alt** combination now triggers the **left Cmd/GUI key** instead of the right one, because Raspberry Pi OS only responds to the left Cmd key by default.
- **Fn + X** (the X button in the gamepad section) now triggers a **bootloader jump** for easy firmware updates. The `make flash` command will wait for you to manually enter DFU mode using this combination.


## How to change key bindings

You can configure all key bindings, key combinations, and behaviors in the `Core/Src/keymaps.c` file.


## Building

Simply use `make`:

```bash
# Build the firmware
make all

# Clean build files
make clean

# Flash the firmware
make flash
```

**Important for first-time flashing:**

For the **first firmware flash**, you need to use one of these methods:

1. **Official flashing script**: Use the official ClockworkPi flashing script from [uconsole_keyboard_flash.tar.gz](https://github.com/clockworkpi/uConsole/raw/master/Bin/uconsole_keyboard_flash.tar.gz) (replace `uconsole_keyboard.ino.bin` with the new firmware)

2. **Hardware switch**: Use the switch on the back of the keyboard to switch it to update/flashing mode

After the first flash, you can use `make flash`. The command will wait for you to manually enter DFU mode by pressing Fn+X combination on the keyboard.


## License

Based on the original uConsole keyboard firmware by ClockworkPi.


## Support the Developer and the Project

* [GitHub Sponsors](https://github.com/sponsors/ClusterM)
* [Buy Me A Coffee](https://www.buymeacoffee.com/cluster)
* [Sber](https://messenger.online.sberbank.ru/sl/Lnb2OLE4JsyiEhQgC)
* [Donation Alerts](https://www.donationalerts.com/r/clustermeerkat)
* [Boosty](https://boosty.to/cluster)