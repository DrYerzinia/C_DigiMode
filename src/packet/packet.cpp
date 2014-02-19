// packet.cpp

#include "packet.h"

static unsigned short crc_table [256] = {

0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned short CRCCCITT(char_array &data){

	unsigned short crc = 0xFFFF;
	unsigned short temp;
	
	for(int i = 0; i < data.len-2; ++i) {
		unsigned char dat = data.data[i];
		unsigned char rv = 0x00;
		for(int j = 0; j < 8; j++, dat >>= 1)
			rv = (rv << 1) | (dat & 0x01);
		dat = rv;
		temp = (dat^(crc >> 8)) & 0xFF;
		crc = crc_table[temp] ^ (crc << 8);
	}

	crc ^= 0xFFFF;
	unsigned short rv = 0x0000;
	for(int j = 0; j < 16; j++, crc >>= 1)
		rv = (rv << 1) | (crc & 0x01);

	crc = (rv>>8)|(rv<<8);

	return crc;

}

packet::packet(){

	sample_rate = 44100;
	bit_rate = 1200;

	offset = 0;
	hysteresis = 0;
	noise_floor = 0;

	frequency_0 = 1200;
	frequency_1 = 2200;

	start_only();
	init();

}

packet::packet(float sr, float br){

	sample_rate = sr;
	bit_rate = br;

	offset = 0;
	hysteresis = 0;
	noise_floor = 0;

	frequency_0 = 1200;
	frequency_1 = 2200;

	start_only();
	init();

}

packet::packet(float sr, float br, float off, float hys, float nf){

	sample_rate = sr;
	bit_rate = br;

	offset = off;
	hysteresis = hys;
	noise_floor = nf;

	frequency_0 = 1200;
	frequency_1 = 2200;

	start_only();
	init();

}

packet::packet(float sr, float br, float f0, float f1){

	sample_rate = sr;
	bit_rate = br;

	offset = 0;
	hysteresis = 0;
	noise_floor = 0;

	frequency_0 = f0;
	frequency_1 = f1;

	master_count = 0;

	start_only();
	init();

}

packet::packet(float sr, float br, float off, float hys, float nf, float f0, float f1){

	sample_rate = sr;
	bit_rate = br;

	offset = off;
	hysteresis = hys;
	noise_floor = nf;

	frequency_0 = f0;
	frequency_1 = f1;

	start_only();
	init();

}

packet::~packet(){
}

void packet::start_only(){

	fcMax = 0;
	fcMin = 0;
	master_count = 0;

}

void packet::init(){

	window = round(sample_rate/bit_rate);

	bitwidth = sample_rate/bit_rate;

	last = -1;
	last_bit = 0;

	sample_counter = 0;
	same_count = 0;

	freq_sync_found = false;
	packet_start = false;

	bit_stuffing = false;

}

void packet::reset(){

	max = 0;
	countlast = 0;

	input_buffer.clear();

	//last_byte_sequence = byte_sequence;

	byte_sequence.clear();
	bit_sequence.clear();

	init();

}

char_array* packet::proccess_byte(char data_point){

	//if(sample_counter > sample_rate*2){
	//	reset();
	//	std::cout << std::endl;
	//}

	char_array* new_data = NULL;

	input_buffer.push_back(data_point);
	master_count++;

	if(input_buffer.size() > window){

		float s1i = 0;
		float s1q = 0;
		float s2i = 0;
		float s2q = 0;

		int n = 0;
		for(std::list<char>::iterator it = input_buffer.begin(); it != input_buffer.end(); it++){

			s1i += ((float)*it)*(sin(2*PI*frequency_0*(sample_counter+n)/sample_rate));
			s1q += ((float)*it)*(cos(2*PI*frequency_0*(sample_counter+n)/sample_rate));

			s2i += ((float)*it)*(sin(2*PI*frequency_1*(sample_counter+n)/sample_rate));
			s2q += ((float)*it)*(cos(2*PI*frequency_1*(sample_counter+n)/sample_rate));

			n++;

		}

		float fc1 = s1i*s1i + s1q*s1q;
		float fc2 = s2i*s2i + s2q*s2q;

		float fcd = (fc1 - fc2);

		if(fcd > fcMax)
			fcMax = fcd*0.85 + fcMax*0.15;
		else
			fcMax = fcd*0.00015 + fcMax*0.99985;

		if(fcd < fcMin)
			fcMin = fcd*0.85 + fcMin*0.15;
		else
			fcMin = fcd*0.00015 + fcMin*0.99985;

		int generated_offset = (fcMax-fcMin)*offset;

		fcd -= generated_offset;

		input_buffer.pop_front();

		fcd_buffer.push_back(fcd);

		if(fcd_buffer.size() > window){

			float fcd_avg = 0;
			for(std::list<float>::iterator it = fcd_buffer.begin(); it != fcd_buffer.end(); it++)
				fcd_avg += (*it);
			fcd_avg /= fcd_buffer.size();

			fcd_buffer.pop_front();
											//if(!debugs.is_open()){debugs.open("out.raw", std::fstream::out);}debugs.put(fcd_avg/10000);debugs.flush();
			//int real_value = current_value;
			//if(std::abs(fcd_avg) < noise_floor)
			//	current_value = last;

			//if(last == -1) last = current_value;

			fcavg_buffer.push_back(fcd_avg);
//			std::cout << fcavg_buffer.size() << std::endl;

			int current_bits = fcavg_buffer.size();

			bool keep_going = true;

			// Wait until we have a x+1 bits stored up
			if(current_bits % ((int)bitwidth) == 0 && current_bits != 0 && current_bits != ((int)bitwidth)){

				int transition = 0;

				// If no freq_sych lock to nearest zero crossing 50% ahead behind
				if(1){//!freq_sync_found){

					// Zero crossing index
					int zc_index = -1;

					int i;
					// +/- half a bit
					for(i = current_bits-bitwidth-(bitwidth/2); i < fcavg_buffer.size() && i < current_bits-bitwidth+(bitwidth/2); i++){
						// If the bits are one negative one positive IE zero-crossing
						std::list<float>::iterator it1 = fcavg_buffer.begin();
						std::list<float>::iterator it2 = fcavg_buffer.begin();
						std::advance(it1, i-1);
						std::advance(it2,   i);
						if( ((*it1) >= 0 && (*it2) < 0) || ((*it2) >= 0 && (*it1) < 0)){
							zc_index = i;
						}
					}

					// No zero crossing keep on going
					if(zc_index == -1) keep_going = false;

					// Found something!
					else
						transition = zc_index;

				}
				// check 20% of a bit width ahead/behind current bits out for enough slope to be considered a change
				// enough slope to be considered a change is relative to the magnitude of the Fourier coefficient difference
				// and slope is taken over a window width
				// if it is a change and there is a zero crossing in the 20% lock to zero crossing
				// if no zero crossing just center at the current distance out
				// if its a change keep going
				if(keep_going){

					// Remove bits we just processed TODO: RING BUFFER
					int i;
					for(i = 0; i < transition; i++)
						fcavg_buffer.pop_front();

					int current_value = 0;
					if(fcavg_buffer.front() < 0)
						current_value = 1;

					// Calculate how many bit lengths there are to the transition
					float new_bits = round(((float)transition)/((float)bitwidth));

					//std::cout << new_bits << " " << master_count << " ";

					for(int i = 0; i < round(new_bits); i++){
						int next_bit = 0;
						bool fb = bit_stuffing;
						if((current_value == 0 && last_bit == 0) || (current_value == 1 && last_bit == 1)){
							next_bit = 1;
							if(same_count == 4){
								if(freq_sync_found && packet_start){
									bit_stuffing = true;
									same_count = 0;
								}
							} else same_count++;
						} else same_count = 0;
						last_bit = current_value;
						if(!(bit_stuffing && next_bit == 0 && fb)){
							bit_sequence.push_back(next_bit);
							//std::cout << next_bit;
						} else {
							//std::cout << " BTST ";
						}
						if(fb) bit_stuffing = false;
					}
					//std::cout << std::endl;
					// needs hysteresis

					if(bit_sequence.size() >= 8
					&& bit_sequence[bit_sequence.size()-8] == 0
					&& bit_sequence[bit_sequence.size()-7] == 1
					&& bit_sequence[bit_sequence.size()-6] == 1
					&& bit_sequence[bit_sequence.size()-5] == 1
					&& bit_sequence[bit_sequence.size()-4] == 1
					&& bit_sequence[bit_sequence.size()-3] == 1
					&& bit_sequence[bit_sequence.size()-2] == 1
					&& bit_sequence[bit_sequence.size()-1] == 0){

						if(packet_start) {
							if(byte_sequence.size() >= 14){

								unsigned short len = byte_sequence.size();
								char *data = byte_sequence.data();

								new_data = (char_array*) malloc(sizeof(char_array));
								new_data->len = len;
								new_data->data = (char*) malloc(sizeof(char)*len);
								memcpy(new_data->data, data, len);

							}
							//std::cout << "\n\nENDPACKET\n\n";
							freq_sync_found = false;
							reset();
						} else {
							freq_sync_found = true;
							packet_start = false;
						}
						bit_sequence.clear();
					}
					if(freq_sync_found && bit_sequence.size() >= 15){
						if(!packet_start){

							packet_start = true;
							//bit_sequence.erase(bit_sequence.begin(),bit_sequence.begin()+1);
						} else {
							char byte = 0;
							for(int i = 7; i >= 0; i--){
								byte <<= 1;
								if(bit_sequence[i]) byte |= 1;
							}
							// REMOVE
							char pc = ' ';
							if(std::isprint(byte)) pc = byte;
							//std::cout << std::bitset<8>(byte) << " " << pc << std::endl;
							//std::cout.flush();
							// END REMOVE
							bit_sequence.erase(bit_sequence.begin(),bit_sequence.begin()+8);
							//std::cout << std::hex << " " << (int)((unsigned char)byte) << " " << std::dec << std::bitset<8>(byte);
							byte_sequence.push_back(byte);
						}
					}
					//if(?){

						// while decoding check for end of message frame sync and have timeout
						//newByte = true;

						//byte_sequence.push_back(?);
						//bit_sequence.clear();
					//}

					last = current_value;
					countlast = 1 + different_erasure_count;
					different_erasure_count = 0;

				}

			} else
				countlast++;

		}

	}

	sample_counter++;

	return new_data;

}

void packet::set_sample_rate(float sr){
	sample_rate = sr;
}

void packet::set_bit_rate(float br){
	bit_rate = br;
}

void packet::set_frequency_0(float f0){
	frequency_0 = f0;
}

void packet::set_frequency_1(float f1){
	frequency_1 = f1;
}

void packet::set_offset(float off){
	offset = off;
}

void packet::set_hysteresis(float hys){
	hysteresis = hys;
}

void packet::set_noise_floor(float nf){
	noise_floor = nf;
}

