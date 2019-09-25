#!/bin/bash

echo "Looking for touchscreen device..."
TOUCH_DEVICE=`./find_touchscreen_name.sh`
ANDROID_VERSION=`adb shell getprop ro.build.version.sdk`
MIN_VERSION=23

echo "$TOUCH_DEVICE"
echo "SDK version: $ANDROID_VERSION"

# Check if input device exists
if [[ "$TOUCH_DEVICE" = *"Touchscreen device found!"* ]]
then
    # Device found! Start recording
    echo "Recording will start as soon as you put your finger in the touchscreen."
    echo "Press ctrl+c to stop recording..."

    if (( $(echo $ANDROID_VERSION ">" $MIN_VERSION| tr -d $'\r' | bc -l) )); then
        #exec-out is shell without buffering, fixing missing last touch data event
        adb exec-out getevent -t "${TOUCH_DEVICE#*-> }" > recorded_touch_events.txt
    else
        # if Android version <= 6.0
        adb shell getevent -t "${TOUCH_DEVICE#*-> }" > recorded_touch_events.txt
    fi
fi
