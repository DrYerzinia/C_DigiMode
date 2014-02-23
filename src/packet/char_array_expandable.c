// char_array_expandable.c

#include "char_array_expandable.h"

void char_array_expandable_init(char_array_expandable *self, unsigned short capacity){

	self->length = 0;
	self->capacity = capacity;

	self->data = (signed char*) malloc(sizeof(char)*capacity);

}

void char_array_expandable_put(char_array_expandable *self, signed char c){

	if(self->length == self->capacity){

		unsigned char old_capacity = self->capacity;

		self->capacity += self->capacity/5;

		signed char *new_data = (signed char*) malloc(sizeof(char) * self->capacity);
		signed char *old_data = self->data;

		unsigned char i;
		for(i = 0; i < old_capacity; i++)
			new_data[i] = old_data[i];

		free(old_data);

		self->data = new_data;

	}

	self->data[self->length] =  c;

	self->length++;

}

unsigned short char_array_expandable_size(char_array_expandable *self){

	return self->length;

}

void char_array_expandable_clear(char_array_expandable *self){

	self->length = 0;

}

void char_array_expandable_destroy(char_array_expandable *self){

	free(self->data);

}
