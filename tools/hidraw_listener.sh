#!/bin/bash
#
# Continuously read from uConsole keyboard hidraw and output only Report ID 5
# (layer + FN lock). Other reports are filtered by known sizes (1→9, 2→6, 3→3, 4→4, 5→4).
# Output format: "layer=N fn_lock=on" or "layer=N fn_lock=off" (one line per Report 5).
#

DEVICE_VID="1eaf"
DEVICE_PID="0024"

# Report ID -> size in bytes (from HID descriptor)
report_size() {
    case "$1" in
        01) echo 9 ;;   # Keyboard
        02) echo 6 ;;   # Mouse
        03) echo 3 ;;   # Consumer
        04) echo 4 ;;   # Gamepad
        05) echo 4 ;;   # Vendor (layer/FN)
        *)  echo 1 ;;
    esac
}

find_hidraw() {
    for d in /sys/class/hidraw/hidraw*; do
        [[ -d "$d" ]] || continue
        [[ -e "$d/device" ]] || continue
        dev="/dev/$(basename "$d")"
        p=$(readlink -f "$d/device" 2>/dev/null) || p="$d/device"
        while [[ -n "$p" && "$p" != "/" ]]; do
            if [[ -f "$p/idVendor" && -f "$p/idProduct" ]]; then
                ven=$(cat "$p/idVendor" 2>/dev/null | tr 'A-F' 'a-f')
                prod=$(cat "$p/idProduct" 2>/dev/null | tr 'A-F' 'a-f')
                if [[ "$ven" == "$DEVICE_VID" && "$prod" == "$DEVICE_PID" ]]; then
                    echo "$dev"
                    return 0
                fi
                break
            fi
            p=$(dirname "$p")
        done
    done
    return 1
}

[[ "$(uname -s)" == "Linux" ]] || exit 1
HIDRAW=$(find_hidraw)
[[ -n "$HIDRAW" ]] || { echo "Keyboard not found (${DEVICE_VID}:${DEVICE_PID})" >&2; exit 1; }

exec 3<> "$HIDRAW" || { echo "Cannot open $HIDRAW" >&2; exit 1; }

while true; do
    # Read one byte (Report ID)
    id_hex=$(dd bs=1 count=1 <&3 2>/dev/null | xxd -p 2>/dev/null | tr -d '\n')
    [[ -n "$id_hex" && ${#id_hex} -ge 2 ]] || break
    id_hex="${id_hex:0:2}"

    size=$(report_size "$id_hex")
    rest=$(( size - 1 ))
    [[ $rest -le 0 ]] && continue

    # Read remaining bytes of this report
    rest_hex=$(dd bs=1 count=$rest <&3 2>/dev/null | xxd -p -c 256 2>/dev/null | tr -d '\n')
    [[ ${#rest_hex} -ge $(( rest * 2 )) ]] || break

    if [[ "$id_hex" == "05" && $size -eq 4 ]]; then
        # data[0] = first byte of rest = first 2 hex chars
        b0_hex="${rest_hex:0:2}"
        b0=$(( 16#$b0_hex ))
        layer=$(( b0 & 0x0F ))
        fn_lock=$(( (b0 & 0x10) != 0 ))
        if [[ $fn_lock -eq 1 ]]; then
            echo "layer=$layer fn_lock=on"
        else
            echo "layer=$layer fn_lock=off"
        fi
    fi
done

exit 1
