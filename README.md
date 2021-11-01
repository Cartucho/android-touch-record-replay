# Record and Replay Touchscreen Events on Android

How to record and replay touchscreen events on an Android device and emulator.

<p align="center">
  <img src="https://user-images.githubusercontent.com/15831541/44855448-f4e1a080-ac62-11e8-8b7d-4e48cc80a269.gif"/>
</p>


## Fork
This fork from [android-touch-record-replay](https://github.com/Cartucho/android-touch-record-replay) adds support for :

- Replaying touches on Android emulators
- Specify custom filename when recording/replaying


## Table of contents

- [Prerequisites](#prerequisites)
- [Quick start](#quick-start)
- [Easy method](#easy-method)
- [Step-by-step method and Explanation](#step-by-step-method-and-explanation)
- [Outputting Data](#output-data-to-csv)
- [Authors](#authors)

## Prerequisites

You need to:

- Install ADB in your computer (`sudo apt install adb`)
- Enable `USB Debugging` in your Android device (`Settings > Developer Options > USB Debugging`)
- Use an Android API <= 28

Now, connect your device to your computer via a USB cable (or run your emulator), and run the following bash script in a terminal:

    ./find_touchscreen_name.sh

if you get the output message `"Touchscreen device found!"`, then you can use the [Easy method](#easy-method). Otherwise, you need to use the [Step-by-step method](#step-by-step-method-and-explanation).

Optional:

- It is easier for you to visualize where you touched the screen by turning one of the following (1) `Show touches` or (2) `Pointer location` (In your phone go to `Settings > Developer Options > Input > Show touches`)

## Quick start

To start using the scripts you need to clone the repo:

```
git clone https://github.com/Nutriz/android-touch-record-replay
```

## Easy method

1. Record touch events:

```bash
# use default filename for records
./record_touch_events.sh

# use specific filename
./record_touch_events.sh my_records.txt
```

2. Replay those touch event:

```bash
# replay on real device with default filename
./replay_touch_events.sh -realdevice

# replay on emulator with default filename
./replay_touch_events.sh -emulator

# replay on emulator with specific filename
./replay_touch_events.sh -emulator my_records.txt

```

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

## Outputting Touch Data

You may want to export and anyalse the touch screen data.

As mentioned in the previous section the touch data is in the following format

| Timestamp         | Type | Code | Value    |
| ----------------- | ---- | ---- | -------- |
| [   53890.813990] | 0000 | 0000 | 000005b0 |
| [   53890.828065] | 0003 | 0035 | 000001be |
| [   53890.828065] | 0003 | 0036 | 00000258 |
| [   53890.828065] | 0003 | 0030 | 0000001f |
| [   53890.828065] | 0003 | 0032 | 0000001f |
| [   53890.828065] | 0003 | 0039 | 00000000 |

### Timestamp

According to the [android documentation](https://source.android.com/devices/input/getevent) the timestamp data is in the following format:

> getevent timestamps use the format $SECONDS.$MICROSECONDS in the CLOCK_MONOTONIC timebase.

[CLOCK_REALTIME vs. CLOCK_MONOTONIC](https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic)

This dataformat is not ideal as it is relative to an arbitrary time in the system.

We will not use this data in our data collection script.

### Type & Code

Currently we have output these as their codes, we can use the `-l` argument to output these codes as descriptive labels.

For example:

`adb shell getevent -lt /dev/input/event7 > recorded_touch_events.txt`

would now output:

| Timestamp         | Type   | Code               | Value    |
| ----------------- | ------ | ------------------ | -------- |
| [   46112.124651] | EV_ABS | ABS_MT_TRACKING_ID | 000005b0 |
| [   46112.124651] | EV_ABS | ABS_MT_POSITION_X  | 0000010f |
| [   46112.124651] | EV_ABS | ABS_MT_POSITION_Y  | 00000479 |
| [   46112.124651] | EV_ABS | ABS_MT_PRESSURE    | 0000005f |
| [   46112.124651] | EV_ABS | ABS_MT_TOUCH_MAJOR | 0000005f |
| [   46112.124651] | EV_SYN | SYN_REPORT         | 00000000 |
| [   46112.173948] | EV_SYN | 0004               | 0000b420 |
| [   46112.173948] | EV_SYN | 0005               | 0a5db877 |

### Value

The value is currently being output as hexadecimal. We will convert the value into an integer when we collect the data in our script later.

The following command will display the `min` and `max` values for each input:

`adb shell getevent -lp /dev/input/event7`

```
  name:     "Melfas MMSxxx Touchscreen"
  events:
    ABS (0003): ABS_MT_SLOT           : value 0, min 0, max 9, fuzz 0, flat 0, resolution 0
                ABS_MT_TOUCH_MAJOR    : value 0, min 0, max 30, fuzz 0, flat 0, resolution 0
                ABS_MT_POSITION_X     : value 0, min 0, max 720, fuzz 0, flat 0, resolution 0
                ABS_MT_POSITION_Y     : value 0, min 0, max 1280, fuzz 0, flat 0, resolution 0
                ABS_MT_TRACKING_ID    : value 0, min 0, max 65535, fuzz 0, flat 0, resolution 0
                ABS_MT_PRESSURE       : value 0, min 0, max 255, fuzz 0, flat 0, resolution 0
  input props:
    INPUT_PROP_DIRECT
```

### Output data to CSV

The `human_readable_data.py` script will convert all of the touch data into a more readable format and output it to a csv file.

**Usage:** `python human_readable_data.py arg1 arg2`

| Argument | Value               | Example                             |
| -------- | ------------------- | ----------------------------------- |
| arg1     | touchscreen device  | /dev/input/event7                   |
| arg2     | csv output location | /Users/$user/Desktop/touch_info.csv |

This will create a csv file containing the following information:

| **Timestamp**                  | **Type** | **Code**           | **Value** |
| ------------------------------ | -------- | ------------------ | --------- |
| **2019-02-13 10:30:41.558271** | EV_ABS   | ABS_MT_TRACKING_ID | 1804      |
| **2019-02-13 10:30:41.558557** | EV_ABS   | ABS_MT_POSITION_X  | 805       |
| **2019-02-13 10:30:41.558797** | EV_ABS   | ABS_MT_POSITION_Y  | 848       |
| **2019-02-13 10:30:41.559035** | EV_ABS   | ABS_MT_PRESSURE    | 51        |
| **2019-02-13 10:30:41.559245** | EV_ABS   | ABS_MT_TOUCH_MAJOR | 51        |
| **2019-02-13 10:30:41.559453** | EV_SYN   | SYN_REPORT         | 0         |
| **2019-02-13 10:30:41.697711** | EV_SYN   | 4                  | 66179     |
| **2019-02-13 10:30:41.701748** | EV_SYN   | 5                  | 233329954 |

## Authors:

* **João Cartucho**
* **Jérôme Gully**