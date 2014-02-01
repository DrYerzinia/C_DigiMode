// psk31_wcpd.cpp

#include "psk31_wcpd.h"

psk31_wcpd::psk31_wcpd(){

	sample_rate = 22050;
	bit_rate = 31.25;
	psk31freq = 1900;

	init();

}

psk31_wcpd::psk31_wcpd(float sample_rate, float bit_rate, float psk31freq){

	this->sample_rate = sample_rate;
	this->bit_rate = bit_rate;
	this->psk31freq = psk31freq;

	init();

}

psk31_wcpd::~psk31_wcpd(){

	// TODO

}

void psk31_wcpd::init(){

	initVaricode();

	// Match Wave Phase against 1 full wavelength
	ninty_deg_phase_width = (sample_rate/psk31freq);

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

bool psk31_wcpd::proccess_byte(char next_data_point){

	bool newByte = false;

	input_data[input_position] = next_data_point;

	if(input_count > input_data_width){


		int pos;
		int postmp;

		pos = input_position+1;

		int topsum1, topsum2;
		float max;

		float phasesum[(int)ninty_deg_phase_width];

		for(int i = 0; i < ninty_deg_phase_width; i++)
			phasesum[i] = 0.0f;

		for(int i = 0; i < ninty_deg_phase_width; i++){

			postmp = pos + i;
			if(postmp < 0) postmp = input_data_width + postmp;
			else if(postmp >= input_data_width) postmp = postmp - input_data_width;

			for(int j = 0; j < ninty_deg_phase_width; j++){
				phasesum[j] += input_data[postmp]*sin(((input_count+i)/ninty_deg_phase_width)*2*PI+(j*2*PI/ninty_deg_phase_width));
			}

		}

		max = -100000;
		for(int i = 0; i < ninty_deg_phase_width; i++){
			if(phasesum[i] > max){
				max = phasesum[i];
				topsum1 = i;
			}
		}

		pos = (int)floor(input_position-ninty_deg_phase_width+1.5);

		for(int i = 0; i < ninty_deg_phase_width; i++)
			phasesum[i] = 0.0f;

		for(int i = 0; i < ninty_deg_phase_width; i++){

			postmp = pos + i;
			if(postmp < 0) postmp = input_data_width + postmp;
			else if(postmp >= input_data_width) postmp = postmp - input_data_width;

			for(int j = 0; j < ninty_deg_phase_width; j++){
				phasesum[j] += input_data[postmp]*sin(((input_count+i)/ninty_deg_phase_width)*2*PI+(j*2*PI/ninty_deg_phase_width));
			}

		}

		max = -100000;
		for(int i = 0; i < ninty_deg_phase_width; i++){
			if(phasesum[i] > max){
				max = phasesum[i];
				topsum2 = i;
			}
		}

		float pd = std::abs(topsum1-topsum2);
		if(pd > ninty_deg_phase_width/2) pd = std::abs(pd-ninty_deg_phase_width);
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

