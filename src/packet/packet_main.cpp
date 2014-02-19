// packet_main.cpp

#include <stdlib.h>

#include <stddef.h>

#include <ctype.h>
#include <stdio.h>

#include <argp.h>

#include "packet.h"

// Command Line Parameters
const char *program_version = "packet 0.7";
const char *program_bug_address = "<dryerzinia@gmail.com>";

static char doc[] = "packet -- A program for demodulating AFSK encoded APRS packets";

static char args_doc[] = "";

static struct argp_option options[] = {
		{"filename", 'i', "FILE", OPTION_ARG_OPTIONAL, "File to read samples from (stdin)"},
		{"sample-rate", 's', 0, OPTION_ARG_OPTIONAL, "Sample Rate of input (44100)"},
		{"bit-rate", 'b', 0, OPTION_ARG_OPTIONAL, "Bit Rate of packets (1200)"},
		{"show-errors", 'e', 0, OPTION_ARG_OPTIONAL, "Show packets with bad checksums"},
		{"raw", 'r', 0, OPTION_ARG_OPTIONAL, "Print packets as raw data with unsigned short length before them"},
		{"noise-floor", 'n', 0, OPTION_ARG_OPTIONAL, "Noise floor (Not properly implemented yet)"},
		{"offset", 'o', 0, OPTION_ARG_OPTIONAL, "Float offset to multiply average amplitude of the Fouriour Coefficent difference by (0.0925)"},
		{0}
};

struct arguments {
	char *filename;
	int sample_rate;
	int bit_rate;
	bool show_errors;
	bool raw;
	int noise_floor;
	float offset;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state){

	// remove cast in full C
	struct arguments *arguments = ((struct arguments*)(state->input));

	switch(key){
		case 'i':
			arguments->filename = arg;
			break;
		case 's':
			arguments->sample_rate = atoi(arg);
			break;
		case 'b':
			arguments->bit_rate = atoi(arg);
			break;
		case 'e':
			arguments->show_errors = true;
			break;
		case 'r':
			arguments->raw = true;
			break;
		case 'n':
			arguments->noise_floor = atoi(arg);
			break;
		case 'o':
			arguments->offset = atof(arg);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

/**
 * Write the packet length and then the packet to stdout
 */
void print_raw_packet(APRSPacket &packet){

	unsigned short length = packet.data.len;

	fputc((length>>8) & 0xFF, stdout);
	fputc( length     & 0xFF, stdout);

	fwrite(packet.data.data, sizeof(char), packet.data.len, stdout);
	fflush(stdout);

	fprintf(stderr, "packet: Packet Length: %d\n", packet.data.len);

}

void print_detailed_packet(APRSPacket &packet, unsigned short calculated_checksum, unsigned short checksum){

	printf("Packet Received\n");

	if(calculated_checksum != checksum)
		printf("Checksum Error!\nCaluclated checksum is: %04X\n", calculated_checksum);

	printf("Size: %d\n", packet.data.len);
	printf("Destination Address: %s\n", packet.destination_address);
	printf("Source Address: %s\n", packet.source_address);
	for(int i = 0; i < packet.repeaters; i++)
		printf("Repeater-%d: %s\n", i+1, packet.repeater_addresses[i]);

	printf("\nData: \n");

	int i = 0;
	while(i < packet.data.len){
		for(int j = 0; j < 20; j++){
			if(i < packet.data.len){
				if(isprint(packet.data.data[i])) putchar(packet.data.data[i]);
				else printf(".");
			} else putchar(' ');
			i++;
		}
		i -= 20;
		printf("  |  ");
		for(int j = 0; j < 20; j++){
			if(i < packet.data.len){
				unsigned char val = packet.data.data[i];
				printf(" %02X", (int)val);
				i++;
			}
		}
		putchar('\n');
	}

	printf("\nChecksum: %04X\n", checksum);
	fflush(stdout);

}

void print_packet(APRSPacket &packet, bool show_errors_bool, bool raw){

	unsigned short fsc = CRCCCITT(packet.data);

	unsigned short fsc2 = packet.data.data[packet.data.len-2];
	fsc2 <<= 8;
	fsc2 |= packet.data.data[packet.data.len-1]&0xFF;

	if(fsc != fsc2 && !show_errors_bool) return;

	if(raw)
		print_raw_packet(packet);

	else
		print_detailed_packet(packet, fsc, fsc2);

}

int main(int argc, char **argv){

	struct arguments arguments;

	arguments.filename = NULL;
	arguments.sample_rate = 44100;
	arguments.bit_rate = 1200;
	arguments.show_errors = false;
	arguments.raw = false;
	arguments.offset = 0.0925;
	arguments.noise_floor = 0;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	packet pac(arguments.sample_rate, arguments.bit_rate, arguments.offset, 0, arguments.noise_floor);

	FILE *input_file = NULL;

	if(arguments.filename == NULL)
		input_file = stdin;

	else
		input_file = fopen(arguments.filename, "r");

	// Read and process data from file
	if(input_file != NULL){

		char byte;
		while(!feof(input_file)){

			byte = getc(input_file);

			char_array *data = pac.proccess_byte(byte);

			if(data){

				APRSPacket new_packet;
				APRSPacket_from_data(&new_packet, data);
				print_packet(new_packet, arguments.show_errors, arguments.raw);

			}

		}

		fclose(input_file);

	}

	// File coulden't open
	else {

		fprintf(stderr, "Couldn't open input file\n");
		return -1;

	}

	putchar('\n');

	return 0;

}
