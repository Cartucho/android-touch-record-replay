import subprocess
import datetime

p = subprocess.Popen(['./find_touchscreen_name.sh'], stdout=subprocess.PIPE)
out, err = p.communicate()
if "Touchscreen device found!" in out:
    device_str = out.split("-> ", 1)[1]
    #print(device_str)
    p = subprocess.Popen(['adb', 'shell', 'getevent', '-lt', device_str], stdout=subprocess.PIPE)
    while True:
        output = p.stdout.readline()
        if output == '' and p.poll() is not None:
            break
        if output:
            time, event_data = output.split("]", 1)
            event_type, event_code, event_value = event_data.split()
            try:
                # convert hexadecimal to int
                 event_value = int(event_value, 16)
            except ValueError:
                pass
            print("[ {}] {} {} {}".format(datetime.datetime.now(), event_type, event_code, event_value))
