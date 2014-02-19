#include "APRSPacket.h"

void APRSPacket_from_data(APRSPacket *packet, char_array *data){

	int i;

	packet->data.len = data->len;
	packet->data.data = data->data;

	for(i = 0; i < 7; i++) packet->destination_address[i] = (char)((unsigned char)(data->data[i])>>1);
	packet->destination_address[8] = '\0';

	for(i = 7; i < 14; i++) packet->source_address[i-7] = (char)((unsigned char)(data->data[i])>>1);
	packet->source_address[8] = '\0';

	packet->repeaters = 0;
	packet->repeater_addresses = (char**) malloc(sizeof(void*)*8);

	int n = 14;
	while(n < data->len){

		if(data->data[n] == 0x03 && (unsigned char)data->data[n+1] == 0xF0) break;

		char* new_address = (char*) malloc(sizeof(char)*9);
		for(i = n; i < n+7; i++) new_address[i-n] = (char)((unsigned char)(data->data[i])>>1);
		n+=7;

		packet->repeater_addresses[packet->repeaters] = new_address;
		packet->repeaters++;

	}

}
