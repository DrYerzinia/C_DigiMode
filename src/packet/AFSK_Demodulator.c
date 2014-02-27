// AFSK_Demodulator.c

#include "AFSK_Demodulator.h"

#ifdef __DEBUG
	#include <stdio.h>
	FILE *fourier_coefficient_debug = NULL;
#endif

void AFSK_Demodulator_reset(AFSK_Demodulator *self){

	self->count_last = 0;

	self->window = (int)(self->sample_rate/self->bit_rate+0.5);

	self->bitwidth = self->sample_rate/self->bit_rate;

	/*
	 * Calculate Goertzel coefficents for calculating frequency magnitudes
	 */
	float k0 = (int)(0.5+((float)self->window*self->frequency_0/self->sample_rate));
	float k1 = (int)(0.5+((float)self->window*self->frequency_1/self->sample_rate));
	float w0 = (2*PI/self->window)*k0;
	float w1 = (2*PI/self->window)*k1;

	self->coeff0 = 2*cos(w0);
	self->coeff1 = 2*cos(w1);

	self->last_bit = 0;

	self->bit_stuffing = false;

	if(self->input_buffer.size != 0)
		char_ring_buffer_destory(&self->input_buffer);
	char_ring_buffer_init(&self->input_buffer, self->window+2);

	if(self->fcd_buffer.size != 0)
		float_ring_buffer_destory(&self->fcd_buffer);
	float_ring_buffer_init(&self->fcd_buffer, self->window+2);

	char_array_expandable_clear(&self->byte_sequence);
	char_ring_buffer_clear(&self->bit_sequence);

}

void AFSK_Demodulator_init(AFSK_Demodulator *self, uint32_t sr, uint16_t br, float off, float nf, uint16_t frequency_0, uint16_t frequency_1){

	#ifdef __DEBUG
		fourier_coefficient_debug = fopen("fourier_coefficient_debug.raw", "w");
	#endif

	self->sample_rate = sr;
	self->bit_rate = br;

	self->offset = off;
	self->noise_floor = nf;

	self->frequency_0 = frequency_0;
	self->frequency_1 = frequency_1;

	self->input_buffer.size = 0;
	self->fcd_buffer.size   = 0;

	self->fcMax = 0;
	self->fcMin = 0;

	char_ring_buffer_init(&self->bit_sequence, 14);

	char_array_expandable_init(&self->byte_sequence, 330);

	AFSK_Demodulator_reset(self);

}

void AFSK_Demodulator_destroy(AFSK_Demodulator *self){

	if(self->input_buffer.size != 0)
		char_ring_buffer_destory(&self->input_buffer);
	if(self->fcd_buffer.size != 0)
		float_ring_buffer_destory(&self->fcd_buffer);

	if(self->bit_sequence.size != 0)
		char_ring_buffer_destory(&self->bit_sequence);

	if(self->byte_sequence.capacity != 0)
		char_array_expandable_destroy(&self->byte_sequence);

}

