#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/input.h> // this does not compile
#include <unistd.h>
#include <errno.h>

// from <linux/input.h>

typedef uint32_t        __u32;
typedef uint16_t        __u16;
typedef __signed__ int  __s32;

struct input_event {
    struct timeval time;
    __u16 type;
    __u16 code;
    __u32 value;
};

#define MICROSEC 1000000

#define EVIOCGVERSION		_IOR('E', 0x01, int)			/* get driver version */
#define EVIOCGID		_IOR('E', 0x02, struct input_id)	/* get device ID */
#define EVIOCGKEYCODE		_IOR('E', 0x04, int[2])			/* get keycode */
#define EVIOCSKEYCODE		_IOW('E', 0x04, int[2])			/* set keycode */

#define EVIOCGNAME(len)		_IOC(_IOC_READ, 'E', 0x06, len)		/* get device name */
#define EVIOCGPHYS(len)		_IOC(_IOC_READ, 'E', 0x07, len)		/* get physical location */
#define EVIOCGUNIQ(len)		_IOC(_IOC_READ, 'E', 0x08, len)		/* get unique identifier */

#define EVIOCGKEY(len)		_IOC(_IOC_READ, 'E', 0x18, len)		/* get global keystate */
#define EVIOCGLED(len)		_IOC(_IOC_READ, 'E', 0x19, len)		/* get all LEDs */
#define EVIOCGSND(len)		_IOC(_IOC_READ, 'E', 0x1a, len)		/* get all sounds status */
#define EVIOCGSW(len)		_IOC(_IOC_READ, 'E', 0x1b, len)		/* get all switch states */

#define EVIOCGBIT(ev,len)	_IOC(_IOC_READ, 'E', 0x20 + ev, len)	/* get event bits */
#define EVIOCGABS(abs)		_IOR('E', 0x40 + abs, struct input_absinfo)		/* get abs value/limits */
#define EVIOCSABS(abs)		_IOW('E', 0xc0 + abs, struct input_absinfo)		/* set abs value/limits */

#define EVIOCSFF		_IOC(_IOC_WRITE, 'E', 0x80, sizeof(struct ff_effect))	/* send a force effect to a force feedback device */
#define EVIOCRMFF		_IOW('E', 0x81, int)			/* Erase a force effect */
#define EVIOCGEFFECTS		_IOR('E', 0x84, int)			/* Report number of effects playable at the same time */

#define EVIOCGRAB		_IOW('E', 0x90, int)			/* Grab/Release device */

// end <linux/input.h>

void remove_specific_chars(char* str, char c1, char c2) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c1 && *pw != c2);
    }
    *pw = '\0';
}

// ENUM to separate between different operation modes
typedef enum e_Mode {MODE_UNDEFINED, MODE_RELATIVE, MODE_ABSOLUTE} Mode;

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int ret;
    int version;
    Mode mode=MODE_ABSOLUTE;
    struct input_event event;


	int opt;
	int verbose=0;

	while ((opt = getopt(argc, argv, "vr")) != -1) {
		switch (opt) {
		case 'v':
		   verbose++;
		   break;
		case 'r':
		   mode = MODE_RELATIVE;
		   break;
		default: /* '?' */
		   fprintf(stderr, "Usage: %s [-r] [-v] input_device input_events\n",
			   argv[0]);
		   exit(EXIT_FAILURE);
		}
	}

	if (verbose) {
		printf("verbose=%d; mode=%d; optind=%d\n",
		   verbose, mode, optind);
	}	

	if (optind != argc-2) {
		fprintf(stderr, "Expected two argument after options\n");
		exit(EXIT_FAILURE);
	}

    fd = open(argv[optind], O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }
    if (ioctl(fd, EVIOCGVERSION, &version)) {
        fprintf(stderr, "could not get driver version for %s, %s\n", argv[optind], strerror(errno));
        return 1;
    }

    FILE * fd_in;
    if (strcmp(argv[optind+1], "-")==0) {
	fd_in = stdin;
    } else {
	fd_in = fopen(argv[optind+1], "r");
    }
    if (fd_in == NULL) {
        fprintf(stderr, "could not open input file: %s\n", argv[optind+1]);
        return 1;
    }


    char line[128];
    unsigned int sleep_time;
    double timestamp_previous = -1.0;
    double timestamp_now;
    char type[32];
    char code[32];
    char value[32];

    while (fgets(line, sizeof(line), fd_in) != NULL) {

	// look for special escape character        
	if (line[0]=='#') {
		fprintf(stdout, "%s", line);
		fflush(stdout);
		continue;
		fprintf(stdout, "this is not written");
	}

	// remove the characters [ and ] surrounding the timestamp
        remove_specific_chars(line, '[', ']');
        sscanf(line, "%lf %s %s %s", &timestamp_now, type, code, value);

	// if required sleep before issuing the event
	if (mode==MODE_ABSOLUTE) {
		if(timestamp_previous != -1.0)
		{
		  // In order to playback the same gestures the code sleeps accordingly to the timestamps from the inputed recording
		  sleep_time = (unsigned int) ((timestamp_now - timestamp_previous) * MICROSEC);

		  // we don't care about the value of a single event's timestamp but the difference between two sequential events
		  usleep(sleep_time); // sleep_time is in MICROSECONDS
		}

		timestamp_previous = timestamp_now;
	} else if (mode==MODE_RELATIVE && timestamp_now > 0.0) {
		// In MODE_RELATIVE the timestamp just contains the time to sleep before issuing the next command
		sleep_time = (unsigned int) (timestamp_now * MICROSEC);
		usleep(sleep_time);
		if (verbose>0) {
                	fprintf(stdout, "!sleeping for %d (%lf)\n%s", sleep_time, timestamp_now, line);
                	fflush(stdout);
		}
	}

        // write the event to the appropriate input device
        memset(&event, 0, sizeof(event));
        event.type = (int) strtol(type, NULL, 16);
        event.code = (int) strtol(code, NULL, 16);
        event.value = (uint32_t) strtoll(value, NULL, 16);
        ret = write(fd, &event, sizeof(event));
        if(ret < sizeof(event)) {
            fprintf(stderr, "write event failed, %s\n", strerror(errno));
            return -1;
        }

        // Clear temporary buffers
        memset(line, 0, sizeof(line));
        memset(type, 0, sizeof(type));
        memset(code, 0, sizeof(code));
        memset(value, 0, sizeof(value));
    }

    fclose(fd_in);
    close(fd);

    return 0;
}
