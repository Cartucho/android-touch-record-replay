#!/bin/bash

# this script reads the info of each of the input devices
while read -r line; do
  OUTPUT=`adb shell getevent -lp "$line"`
  # The touchscreen device contains the keyword ABS_MT_TOUCH in its info
  if [[ "$OUTPUT" = *"ABS_MT_TOUCH"* ]]; then
    echo "Touchscreen device found! -> $line"
    exit
  fi
done < <(adb shell getevent -lp | egrep -o "(/dev/input/event\S+)") # this will find all the input devices
# (/dev/input/event\S+) searches for a string that starts with "/dev/input/event" and is followed by any non-whitespace characters

echo "Touchscreen not found!"
