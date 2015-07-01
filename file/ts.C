#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define VALUE_TYPE_TEMP_CENTIGRADE 1
#define VALUE_TYPE_RELATIVE_HUMIDITY_PERCENT 2

typedef struct {
  int value;
  int value_type;
  intmax_t time;
  int sensor_id;  
} Measurement;

typedef struct {
	intmax_t start_value;
	off_t approx_offset;
} Index_node;

#ifdef REAL_TIME
intmax_t get_current_time_in_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
clock_serv_t cclock;
mach_timespec_t mts;
host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
clock_get_time(cclock, &mts);
mach_port_deallocate(mach_task_self(), cclock);
spec.tv_sec = mts.tv_sec;
spec.tv_nsec = mts.tv_nsec;

#else
clock_gettime(CLOCK_MONOTONIC, &spec);
#endif

    s  = spec.tv_sec*1000 + round(spec.tv_nsec / 1.0e6);
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return (intmax_t)s;
}
#else
intmax_t get_current_time_in_ms (void)
{
	// fake implementation for performance tests
	static int t = 0;
	return t++;
}

#endif

#define BUFSIZE 1000

Measurement buffer[BUFSIZE];

int buf_idx = 0;

int fd;

int fd_seconds;

int current_second;

void push_measurement(Measurement m) {
	Index_node node;
	
	if (buf_idx == BUFSIZE) {
		// flush buffer
		if (write(fd,&buffer[0],BUFSIZE*sizeof(Measurement)) <= 0)
			printf("Fehler !!");
		
		if (buffer[BUFSIZE-1].time/1000 != current_second) {
			// record the approximate position in the file, NOT THE EXACT ONE (why should we ...)
			node.approx_offset = lseek( fd, 0, SEEK_CUR );
			node.start_value = current_second*1000;
			//printf("Index of second %ld is %ld\n",node.start_value,(intmax_t)node.approx_offset);
			if (write(fd_seconds,&node,sizeof(Index_node)) <= 0)
				printf("Fehler !!");
			current_second = buffer[BUFSIZE-1].time/1000; // HACK HACK assumes that max one second per buf
		}
		buf_idx = 0;		
	}
	buffer[buf_idx++] = m;		
}

void flush_buffer() {
	if (write(fd,&buffer[0],buf_idx*sizeof(Measurement)) <= 0)
		printf("Fehler !!");
	buf_idx = 0;			
}

int main() {
	int iValues = 100000;
	int iSensors = 100;
	int i,j;
	intmax_t t;
	Measurement m;
	current_second = get_current_time_in_ms() / 1000;
	
	//printf("%ld\n", get_current_time_in_ms());
  	fd = open("data.bin",O_RDWR | O_APPEND | O_CREAT,S_IRWXU);
  	fd_seconds = open("data_seconds.bin",O_RDWR | O_APPEND | O_CREAT,S_IRWXU);
  	for(i=0;i<iValues;i++) {
  		t = get_current_time_in_ms();
		for(j=0;j<iSensors;j++) {
			m.value = i+j;
			m.value_type = VALUE_TYPE_TEMP_CENTIGRADE;
			m.time = t;
			m.sensor_id = j;
			push_measurement(m);
		}
  	}
	flush_buffer();
	fsync(fd);
	close(fd);
	fsync(fd_seconds);
	close(fd_seconds);
	//printf("%ld\n", get_current_time_in_ms());
}