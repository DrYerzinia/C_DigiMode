// AFSK_Demodulator.c

#include "AFSK_Demodulator.h"

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

unsigned short CRCCCITT(char_array *data){

	unsigned short crc = 0xFFFF;
	unsigned short temp;

	int i;
	for(i = 0; i < data->len-2; ++i) {
		unsigned char dat = data->data[i];
		unsigned char rv = 0x00;
		int j;
		for(j = 0; j < 8; j++, dat >>= 1)
			rv = (rv << 1) | (dat & 0x01);
		dat = rv;
		temp = (dat^(crc >> 8)) & 0xFF;
		crc = crc_table[temp] ^ (crc << 8);
	}

	crc ^= 0xFFFF;
	unsigned short rv = 0x0000;
	for(i = 0; i < 16; i++, crc >>= 1)
		rv = (rv << 1) | (crc & 0x01);

	crc = (rv>>8)|(rv<<8);

	return crc;

}

void start_only(AFSK_Demodulator *self){

	self->fcMax = 0;
	self->fcMin = 0;

}

void AFSK_Demodulator_init(AFSK_Demodulator *self, float sr, float br, float off, float hys, float nf){

	self->sample_rate = sr;
	self->bit_rate = br;

	self->offset = off;
	self->hysteresis = hys;
	self->noise_floor = nf;

	self->frequency_0 = 1200;
	self->frequency_1 = 2200;

	self->input_buffer.size = 0;
	self->fcd_buffer.size   = 0;
	self->fcavg_buffer.size = 0;

	char_ring_buffer_init(&self->bit_sequence, 30);

	char_array_expandable_init(&self->byte_sequence, 330);

	start_only(self);
	reset(self);

}

void AFSK_Demodulator_destory(AFSK_Demodulator *self){

	if(self->input_buffer.size != 0)
		char_ring_buffer_destory(&self->input_buffer);
	if(self->fcd_buffer.size != 0)
		float_ring_buffer_destory(&self->fcd_buffer);
	if(self->fcavg_buffer.size != 0)
		float_ring_buffer_destory(&self->fcavg_buffer);

	if(self->bit_sequence.size != 0)
		char_ring_buffer_destory(&self->bit_sequence);

	if(self->byte_sequence.capacity != 0)
		char_array_expandable_destroy(&self->byte_sequence);

}

void reset(AFSK_Demodulator *self){

	self->max = 0;
	self->countlast = 0;

	self->window = round(self->sample_rate/self->bit_rate);

	self->bitwidth = self->sample_rate/self->bit_rate;

	self->last = -1;
	self->last_bit = 0;

	self->sample_counter = 0;
	self->same_count = 0;

	self->freq_sync_found = false;
	self->packet_start = false;

	self->bit_stuffing = false;

	if(self->input_buffer.size != 0)
		char_ring_buffer_destory(&self->input_buffer);
	char_ring_buffer_init(&self->input_buffer, self->window+2);

	if(self->fcd_buffer.size != 0)
		float_ring_buffer_destory(&self->fcd_buffer);
	float_ring_buffer_init(&self->fcd_buffer, self->window+2);

	if(self->fcavg_buffer.size != 0)
		float_ring_buffer_destory(&self->fcavg_buffer);
	float_ring_buffer_init(&self->fcavg_buffer, self->bitwidth*10);

	char_array_expandable_clear(&self->byte_sequence);
	char_ring_buffer_clear(&self->bit_sequence);

}

