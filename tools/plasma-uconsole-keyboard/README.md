# uConsole Keyboard Layer plasmoid

KDE Plasma panel widget that shows and switches the uConsole keyboard layer (0–14) and FN lock. It reads state in real time from the keyboard via hidraw (Report ID 5) and can set layer/FN lock by running the scripts.

## Requirements

- Linux with hidraw
- uConsole keyboard firmware that exposes Report ID 5 (layer + FN lock)
- Scripts `keyboard_state.sh` and `hidraw_listener.sh` from this repo’s `tools/` directory
- **udev rule** so the device is readable/writable without root (see below)

## udev rule

Copy the rule and reload udev, then re-plug the keyboard or reboot:

```bash
sudo cp tools/70-uconsole-keyboard.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
```

Ensure your user is in the `plugdev` group (or change `GROUP` in the rule and use that group):

```bash
sudo usermod -aG plugdev $USER
# log out and back in
```

## Build and install

From the project root:

```bash
cd tools/plasma-uconsole-keyboard
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

Or install to your user (no sudo for install):

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make
make install
```

Then install the scripts so the plasmoid can find them:

- **Default**: copy `keyboard_state.sh` and `hidraw_listener.sh` into the **parent** of the plasmoid package directory.
  - If the plasmoid is at `~/.local/share/plasma/plasmoids/org.kde.plasma.uconsole-keyboard/`, copy the two scripts into `~/.local/share/plasma/plasmoids/`.
- **Or** set “Scripts directory” in the widget configuration to the directory that contains both scripts.

## Test

```bash
plasmawindowed org.kde.plasma.uconsole-keyboard
```

(Requires `plasma-sdk`.)

## Usage

- **Panel**: shows current layer (e.g. “L0”) and “FN” when FN lock is on. Click to open the popup.
- **Popup**: choose layer 0–14 from the list and toggle FN lock. Changes are sent to the keyboard immediately; the panel updates when the device sends Report ID 5.
