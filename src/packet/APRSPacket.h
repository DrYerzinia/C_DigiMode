#ifndef APRSPACKET_H
#define APRSPACKET_H

#include "char_array.h"

#include <stdlib.h>

typedef struct {

	char_array		data;

	char			destination_address[9];
	char			source_address[9];

	unsigned char	repeaters;
	char			**repeater_addresses;

} APRSPacket;

void APRSPacket_from_data(APRSPacket *packet, char_array *data);

void APRSPacket_destory(APRSPacket *packet);

#endif