char_array* AFSK_Demodulator_proccess_byte(AFSK_Demodulator *self, int8_t data_point){

	char_array* new_data = NULL;

	char_ring_buffer_put(&self->input_buffer, data_point);

	if(char_ring_buffer_avail(&self->input_buffer) > self->window){

		float q1_0 = 0;
		float q1_1 = 0;
		float q2_0 = 0;
		float q2_1 = 0;

		int16_t i;
		for(i = 0; i <= self->window; i++){

			float q0_0 = self->coeff0*q1_0 - q2_0 + char_ring_buffer_get(&self->input_buffer, i);
			float q0_1 = self->coeff1*q1_1 - q2_1 + char_ring_buffer_get(&self->input_buffer, i);
			q2_0 = q1_0;
			q2_1 = q1_1;
			q1_0 = q0_0;
			q1_1 = q0_1;

		}

		float fc1 = q1_0*q1_0 + q2_0*q2_0 - q1_0*q2_0*self->coeff0;
		float fc2 = q1_1*q1_1 + q2_1*q2_1 - q1_1*q2_1*self->coeff1;

		float fcd = fc1 - fc2;

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

		uint16_t avail = float_ring_buffer_avail(&self->fcd_buffer);
		if(avail > self->window/2){

			float fcd_avg = 0;
			for(i = 0; i < avail; i++)
				fcd_avg += float_ring_buffer_get(&self->fcd_buffer, i);
			fcd_avg /= avail;

			float_ring_buffer_pop(&self->fcd_buffer);

			/* Debug code to dump averaged Fourier coefficients to raw data
			 * file for analysis
			 */
			#ifdef __DEBUG
				fputc(fcd_avg/10000, fourier_coefficient_debug);
			#endif

			uint8_t current_value = 0;
			if(fcd_avg < 0)
				current_value = 1;

			if(current_value != self->last_bit){

				self->last_bit = current_value;

				// Calculate how many bit lengths there are to the transition
				uint8_t new_bits = (int)((((float)self->count_last)/((float)self->bitwidth))+0.5);

				// If we are not bit stuffing Add a 0
				if(!self->bit_stuffing)
					char_ring_buffer_put(&self->bit_sequence, 0);

				// If we where bit stuffing stop now
				self->bit_stuffing = false;

				// Decrement new_bits
				new_bits--;

				// If new_bits > 5 we just found a preamble
				if(new_bits > 5){

					/* Mark the output waveform anytime we see a Preamble
					 */
					#ifdef __DEBUG
						fputc(127, fourier_coefficient_debug);
					#endif

					// Preamble related things

					uint16_t len = char_array_expandable_size(&self->byte_sequence);
					if(len >= 17){

						signed char *data = self->byte_sequence.data;
						new_data = (char_array*) malloc(sizeof(char_array));
						new_data->len = len;
						new_data->data = (signed char*) malloc(sizeof(char)*len);
						memcpy(new_data->data, data, len);

					}

					char_ring_buffer_clear(&self->bit_sequence);
					char_array_expandable_clear(&self->byte_sequence);

					// Set bit stuffing true so last bit of preamble will be removed
					self->bit_stuffing = true;

				}

				// If its not a preamble
				else {

					// If new_bits == 5 bit stuffing is occurring
					if(new_bits == 5)
						self->bit_stuffing = true;

					// Add the rest of the bits as 1's
					for(i = 0; i < new_bits; i++)
						char_ring_buffer_put(&self->bit_sequence, 1);

				}

				// Add bits to the sequence
				avail = char_ring_buffer_avail(&self->bit_sequence);
				if(avail >= 8){

					signed char byte = 0;
					for(i = 7; i >= 0; i--){
						byte <<= 1;
						if(char_ring_buffer_get(&self->bit_sequence, i)) byte |= 1;
					}

					char_ring_buffer_remove(&self->bit_sequence, 8);
					char_array_expandable_put(&self->byte_sequence, byte);

				}

				self->count_last = 0;

			} else
				self->count_last++;

		}

	}

	return new_data;

}

void AFSK_Demodulator_set_sample_rate(AFSK_Demodulator *self, uint32_t sr){
	self->sample_rate = sr;
	AFSK_Demodulator_reset(self);
}

void AFSK_Demodulator_set_bit_rate(AFSK_Demodulator *self, uint16_t br){
	self->bit_rate = br;
	AFSK_Demodulator_reset(self);
}

void AFSK_Demodulator_set_frequency_0(AFSK_Demodulator *self, uint16_t f0){
	self->frequency_0 = f0;
	AFSK_Demodulator_reset(self);
}

void AFSK_Demodulator_set_frequency_1(AFSK_Demodulator *self, uint16_t f1){
	self->frequency_1 = f1;
	AFSK_Demodulator_reset(self);
}

void AFSK_Demodulator_set_offset(AFSK_Demodulator *self, float off){
	self->offset = off;
}

void AFSK_Demodulator_set_noise_floor(AFSK_Demodulator *self, float nf){
	self->noise_floor = nf;
}

