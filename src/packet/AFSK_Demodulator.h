/**
 * @file	AFSK_Demodulator.h
 * @author	Michael Marques <dryerzinia@gmail.com>
 * @brief	Demodulator for AFSK modulated AX.25 Frames.
 * @ingroup Packet
 *
 * This file contains a struct AFSK_Demodulator for holding all the data
 * necessary for the AFSK_Demodulator process byte function which takes in 8
 * bit signed samples from an audio signal.
 */

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

#include "../container/char_array.h"

#include "char_array_expandable.h"

#include "char_ring_buffer.h"
#include "float_ring_buffer.h"

#ifndef PI
#define PI 3.14159265359
#endif

/**
 * @ingroup Packet
 * Contains all information necessary to demodulate AFSK modulated AX-25 frames
 */
typedef struct {

	/**
	 * Buffer to hold incoming data until there is enough to fill the window
	 * and calculate Fourier Coefficients
	 */
	char_ring_buffer input_buffer;

	/**
	 * Buffer of Fourier Coefficients so we can average them and smooth out
	 * the signal for extracting bits from Zero-Crossings
	 */
	float_ring_buffer fcd_buffer;

	/**
	 * Sample rate of the incoming audio data in Samples per Second
	 * Do not adjust this value directly, instead call the appropriate
	 * functions on the decoder as other variables require re-calibration
	 * when these change
	 */
	uint32_t sample_rate;
	/**
	 * Bit rate of the encoded digital information in the audio signal
	 * Do not adjust this value directly, instead call the appropriate
	 * functions on the decoder as other variables require re-calibration
	 * when these change
	 */
	uint16_t bit_rate;

	/**
	 * Width of the window of samples to run Goertzels algorithm on and to
	 * average Fourier coefficients across
	 */
	uint8_t window;

	/**
	 * Frequency of the 1st AFSK Symbol
	 * Standard VHF packet is 1200/2200 Hz
	 * Standard HF packet is
	 *  PK232 tones 1600/1800 Hz
	 *  KAM tones 2110/2310 Hz
	 */
	uint16_t frequency_0;
	/**
	 * Frequency of the 2nd AFSK Symbol
	 */
	uint16_t frequency_1;

	/**
	 * Goertzel Coefficient for calculating magnitude of frequency 0
	 */
	float coeff0;
	/**
	 * Goertzel Coefficient for calculating magnitude of frequency 1
	 */
	float coeff1;

	/**
	 * A number from 0.0-1.0 to multiply by the average signal magnitude and
	 * then add to the signal to offset the signal so Zero-Crossings of the
	 * Fourier Coefficient data are more accurately spaced
	 * The Fourier Coefficients tend to be biased towards f0 TODO: more testing needed to confirm this statement
	 */
	float offset;

	/**
	 * Float value that will be multiplied be average Fourier signal magnitude
	 * to determine what samples should be considered erasure
	 * TODO: Testing has proved this unhelpful thus far, further testing needed
	 *  May remove this all together
	 */
	float noise_floor;

	/**
	 * Count of samples since last Zero-Crossing
	 */
	uint16_t count_last;

	/**
	 * Width of a bit calculated from Sample Rate and Bit Rate
	 * TODO: call reset in the functions that set Sample Rate and Bit Rate
	 */
	uint8_t bitwidth;

	/**
	 * A 14 character ring buffer for Bit Data
	 * Stores currently demodulated bits after Bit Stuffing removal and
	 * NRZI decoding
	 * The highest number of bits that should be collected before they are
	 * pushed to the byte array is 12, 8 for a byte + 6 from bit stuffing
	 */
	char_ring_buffer bit_sequence;

	/**
	 * Max of Fourier Coefficients calculated with what is essentially
	 * a peak detector with decay
	 * Used with offset parameter to adjust the center of the signal for
	 * determining Zero-Crossings of the Averaged Fourier signal
	 */
	int32_t fcMax;
	/**
	 * Min of Fourier Coefficients
	 */
	int32_t fcMin;

	/**
	 * Value of the last bit to be decoded
	 * used to check if there has been a Zero-Crossing
	 */
	uint8_t last_bit;

	/**
	 * Indicator if bit stuffing is occurring
	 * TODO: same as above
	 */
	bool bit_stuffing;

	/**
	 * Expandable array containing the sequence of bytes in the received packet
	 * It has a default size of 330 to contain a standard APRS packet without
	 * any reallocations
	 */
	char_array_expandable byte_sequence;

} AFSK_Demodulator;

/**
 * @ingroup Packet
 * Set up the demodulator with parameters for the type of signal to receive
 */
void AFSK_Demodulator_init(AFSK_Demodulator *self, uint32_t sr, uint16_t br, float off, float nf, uint16_t frequency_0, uint16_t frequency_1);

/**
 * @ingroup Packet
 * Deallocates memory used in buffers
 * Call this when you are done with decoder or you will have memory leaks
 */
void AFSK_Demodulator_destroy(AFSK_Demodulator *self);

/**
 * @ingroup Packet
 * Pass another byte of signal data to the demodulator
 * Returns a pointer to a char array containing a demodulated packet
 * if this byte did not complete demodulation of a packet it returns a
 * NULL pointer instead
 */
char_array* AFSK_Demodulator_proccess_byte(AFSK_Demodulator *self, int8_t data_point);

/**
 * @ingroup Packet
 * Adjust the sample rate of the audio signal that is being processed
 * This function resets the window width so these parameters cannot be
 * adjusted directly
 */
void AFSK_Demodulator_set_sample_rate(AFSK_Demodulator *self, uint32_t sr);
/**
 * @ingroup Packet
 * Adjust the bit rate of the incoming data
 * This function resets the window width so these parameters cannot be
 * adjusted directly
 */
void AFSK_Demodulator_set_bit_rate(AFSK_Demodulator *self, uint16_t br);

/**
 * @ingroup Packet
 * Sets frequency of the 1st tone data will be modulated on
 */
void AFSK_Demodulator_set_frequency_0(AFSK_Demodulator *self, uint16_t f0);
/**
 * @ingroup Packet
 * Sets frequency of the 2nd tone data will be modulated on
 */
void AFSK_Demodulator_set_frequency_1(AFSK_Demodulator *self, uint16_t f1);

/**
 * @ingroup Packet
 * Sets the offset of the demodulator
 * This will multiply by the average signal magnitude and be added to the
 * Fourier coefficient delta to better center the signal
 */
void AFSK_Demodulator_set_offset(AFSK_Demodulator *self, float off);
/**
 * @ingroup Packet
 * Sets the Noise Floor
 * Fourier coefficients deltas that are below this value multiplied by
 * the average signal magnitude will be ignored as they pertain to
 * detecting Zero-Crossings
 */
void AFSK_Demodulator_set_noise_floor(AFSK_Demodulator *self, float nf);

#endif
