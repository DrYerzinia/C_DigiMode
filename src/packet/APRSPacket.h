#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <stdlib.h>

#include "../crc/crcccitt.h"

#include "../container/char_array.h"

typedef struct {

	char_array		data;

	char			destination_address[9];
	char			source_address[9];

	unsigned char	repeaters;
	char			**repeater_addresses;

} APRSPacket;

void APRSPacket_from_data(APRSPacket *packet, char_array *data);

unsigned short APRSPacket_crc(APRSPacket *packet);

void APRSPacket_destory(APRSPacket *packet);

#endif
