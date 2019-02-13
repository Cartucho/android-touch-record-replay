import subprocess
import datetime
import sys
import csv

#Input your touchscreen device name to the file
#cmdline usage: python human_readable_data.py /dev/input/event$ filepath

touchscreen = sys.argv[1]
outputpath = sys.argv[2]

headers = ["Timestamp", "Type", "Code", "Value"]

with open(outputpath, 'w') as writeFile:
    writer = csv.writer(writeFile)
    writer.writerow(headers)
    writeFile.close()

p = subprocess.Popen(['adb', 'exec-out', 'getevent', '-lt', touchscreen], stdout=subprocess.PIPE)

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

        with open(outputpath, 'a') as writeFile:
            writer = csv.writer(writeFile)
            writer.writerow([datetime.datetime.now(), event_type, event_code, event_value])
            writeFile.close()

        print("[ {}] {} {} {}".format(datetime.datetime.now(), event_type, event_code, event_value))
