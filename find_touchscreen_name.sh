#!/bin/bash

# this script reads the info of each of the input devices
#devs=`adb shell getevent -lp 2>/dev/null | egrep -o "(/dev/input/event\S+)"`
for line in `adb shell getevent -lp 2>/dev/null | egrep -o "(/dev/input/event\S+)"`; do
  echo $line
  output=`adb shell getevent -lp $line`
  # The touchscreen device contains the keyword ABS_MT in its info
  [[ "$output" == *"ABS_MT"* ]] && { echo "Touchscreen device found! -> $line"; exit; }
done
echo "Touchscreen not found!"
