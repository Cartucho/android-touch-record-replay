# Record and Replay Touchscreen Events on Android

[![GitHub stars](https://img.shields.io/github/stars/Cartucho/android-touch-record-replay.svg?style=social&label=Stars)](https://github.com/Cartucho/android-touch-record-replay)

How to record and replay touchscreen events on an Android device.

<p align="center">
  <img src="https://user-images.githubusercontent.com/15831541/44855448-f4e1a080-ac62-11e8-8b7d-4e48cc80a269.gif"/>
</p>


## Table of contents

- [Prerequisites](#prerequisites)
- [Quick start](#quick-start)
- [Easy method](#easy-method)
- [Step-by-step method and Explanation](#step-by-step-method-and-explanation)
- [Authors](#authors)

## Prerequisites

You need to:

- Install ADB in your computer (check if it is already installed by running in a terminal the following command: `adb devices`)
- Enable `USB Debugging` in your Android device (`Settings > Developer Options > USB Debugging`)

Now, connect your device to your computer, via a USB cable, and run the following bash script in a terminal:

    ./find_touchscreen_name.sh

, if this works, aka you get the output message `"Touchscreen device found!"`, you can use the [Easy method](#easy-method) otherwise you need to use the [Step-by-step method](#step-by-step-method-and-explanation).

Optional:

- It is easier for you to visualize things by enabling `Show touches` or `Pointer location` (`Settings > Developer Options > Input > Show touches`)


## Quick start
To start using the mAP you need to clone the repo:

```
git clone https://github.com/Cartucho/android-touch-record-replay
```

## Easy method

1. Record touch events with timestamps:

    `./record_touch_events.sh`

2. Replay those touch event:

    `./replay_touch_events.sh`

## Step-by-step method and Explanation

Android uses the Linux kernel so it processes input events the same way as any other Linux system.
With the android's [getevent](https://source.android.com/devices/input/getevent) tool we can record the live dump of kernel input events and use that recording to replay the same touch events.

1. Connect your phone via a USB cable to your computer.
2. Find out the name of the touchscreen's device (should look something like `/dev/input/event<your_index>`). There a couple of ways of doing this:
    - Use the command `adb shell getevent -lp` and figure out which of the input devices contains events with the `ABS_MT_TOUCH_MAJOR` textual label;
    - Another option is to try the command `adb shell getevent | grep event<your_index>` with the different possible values for `<your_index>` until you see that the terminal is printing events exactly when you move your finger over the phone's touchscreen.

In my case, my touchscreen's name is `/dev/input/event7` which I will be using from now forward. Don't forget to change this in the following step's commands!

3. Using the touchscreen's device name we can now record events using the command `adb shell getevent -t /dev/input/event7 > recorded_touch_events.txt`. Using the `-t` option we also record the timestamps (so that later we can replay the recorded input with the proper delay between events). The content on this file should look something like:

        ...
        [   53890.813990] 0000 0000 00000000
        [   53890.828065] 0003 0035 000001be
        [   53890.828065] 0003 0036 00000258
        [   53890.828065] 0003 0030 0000001f
        [   53890.828065] 0003 0032 0000001f
        [   53890.828065] 0003 0039 00000000
        ...

    , where each line represents an event's `[   timestamp] type code value`.

4. We now push those recorded events into the phone using the command `adb push recorded_touch_events.txt /sdcard/`

5. In order to replay those events we will use the `mysendevent` executable, which will read the `recorded_touch_events.txt` file line by line and inject those events in the kernel input. To do this we need to push this executable to the phone by using the following command `adb push mysendevent /data/local/tmp/`

    Note: In case you want to modify the file `mysendevent.c` you have to recompile it (for android, e.g., use `arm-linux-gnueabi-gcc -static -march=armv7-a mysendevent.c -o mysendevent`) and re-push `mysendevent` to the phone.

6. Finally, we can replay the previous recording by running the command `adb shell /data/local/tmp/mysendevent /dev/input/event7 /sdcard/recorded_touch_events.txt`

## Authors:
* **João Cartucho**

    Feel free to contribute

    [![GitHub contributors](https://img.shields.io/github/contributors/Cartucho/android-touch-record-replay.svg)](https://github.com/Cartucho/android-touch-record-replay/graphs/contributors)
