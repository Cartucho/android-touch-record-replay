#!/bin/bash

echo "Looking for touchscreen device..."
TOUCH_DEVICE=`./find_touchscreen_name.sh`

echo "$TOUCH_DEVICE"

# Check if the file `mysendevent` exists in the phone

MYSENDEVENT=`adb shell ls /data/local/tmp/mysendevent`

if [[ "$MYSENDEVENT" = *"No such file or directory"* ]]
then
    `adb push mysendevent /data/local/tmp/`
fi

`adb push recorded_touch_events.txt /sdcard/`

# Replay the recorded events
`adb shell /data/local/tmp/mysendevent "${TOUCH_DEVICE#*-> }" /sdcard/recorded_touch_events.txt`
