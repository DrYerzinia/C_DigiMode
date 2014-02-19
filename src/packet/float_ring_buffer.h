// float_ring_buffer.h

#ifndef FLOAT_RING_BUFFER_H
#define FLOAT_RING_BUFFER_H

#include <stdlib.h>

typedef struct {

	float *data;

	unsigned short read_position;
	unsigned short write_position;

	unsigned short size;

} float_ring_buffer;

void float_ring_buffer_init(float_ring_buffer *self, unsigned short capacity);

void float_ring_buffer_put(float_ring_buffer *self, float f);
float float_ring_buffer_get(float_ring_buffer *self, unsigned short index);
void float_ring_buffer_pop(float_ring_buffer *self);
void float_ring_buffer_remove(float_ring_buffer *self, unsigned short count);

void float_ring_buffer_clear(float_ring_buffer *self);

unsigned short float_ring_buffer_avail(float_ring_buffer *self);

void float_ring_buffer_destory(float_ring_buffer *self);

#endif
