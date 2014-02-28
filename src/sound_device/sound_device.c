/**
 * @file sound_device.cpp
 * @defgroup sound_device Audio Device Controllers
 */

#include <unistd.h>
#include <stdio.h>

#include <stdbool.h>

#include <errno.h>

#ifdef __OSS__

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/soundcard.h>

#define LENGTH 60    /* how many seconds of speech to store */
#define RATE 44100   /* the sampling rate */
#define SIZE 8      /* sample size: 8 or 16 bits */
#define CHANNELS 1  /* 1 = mono 2 = stereo */

#elif defined __PULSE__

#include <string.h>

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

#elif defined __ALSA__

#include <alsa/asoundlib.h>

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
			return -1;
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

		while (1) { /* loop until Control-C */
			status = read(fd, buf, 1024); /* record some sound */
			if (status != 1024)
				fprintf(stderr, "read wrong number of bytes %d\n", status);
	
			int i;
			for(i = 0; i < 1024; i++)
				fputc(buf[i], stdout);

			fflush(stdout);
		}

	#elif defined __PULSE__

		pa_sample_spec ss;
		ss.format = PA_SAMPLE_U8;
		ss.rate = 44100;
		ss.channels = 1;

		pa_simple *s = NULL;
		int ret = 1;
		int error;

		/* Create the recording stream */
		if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
			fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
			if(s)
				pa_simple_free(s);
			return -1;
		}

		while(true) {
			uint8_t buf[BUFSIZE];

			/* Record some data ... */
			if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
				fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
				if(s)
					pa_simple_free(s);
				return -1;
			}

			/* And write it to STDOUT */
			if (loop_write(STDOUT_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
				fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
				if(s)
					pa_simple_free(s);
				return -1;
			}
		}

		if (s)
			pa_simple_free(s);

	#elif defined __ALSA__

		int err;
		char *buffer;
		int buffer_frames = 128;
		unsigned int rate = 11025;
		snd_pcm_t *capture_handle;
		snd_pcm_hw_params_t *hw_params;
		snd_pcm_format_t format = SND_PCM_FORMAT_S8;

		if((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf(stderr, "cannot open audio device %s (%s)\n", argv[1], snd_strerror(err));
		    return -1;
		}

		fprintf(stderr, "audio interface opened\n");

		if((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
			fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params allocated\n");

		if((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
			fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
		    return -1;
		}

		fprintf(stderr, "hw_params initialized\n");

		if((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
			fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params access set\n");

		if((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
			fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params format set\n");

		if((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0)) < 0) {
			fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params rate set\n");

		if((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2)) < 0) {
			fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params channels set\n");

		if((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
			fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
			return -1;
		}

		fprintf(stderr, "hw_params set\n");

		snd_pcm_hw_params_free (hw_params);

		fprintf(stderr, "hw_params freed\n");

		if((err = snd_pcm_prepare(capture_handle)) < 0) {
			fprintf(stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror(err));
			exit (1);
		}

		fprintf(stderr, "audio interface prepared\n");

		buffer = malloc(128 * snd_pcm_format_width(format) / 8 * 2);

		fprintf(stdout, "buffer allocated\n");

		while(true){
			if((err = snd_pcm_readi (capture_handle, buffer, buffer_frames)) != buffer_frames){
				fprintf (stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
				return -1;
			}
			int i;
			for(i = 0; i < 128 * snd_pcm_format_width(format) / 8; i++)
				fputc(buffer[i*2], stdout);
			fflush(stdout);
		}

		free(buffer);

		fprintf(stderr, "buffer freed\n");

		snd_pcm_close(capture_handle);
		fprintf(stderr, "audio interface closed\n");

	#else

		#error "No Sound Library selected!"

    #endif

	return 0;

}
