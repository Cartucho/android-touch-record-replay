#!/bin/bash

echo "Looking for touchscreen device..."
TOUCH_DEVICE=`./find_touchscreen_name.sh`
echo "$TOUCH_DEVICE"

# detect "-emulator" argument for emulator
BINARY_NAME="mysendevent"
if [[ "$1" == "-emulator" ]]
then
   BINARY_NAME+="-x86"
fi

# push binary and touch events
adb push ${BINARY_NAME} /data/local/tmp/
adb push recorded_touch_events.txt /sdcard/

# Replay the recorded events
adb shell /data/local/tmp/"${BINARY_NAME}" "${TOUCH_DEVICE#*-> }" /sdcard/recorded_touch_events.txt