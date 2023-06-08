CC=$(HOME)/Library/Android/sdk/ndk/25.1.8937393/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi21-clang
CCOPTS=--static -ffunction-sections -fdata-sections -Wl,--gc-sections -march=armv7-a
ADB="/Applications/Genymotion.app/Contents/MacOS/tools/adb"
TARGET=/tmp

mysendevent: mysendevent.c
	$(CC) $(CCOPTS) mysendevent.c -o mysendevent

install: mysendevent
	$(ADB) push mysendevent "$(TARGET)" 
	$(ADB) shell "chmod +x '$(TARGET)/mysendevent'"

test: install
	$(ADB) shell "$(TARGET)/mysendevent"

fifo: 
	mkfifo -m 644 fifo

connect: install fifo
	tail -f fifo | $(ADB) shell /tmp/mysendevent -r -v /dev/input/event4 -
