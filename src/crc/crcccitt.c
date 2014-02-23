// crcccitt.c

#include "crcccitt.h"

unsigned short CRCCCITT(uint8_t *data, uint16_t len, uint16_t polynomial){

	uint16_t crc = 0xFFFF;
	uint8_t i, j;

	while(len--){

		crc ^= *data++;

		int j;
		for(j = 0; j < 8; j++){
			if(crc & 0x0001)
				crc = (crc >> 1) ^ polynomial;
			else
				crc >>= 1;
		}

	}

	crc ^= 0xFFFF;

	return crc;

}
