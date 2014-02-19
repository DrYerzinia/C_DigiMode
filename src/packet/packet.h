// packet.h

#pragma once

#include <list>
#include <vector>
#include <string>

#include <cmath>

//////////////////

#include <stdlib.h>

#include <string.h>

#include "APRSPacket.h"

#ifndef PI
#define PI 3.14159265
#endif

unsigned short CRCCCITT(char_array &data);

class packet {

public:

	packet();
	packet(float sr, float br);
	packet(float sr, float br, float off, float hys, float nf);
	packet(float sr, float br, float f0, float f1);
	packet(float sr, float br, float off, float hys, float nf, float f0, float f1);

	~packet();

	void init();
	void reset();

	char_array* proccess_byte(char data_point);

	void set_sample_rate(float sr);
	void set_bit_rate(float br);

	void set_frequency_0(float f0);
	void set_frequency_1(float f1);

	void set_offset(float off);
	void set_hysteresis(float hys);
	void set_noise_floor(float nf);

private:

	void start_only();

	std::list<char> input_buffer;
	std::list<float> fcd_buffer;
	std::list<float> fcavg_buffer;

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

	std::vector<int> bit_sequence;

	int fcMax;
	int fcMin;

	int last;
	int max;
	int countlast;
	int different_erasure_count;

	int last_bit;

	int same_count;
	int master_count;
	bool bit_stuffing;

	bool freq_sync_found;
	bool packet_start;

	std::vector<char> byte_sequence;

};
