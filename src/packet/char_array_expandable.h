// char_array_expandable.h

#ifndef CHAR_ARRAY_EXPANDABLE_H
#define CHAR_ARRAY_EXPANDABLE_H

#include <stdlib.h>

typedef struct {

	unsigned short capacity;
	unsigned short length;

	char *data;

} char_array_expandable;

void char_array_expandable_init(char_array_expandable *self, unsigned short capacity);

void char_array_expandable_put(char_array_expandable *self, char c);

unsigned short char_array_expandable_size(char_array_expandable *self);

void char_array_expandable_clear(char_array_expandable *self);

void char_array_expandable_destroy(char_array_expandable *self);

#endif
