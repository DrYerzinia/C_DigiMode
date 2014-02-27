/**
 * @file sound_device.cpp
 * @defgroup sound_device Audio Device Controllers
 */

#include <iostream>
#include <string>

#include "../util/Util.h"

#ifdef __CYGWIN32__
#define __OSS__
#else
#define __PULSE__
#endif

#ifdef __OSS__

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/soundcard.h>

#define LENGTH 60    /* how many seconds of speech to store */
#define RATE 44100   /* the sampling rate */
#define SIZE 8      /* sample size: 8 or 16 bits */
#define CHANNELS 1  /* 1 = mono 2 = stereo */

#endif

#ifdef __PULSE__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE 1024

/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;

    while (size > 0) {
        ssize_t r;

        if ((r = write(fd, data, size)) < 0)
            return r;

        if (r == 0)
            break;

        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }

    return ret;
}

#endif

unsigned char buf[1024];

int main(int argc, char *argv[]) {

	#ifdef __OSS__

	int fd;		// sound device file descriptor */
	int arg;	// argument for ioctl calls */
	int status;	// return status of system calls */

	/* open sound device */
	fd = open("/dev/dsp", O_RDWR);
	if (fd < 0) {
		perror("open of /dev/dsp failed");
		exit(1);
	}

	/* set sampling parameters */
	arg = SIZE;	   /* sample size */
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);
	if(status == -1)
		perror("SOUND_PCM_WRITE_BITS ioctl failed");
	if(arg != SIZE)
		perror("unable to set sample size");

	arg = CHANNELS;  /* mono or stereo */
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);

	if (status == -1)
		perror("SOUND_PCM_WRITE_CHANNELS ioctl failed");
	if(arg != CHANNELS)
		perror("unable to set number of channels");

	arg = RATE;	   /* sampling rate */
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if (status == -1)
		perror("SOUND_PCM_WRITE_WRITE ioctl failed");

	char wbuf[1];
	
	while (1) { /* loop until Control-C */
		status = read(fd, buf, 1024); /* record some sound */
		if (status != 1024)
			perror((std::string("read wrong number of bytes")+util::intToStr(status)).c_str());

		for(int i = 0; i < 1024; i++){
			wbuf[0] = buf[i];
			std::cout.write(wbuf, 1);
		}
		std::cout.flush();
	}
	#endif
	#ifdef __PULSE__
	    /* The sample type to use */
    //static const 
	pa_sample_spec ss;// = {
        ss.format = PA_SAMPLE_U8;//,
        ss.rate = 44100;//,
        ss.channels = 1;
    //};
    pa_simple *s = NULL;
    int ret = 1;
    int error;

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    for (;;) {
        uint8_t buf[BUFSIZE];

        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        /* And write it to STDOUT */
        if (loop_write(STDOUT_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
    }

    ret = 0;

finish:

    if (s)
        pa_simple_free(s);

    return ret;
	
	#endif
}
