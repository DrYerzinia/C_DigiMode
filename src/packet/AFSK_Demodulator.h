// AFSK_Demodulator.h

#ifndef AFSK_DEMODULATOR_H
#define AFSK_DEMODULATOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>

#include "APRSPacket.h"

#include "../math/trig_table.h"

#include "../container/char_array.h"

#include "char_array_expandable.h"

#include "char_ring_buffer.h"
#include "float_ring_buffer.h"

unsigned short CRCCCITT(char_array *data);

typedef struct {

	char_ring_buffer input_buffer;
	float_ring_buffer fcd_buffer;
	float_ring_buffer fcavg_buffer;	// float ring buffer with max size of 10 bit times where it starts overwriting old data

	float sample_rate;
	float bit_rate;

	float window;

	float frequency_0;
	float frequency_1;

	float offset;
	float hysteresis;
	float noise_floor;

	int sample_counter;

	float bitwidth;

	// 30 char ring buffer for bit data
	char_ring_buffer bit_sequence;

	int fcMax;
	int fcMin;

	int last;
	int max;
	int countlast;

	int last_bit;

	int same_count;
	bool bit_stuffing;

	bool freq_sync_found;
	bool packet_start;

	// default 330 length expandable array, expands by 50 bytes each time 330 is length of a APRS packet exactly
	char_array_expandable byte_sequence;

} AFSK_Demodulator;

void AFSK_Demodulator_init(AFSK_Demodulator *self, float sr, float br, float off, float hys, float nf);
void AFSK_Demodulator_destroy(AFSK_Demodulator *self);

char_array* AFSK_Demodulator_proccess_byte(AFSK_Demodulator *self, char data_point);

void AFSK_Demodulator_set_sample_rate(AFSK_Demodulator *self, float sr);
void AFSK_Demodulator_set_bit_rate(AFSK_Demodulator *self, float br);

void AFSK_Demodulator_set_frequency_0(AFSK_Demodulator *self, float f0);
void AFSK_Demodulator_set_frequency_1(AFSK_Demodulator *self, float f1);

void AFSK_Demodulator_set_offset(AFSK_Demodulator *self, float off);
void AFSK_Demodulator_set_hysteresis(AFSK_Demodulator *self, float hys);
void AFSK_Demodulator_set_noise_floor(AFSK_Demodulator *self, float nf);

void AFSK_Demodulator_start_only(AFSK_Demodulator *self);

#endif