char_array* proccess_byte(AFSK_Demodulator *self, char data_point){

	char_array* new_data = NULL;

	char_ring_buffer_put(&self->input_buffer, data_point);

	if(char_ring_buffer_avail(&self->input_buffer) > self->window){

		float s1i = 0;
		float s1q = 0;
		float s2i = 0;
		float s2q = 0;

		int i;
		for(i = 0; i <= self->window; i++){

			s1i += char_ring_buffer_get(&self->input_buffer, i)*(sin(2*PI*self->frequency_0*(self->sample_counter+i)/self->sample_rate));
			s1q += char_ring_buffer_get(&self->input_buffer, i)*(cos(2*PI*self->frequency_0*(self->sample_counter+i)/self->sample_rate));

			s2i += char_ring_buffer_get(&self->input_buffer, i)*(sin(2*PI*self->frequency_1*(self->sample_counter+i)/self->sample_rate));
			s2q += char_ring_buffer_get(&self->input_buffer, i)*(cos(2*PI*self->frequency_1*(self->sample_counter+i)/self->sample_rate));

		}

		float fc1 = s1i*s1i + s1q*s1q;
		float fc2 = s2i*s2i + s2q*s2q;

		float fcd = (fc1 - fc2);

		if(fcd > self->fcMax)
			self->fcMax = fcd*0.85 + self->fcMax*0.15;
		else
			self->fcMax = fcd*0.00015 + self->fcMax*0.99985;

		if(fcd < self->fcMin)
			self->fcMin = fcd*0.85 + self->fcMin*0.15;
		else
			self->fcMin = fcd*0.00015 + self->fcMin*0.99985;

		int generated_offset = (self->fcMax-self->fcMin)*self->offset;

		fcd -= generated_offset;

		char_ring_buffer_pop(&self->input_buffer);

		float_ring_buffer_put(&self->fcd_buffer, fcd);

		int avail = float_ring_buffer_avail(&self->fcd_buffer);
		if(avail > self->window){

			float fcd_avg = 0;
			for(i = 0; i < avail; i++)
				fcd_avg += float_ring_buffer_get(&self->fcd_buffer, i);
			fcd_avg /= avail;

			float_ring_buffer_pop(&self->fcd_buffer);

			float_ring_buffer_put(&self->fcavg_buffer, fcd_avg);

			int current_bits = float_ring_buffer_avail(&self->fcavg_buffer);

			bool keep_going = true;

			// Wait until we have a x+1 bits stored up
			if(current_bits % ((int)self->bitwidth) == 0 && current_bits != 0 && current_bits != ((int)self->bitwidth)){

				int transition = 0;

				// If no freq_sych lock to nearest zero crossing 50% ahead behind
				if(1){//!freq_sync_found){

					// Zero crossing index
					int zc_index = -1;

					int i;
					// +/- half a bit
					avail = float_ring_buffer_avail(&self->fcavg_buffer);
					for(i = current_bits-self->bitwidth-(self->bitwidth/2); i < avail && i < current_bits-self->bitwidth+(self->bitwidth/2); i++){
						// If the bits are one negative one positive IE zero-crossing
						float a = float_ring_buffer_get(&self->fcavg_buffer, i-1);
						float b = float_ring_buffer_get(&self->fcavg_buffer,   i);
						if( (a >= 0 && b < 0) || (b >= 0 && a < 0)){
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

					// Remove bits we just processed
					float_ring_buffer_remove(&self->fcavg_buffer, transition);

					int current_value = 0;
					if(float_ring_buffer_get(&self->fcavg_buffer, 0) < 0)
						current_value = 1;

					// Calculate how many bit lengths there are to the transition
					float new_bits = round(((float)transition)/((float)self->bitwidth));

					for(i = 0; i < round(new_bits); i++){
						int next_bit = 0;
						bool fb = self->bit_stuffing;
						if((current_value == 0 && self->last_bit == 0) || (current_value == 1 && self->last_bit == 1)){
							next_bit = 1;
							if(self->same_count == 4){
								if(self->freq_sync_found && self->packet_start){
									self->bit_stuffing = true;
									self->same_count = 0;
								}
							} else self->same_count++;
						} else self->same_count = 0;
						self->last_bit = current_value;
						if(!(self->bit_stuffing && next_bit == 0 && fb)){
							char_ring_buffer_put(&self->bit_sequence, next_bit);
							//std::cout << next_bit;
						} else {
							//std::cout << " BTST ";
						}
						if(fb) self->bit_stuffing = false;
					}
					//std::cout << std::endl;
					// needs hysteresis

					avail = char_ring_buffer_avail(&self->bit_sequence);
					if(avail >= 8
					&& char_ring_buffer_get(&self->bit_sequence, avail-8) == 0
					&& char_ring_buffer_get(&self->bit_sequence, avail-7) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-6) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-5) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-4) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-3) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-2) == 1
					&& char_ring_buffer_get(&self->bit_sequence, avail-1) == 0){

						if(self->packet_start) {
							unsigned short len = char_array_expandable_size(&self->byte_sequence);
							if(len >= 14){

								char *data = self->byte_sequence.data;

								new_data = (char_array*) malloc(sizeof(char_array));
								new_data->len = len;
								new_data->data = (char*) malloc(sizeof(char)*len);
								memcpy(new_data->data, data, len);

							}
							self->freq_sync_found = false;
							reset(self);
						} else {
							self->freq_sync_found = true;
							self->packet_start = false;
						}
						char_ring_buffer_clear(&self->bit_sequence);
					}
					if(self->freq_sync_found && avail >= 15){
						if(!self->packet_start){

							self->packet_start = true;
							//bit_sequence.erase(bit_sequence.begin(),bit_sequence.begin()+1);
						} else {
							char byte = 0;
							for(i = 7; i >= 0; i--){
								byte <<= 1;
								if(char_ring_buffer_get(&self->bit_sequence, i)) byte |= 1;
							}
							// REMOVE
							char pc = ' ';
							if(isprint(byte)) pc = byte;
							//std::cout << std::bitset<8>(byte) << " " << pc << std::endl;
							//std::cout.flush();
							// END REMOVE
							char_ring_buffer_remove(&self->bit_sequence, 8);
							//std::cout << std::hex << " " << (int)((unsigned char)byte) << " " << std::dec << std::bitset<8>(byte);
							char_array_expandable_put(&self->byte_sequence, byte);
						}
					}
					//if(?){

						// while decoding check for end of message frame sync and have timeout
						//newByte = true;

						//byte_sequence.push_back(?);
						//bit_sequence.clear();
					//}

					self->last = current_value;

				}

			} else
				self->countlast++;

		}

	}

	self->sample_counter++;

	return new_data;

}

void set_sample_rate(AFSK_Demodulator *self, float sr){
	self->sample_rate = sr;
}

void set_bit_rate(AFSK_Demodulator *self, float br){
	self->bit_rate = br;
}

void set_frequency_0(AFSK_Demodulator *self, float f0){
	self->frequency_0 = f0;
}

void set_frequency_1(AFSK_Demodulator *self, float f1){
	self->frequency_1 = f1;
}

void set_offset(AFSK_Demodulator *self, float off){
	self->offset = off;
}

void set_hysteresis(AFSK_Demodulator *self, float hys){
	self->hysteresis = hys;
}

void set_noise_floor(AFSK_Demodulator *self, float nf){
	self->noise_floor = nf;
}

