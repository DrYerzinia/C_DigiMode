/******************************************************************************
 * File:  AFSK_Demodulator.h                                                  *
 * Author: Michael Marques                                                    *
 * Main file for decoding AFSK Modulated packets from data in a file or piped *
 * in from any source                                                         *
 *****************************************************************************/

#ifndef AFSK_DEMODULATOR_H
#define AFSK_DEMODULATOR_H

// malloc, free, int types, boolean's
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// memcpy for generating packet data to return
#include <string.h>

#include <ctype.h>

#include <math.h>

#include "APRSPacket.h"

#include "../math/trig_table.h"

#include "../container/char_array.h"

#include "char_array_expandable.h"

#include "char_ring_buffer.h"
#include "float_ring_buffer.h"

typedef struct {

	/* Buffer to hold incoming data until there is enough to fill the window
	 * and calculate Fourier Coefficients
	 */
	char_ring_buffer input_buffer;

	/* Buffer of Fourier Coefficients so we can average them and smooth out
	 * the signal for extracting bits from Zero-Crossings
	 */
	float_ring_buffer fcd_buffer;

	/* Sample rate of the incoming audio data in Samples per Second
	 * Do not adjust these values directly, instead call the appropriate
	 * functions on the decoder as other variables require re-calibration
	 * when these change
	 */
	float sample_rate;
	float bit_rate;

	float window;

	/* Frequencies of the 2 FSK symbols
	 * Standard VHF packet is 1200/2200 Hz
	 * Standard HF packet is
	 *  PK232 tones 1600/1800 Hz
	 *  KAM tones 2110/2310 Hz
	 */
	float frequency_0;
	float frequency_1;

	/* Goertzel Coefficients for calculating magnitude of frequencies
	 */
	float coeff0;
	float coeff1;

	/* A number from 0.0-1.0 to multiply by the average signal magnitude and
	 * then add to the signal to offset the signal so Zero-Crossings of the
	 * Fourier Coefficient data are more accurately spaced
	 * The Fourier Coefficients tend to be biased towards f0 TODO: more testing needed to confirm this statement
	 */
	float offset;

	/* Float value that will be multiplied be average Fourier signal magnitude
	 * to determine what samples should be considered erasure
	 * TODO: Testing has proved this unhelpful thus far, further testing needed
	 *  May remove this all together
	 */
	float noise_floor;

	/* Count of samples since last Zero-Crossing
	 */
	int count_last;

	/* Width of a bit calculated from Sample Rate and Bit Rate
	 * TODO: call reset in the functions that set Sample Rate and Bit Rate
	 */
	float bitwidth;

	/* A 30 character ring buffer for Bit Data
	 * Stores currently demodulated bits after Bit Stuffing removal and
	 * NRZI decoding
	 * The highest number of bits that should be collected before they are
	 * pushed to the byte array is ~16
	 * TODO: Determine actual necessary size and adjust
	 */
	char_ring_buffer bit_sequence;

	/* Max and Min of Fourier Coefficients calculated with what is essentially
	 * a peak detector with decay
	 * Used with offset parameter to adjust the center of the signal for
	 * determining Zero-Crossings of the Averaged Fourier signal
	 */
	int fcMax;
	int fcMin;

	/* Value of the last bit to be decoded
	 * used to check if there has been a Zero-Crossing
	 */
	int last_bit;

	/* Indicator if bit stuffing is occurring
	 * TODO: same as above
	 */
	bool bit_stuffing;

	/* Expandable array containing the sequence of bytes in the received packet
	 * It has a default size of 330 to contain a standard APRS packet without
	 * any reallocations
	 */
	char_array_expandable byte_sequence;

} AFSK_Demodulator;

/* Set up the demodulator with parameters for the type of signal to receive
 */
void AFSK_Demodulator_init(AFSK_Demodulator *self, float sr, float br, float off, float nf, int frequency_0, int frequency_1);

/* Deallocates memory used in buffers
 * Call this when you are done with decoder or you will have memory leaks
 */
void AFSK_Demodulator_destroy(AFSK_Demodulator *self);

/* Pass another byte of signal data to the demodulator
 * Returns a pointer to a char array containing a demodulated packet
 * if this byte did not complete demodulation of a packet it returns a
 * NULL pointer instead
 */
char_array* AFSK_Demodulator_proccess_byte(AFSK_Demodulator *self, signed char data_point);

/* Adjust the bit rate of the incoming data and the sample rate of the
 * audio signal that is being processed
 * These functions reset the window width so these parameters can not be
 * adjusted directly
 */
void AFSK_Demodulator_set_sample_rate(AFSK_Demodulator *self, float sr);
void AFSK_Demodulator_set_bit_rate(AFSK_Demodulator *self, float br);

/* Set the tone frequencies to listen for packets on
 */
void AFSK_Demodulator_set_frequency_0(AFSK_Demodulator *self, float f0);
void AFSK_Demodulator_set_frequency_1(AFSK_Demodulator *self, float f1);

/* Set demodulator tuning parameters of the demodulator
 * Adjusting the offset and noise floor can increase the number of packets
 * that are decoded
 */
void AFSK_Demodulator_set_offset(AFSK_Demodulator *self, float off);
void AFSK_Demodulator_set_noise_floor(AFSK_Demodulator *self, float nf);

#endif
