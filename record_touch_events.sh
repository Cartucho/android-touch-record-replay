#!/bin/bash

echo "Looking for touchscreen device..."
TOUCH_DEVICE=`./find_touchscreen_name.sh`

echo "$TOUCH_DEVICE"

# Check if input device exists
if [[ "$TOUCH_DEVICE" = *"Touchscreen device found!"* ]]
then
    # Device found! Start recording
    echo "Recording will start as soon as you put your finger in the touchscreen."
    echo "Press ctrl+c to stop recording..."
    `adb shell getevent -t "${TOUCH_DEVICE#*-> }" > recorded_touch_events.txt`
fi
