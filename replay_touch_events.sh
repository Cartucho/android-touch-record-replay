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

# record into specific touch file
TOUCH_FILE_NAME="recorded_touch_events.txt"
if [ ! -z "$2" ]
then
   TOUCH_FILE_NAME="$2"
fi

# push binary and touch events
adb push ${BINARY_NAME} /data/local/tmp/
adb push $TOUCH_FILE_NAME /sdcard/

# Replay the recorded events
adb shell /data/local/tmp/"${BINARY_NAME}" "${TOUCH_DEVICE#*-> }" /sdcard/"${TOUCH_FILE_NAME}"