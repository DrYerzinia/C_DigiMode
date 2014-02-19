// float_ring_buffer.c

#include "float_ring_buffer.h"

void float_ring_buffer_init(float_ring_buffer *self, unsigned short capacity){

	self->size = capacity;
	self->data = (float*) malloc(sizeof(float) * capacity);

	self->read_position = 0;
	self->write_position = 0;

}

void float_ring_buffer_put(float_ring_buffer *self, float f){

	self->data[self->write_position++] = f;

	if(self->write_position >= self->size)
		self->write_position = 0;

}

float float_ring_buffer_get(float_ring_buffer *self, unsigned short index){

	unsigned short position = self->read_position + index;

	if(position >= self->size)
		position -= self->size;

	return self->data[position];

}

void float_ring_buffer_pop(float_ring_buffer *self){

	self->read_position++;
	if(self->read_position >= self->size)
		self->read_position = 0;

}

void float_ring_buffer_remove(float_ring_buffer *self, unsigned short count){

	self->read_position += count;

	if(self->read_position >= self->size)
		self->read_position -= self->size;

}

void float_ring_buffer_clear(float_ring_buffer *self){

	self->read_position = 0;
	self->write_position = 0;

}

unsigned short float_ring_buffer_avail(float_ring_buffer *self){

	if(self->read_position < self->write_position)
		return self->write_position - self->read_position;

	return self->write_position + self->size - self->read_position;

}

void float_ring_buffer_destory(float_ring_buffer *self){

	free(self->data);

}
