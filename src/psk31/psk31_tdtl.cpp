// psk31.cpp

#include "psk31_tdtl.h"

psk31_tdtl::psk31_tdtl(){

	sample_rate = 22050;
	bit_rate = 31.25;
	psk31freq = 1900;

	init();

}

psk31_tdtl::psk31_tdtl(float sample_rate, float bit_rate, float psk31freq){

	this->sample_rate = sample_rate;
	this->bit_rate = bit_rate;
	this->psk31freq = psk31freq;

	init();

}

psk31_tdtl::~psk31_tdtl(){

	// TODO

}

void psk31_tdtl::init(){

	initVaricode();

	ninty_deg_phase_width = (sample_rate/psk31freq)/4.0;

	bitwidth = (int)floor((floor((psk31freq/bit_rate)+0.5)*(sample_rate/psk31freq))+0.5);

	input_data_width = (int)(bitwidth+ninty_deg_phase_width);
	input_data = new char[input_data_width];

	input_position = 0;

	input_count = 0;

	max = 0;

	threshhold_value = 0.5;

	last = -1;
	countlast = 1;

}

bool psk31_tdtl::proccess_byte(char next_data_point){

	bool newByte = false;

	input_data[input_position] = next_data_point;

	if(input_count > input_data_width){


		int pos;
		pos = input_position+1;
		if(pos >= input_data_width) pos = pos - input_data_width;
		char y1 = input_data[pos];
		pos = (int)floor(1+input_position+ninty_deg_phase_width);
		if(pos >= input_data_width) pos = pos - input_data_width;
		char x1 = input_data[pos];
		pos = (int)floor(input_position-ninty_deg_phase_width+1.5);
		if(pos < 0) pos = input_data_width + pos;
		char y2 = input_data[pos];
		char x2 = input_data[input_position];

		float pd = std::abs(atan2(y1, x1)-atan2(y2, x2));
		if(pd > PI) pd = std::abs(pd-2*PI);
 		phase_change_data.push_back(pd);

	}

	input_position++;
	input_count++;

	if(input_position > input_data_width) input_position = 0;

	if(phase_change_data.size() >= bitwidth/4){

		float phase_change_sum = 0.0;

		for(std::list<float>::iterator it = phase_change_data.begin(); it != phase_change_data.end(); it++){

			phase_change_sum += (*it);

		}

		if(phase_change_sum > max) max = phase_change_sum;

		int current_value;

		if(phase_change_sum > threshhold_value*max)
			current_value = 1;
		else
			current_value = 0;

		if(last == -1) last = current_value;

		if(current_value != last){

			for(int i = 0; i < floor(((float)countlast)/((float)bitwidth)+0.5); i++)
				bit_sequence.push_back(current_value);

			if(bit_sequence.size() >= 2 && bit_sequence[bit_sequence.size()-1] == 0 && bit_sequence[bit_sequence.size()-2] == 0){
				int one = 1;
				int val = 0;
				for(int i = bit_sequence.size()-3; i >= 0; i--){
					if(bit_sequence[i] == 1)
						val |= one;
					one <<= 1;
				}

				newByte = true;

				byte_sequence.push_back(getVaricode(val));
				bit_sequence.clear();
			}

			last = current_value;
			countlast = 1;

		} else countlast++;

		phase_change_data.pop_front();

	}

	return newByte;

}

