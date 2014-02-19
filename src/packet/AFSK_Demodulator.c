// AFSK_Demodulator.c

#include "AFSK_Demodulator.h"

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

	self->window = (int)(self->sample_rate/self->bit_rate+0.5);

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

			float time = 2*PI*(self->sample_counter+i)/self->sample_rate;

			float phase0 = self->frequency_0 * time;
			float phase1 = self->frequency_1 * time;

			s1i += char_ring_buffer_get(&self->input_buffer, i) * rad_sin(phase0);
			s1q += char_ring_buffer_get(&self->input_buffer, i) * rad_cos(phase0);

			s2i += char_ring_buffer_get(&self->input_buffer, i) * rad_sin(phase1);
			s2q += char_ring_buffer_get(&self->input_buffer, i) * rad_cos(phase1);

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
					float new_bits = (int)((((float)transition)/((float)self->bitwidth))+0.5);

					for(i = 0; i < (int)(new_bits+0.5); i++){
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

