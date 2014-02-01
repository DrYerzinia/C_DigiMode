// psk31.h

#pragma once

#include <map>
#include <list>
#include <vector>

#include <cmath>

#ifndef PI
#define PI 3.14159265
#endif

#ifndef NULL
#define NULL 0
#endif

class psk31 {

public:

	psk31();
	psk31(float sample_rate, float bit_rate, float psk31freq);

	virtual ~psk31();

	/* Returns True if a new byte has been decoded
	*/
	virtual bool proccess_byte(char next_data_point) = 0;

	void set_sample_rate(float sample_rate);
	void set_bit_rate(float bit_rate);
	void set_psk31freq(float psk31freq);

	void set_threshhold_value(float threshhold_value);

	char getLastByte();

	std::vector<char> getDecodedData();

protected:

	char getVaricode(int code);
	void initVaricode();

	virtual void init() = 0;

	float sample_rate;
	float bit_rate;
	float psk31freq;

	float threshhold_value;

	float ninty_deg_phase_width;

	int bitwidth;

	std::map<int, char>	varicode;

	unsigned int input_position;
	unsigned int input_count;
	unsigned int input_data_width;
	char* input_data;

	std::list<float> phase_change_data;

	std::vector<int> bit_sequence;

	int last;
	int max;
	int countlast;

	std::vector<char> byte_sequence;

};

