// NOAA_ATP.cpp

#include "NOAA_ATP.h"

//REMOVE
#include <iostream>
//END

NOAA_ATP::NOAA_ATP(){

	sample_rate = 11025;
	detection_threshold = 2;

	init();
	
}

NOAA_ATP::NOAA_ATP(int sr){

	sample_rate = sr;
	detection_threshold = 2;

	init();

}

NOAA_ATP::~NOAA_ATP(){

	//

}

void NOAA_ATP::init(){

	sample_count = 0;
	input_buffer_start = 0;

}

bool NOAA_ATP::proccess_byte(char input_data_point){

	ef_buffer.push_back(input_data_point);

	if(ef_buffer.size() > sample_rate/(NOAA_SUBCARRIER_FREQ)){

		char max = 0;
		for(std::list<char>::iterator it = ef_buffer.begin(); it != ef_buffer.end(); it++){
			char val = std::abs(*it);
			if(val > max) max = val;
		}

		envelope_follower_out.push_back(max);
		sample_count++;

		if(max > 64) max = 1;
		else max = 0;
		sync_pulse_detect_buf.push_back(max);

		bool sync_a_detected = false;
		bool sync_b_detected = false;

		if(sync_pulse_detect_buf.size() > 7.0*sample_rate/SYNC_B_FREQ+10){
			int last_state = (*sync_pulse_detect_buf.begin());
			int count_p_high = 0;
			int count_p_low = 0;
			int count_high = 0;
			int p_count_last = 0;
			int count_low = 0;
			int count_sample = 0;
			bool start_count = false;
			for(std::list<char>::iterator it = sync_pulse_detect_buf.begin(); it != sync_pulse_detect_buf.end(); it++){
				if((*it) != last_state){
					if(last_state == 0){
						count_low++;
						start_count = true;
					} else {
						if(start_count) count_high++;
					}
					last_state = *it;
					p_count_last = 0;
				}
				if(start_count){
					if((*it) == 0){
						count_p_low++;
					} else {
						count_p_high++;
					}
				}
				if(start_count){
					if(p_count_last > 14) break;
					p_count_last++;
				}
				if(count_high == 7){
					float pulse_average = float(count_p_high)/float(count_p_low);
					if((std::abs(count_sample-(7.0*sample_rate/SYNC_A_FREQ)) < detection_threshold+4)
					&& (pulse_average > 2.0-(detection_threshold/4.0))){
						sync_a_detected = true;
					}
					if((std::abs(count_sample-(7.0*sample_rate/SYNC_B_FREQ)) < detection_threshold+4)
					&& (pulse_average > 2.0-(detection_threshold/4.0))){
						sync_b_detected = true;
					}
					//std::cout << "thrsh: " << std::abs(count_sample-(7.0*sample_rate/SYNC_B_FREQ)) << ", PA: " << pulse_average << std::endl;
					//std::cout << envelope_follower_out.size() << ", " << count_sample << std::endl;
					break;
				}
				if(start_count) count_sample++;
			}
			sync_pulse_detect_buf.pop_front();
		}

		if(sync_a_detected && (sync_a_positions.empty() || (sample_count-(*(sync_a_positions.rbegin())) > 110)))
			sync_a_positions.push_back(sample_count);
		if(sync_b_detected && (sync_b_positions.empty() || (sample_count-(*(sync_b_positions.rbegin())) > 110)))
			sync_b_positions.push_back(sample_count);

		if(sync_a_positions.size() >= 10){
			int prob_arr[10][2];
			int count = 0;

			for(std::list<int>::iterator it = sync_a_positions.begin(); it != sync_a_positions.end(); it++, count++)
				prob_arr[count][0] = *it;

			for(int i = 0; i < 10; i++)
				prob_arr[i][1] = 0;

			for(int i = 0; i < 10; i++){
				for(int j = 0; j < 10; j++){
					prob_arr[i][1] += int(std::abs(prob_arr[i][0]-prob_arr[j][0]))%int(sample_rate/2);
				}
			}

			std::list<int>::iterator it_sap = sync_a_positions.begin();
			for(int i = 0; i < 10; i++){
				if(prob_arr[i][1] > sample_rate/8){
					it_sap = sync_a_positions.erase(it_sap);
				} else it_sap++;
			}

			if(!sync_a_positions.empty()){
				sync_a_positions_cnf.push_back(sync_a_positions.front());
				sync_a_positions.pop_front();
			}

		}

		if(sync_b_positions.size() >= 10){
			int prob_arr[10][2];
			int count = 0;

			for(std::list<int>::iterator it = sync_b_positions.begin(); it != sync_b_positions.end(); it++, count++)
				prob_arr[count][0] = *it;

			for(int i = 0; i < 10; i++)
				prob_arr[i][1] = 0;

			for(int i = 0; i < 10; i++){
				for(int j = 0; j < 10; j++){
					prob_arr[i][1] += int(std::abs(prob_arr[i][0]-prob_arr[j][0]))%int(sample_rate/2);
				}
			}

			std::list<int>::iterator it_sap = sync_b_positions.begin();
			for(int i = 0; i < 10; i++){
				if(prob_arr[i][1] > sample_rate/8){
					it_sap = sync_b_positions.erase(it_sap);
				} else it_sap++;
			}

			if(!sync_b_positions.empty()){
				sync_b_positions_cnf.push_back(sync_b_positions.front());
				sync_b_positions.pop_front();
			}

		}

		if(sync_b_positions_cnf.size() >= 2){

			int cnf_sync_pos = sync_b_positions_cnf.front()-int(7.0*sample_rate/SYNC_B_FREQ+10);
			std::cout << "CSP: " << cnf_sync_pos << std::endl;
			while(cnf_sync_pos > input_buffer_start){

				int front_sync_offset = (cnf_sync_pos-input_buffer_start)%int(sample_rate/2);

				std::cout << "IBS: " << input_buffer_start << std::endl;
				std::cout << "FSO: " << front_sync_offset << std::endl;

				if(front_sync_offset != 0){

					std::list<char>::iterator it_efb_ers = envelope_follower_out.begin();
					std::advance(it_efb_ers,front_sync_offset);

					envelope_follower_out.erase(envelope_follower_out.begin(), it_efb_ers);

					input_buffer_start += front_sync_offset;

				}

				std::vector<char> nextLine;
				std::list<char>::iterator it_efb = envelope_follower_out.begin();
				if(cnf_sync_pos - input_buffer_start >= int(sample_rate/2)){

					for(int i = 0; i < int(sample_rate/2); i++){
						nextLine.push_back(*it_efb);
						it_efb++;
					}

					input_buffer_start += int(sample_rate/2);
					line_data.push_back(nextLine);

					envelope_follower_out.erase(envelope_follower_out.begin(), it_efb);

				} else {

					int excess_buffer = cnf_sync_pos - input_buffer_start;

					std::cout << "EB : " << excess_buffer << std::endl;

					std::list<char>::iterator it_efb_ers = envelope_follower_out.begin();
					std::advance(it_efb_ers, excess_buffer);

					envelope_follower_out.erase(envelope_follower_out.begin(), it_efb_ers);	

					input_buffer_start += excess_buffer;

				}

			}

			sync_b_positions_cnf.pop_front();

			int cnf_sync_pos_2 = sync_b_positions_cnf.front()-int(7.0*sample_rate/SYNC_B_FREQ+10);

			int data_in_frames = cnf_sync_pos_2-cnf_sync_pos;

			std::cout << "SNCDIF: " << data_in_frames << std::endl;

			int frames = int(((data_in_frames)/(sample_rate/2))+0.5);

			std::cout << "FRAMES: " << frames << std::endl;

			std::cout << "IBS2: " << input_buffer_start << std::endl;

			int count_total = 0;
			for(int i = 0; i < frames; i++){

				std::list<char>::iterator it_efb = envelope_follower_out.begin();
				std::vector<char> nextLine;
				for(int i = 0; i < int(sample_rate/2); i++){

					if(count_total < data_in_frames){
						nextLine.push_back(*it_efb);
						it_efb++;
					} else {
						nextLine.push_back(0);
					}

					count_total++;

				}

				input_buffer_start += int(sample_rate/2);
				line_data.push_back(nextLine);

				envelope_follower_out.erase(envelope_follower_out.begin(), it_efb);

			}

		}

		ef_buffer.pop_front();

	}

}

void NOAA_ATP::set_sample_rate(int sr){

	sample_rate = sr;

}
