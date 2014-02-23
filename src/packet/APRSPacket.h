#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../crc/crcccitt.h"

#include "../container/char_array.h"

#define APRS_CRC_POLY 0x8408

typedef struct {

	char_array		data;

	signed char		destination_address[8];
	signed char		source_address[8];

	unsigned char	repeaters;
	signed char		**repeater_addresses;

} APRSPacket;

void APRSPacket_from_data(APRSPacket *packet, char_array *data);

unsigned short APRSPacket_crc(APRSPacket *packet);

void APRSPacket_destroy(APRSPacket *packet);

#endif
