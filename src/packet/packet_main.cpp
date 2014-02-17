// packet_main.cpp

#include <fstream>
#include <iostream>
#include <iomanip>

#include <cctype>

#include <stdlib.h>

#include "packet.h"

#include "../util/Util.cpp"

/**
 * Write the packet length and then the packet to std::out
 */
void print_raw_packet(packet_data pd){

	unsigned short length = pd.byte_sequence.size();
	char *bytes = pd.byte_sequence.data();

	std::cout.put((length>>8)&0xFF);
	std::cout.put(length&0xFF);

	std::cout.write(bytes, pd.byte_sequence.size());
	std::cout.flush();

	std::clog << "Packet Length: " << pd.byte_sequence.size() << std::endl;

}

void print_packet(packet_data pd, bool show_errors_bool, bool raw){

	unsigned short fsc = packet_data_CRCCCITT(pd);

	unsigned short fsc2 = pd.byte_sequence[pd.byte_sequence.size()-2];
	fsc2 <<= 8;
	fsc2 |= pd.byte_sequence[pd.byte_sequence.size()-1]&0xFF;

	if(fsc != fsc2 && !show_errors_bool) return;

	if(raw)
		print_raw_packet(pd);

	else {

		std::cout << std::endl << "Packet Received - " << std::endl;

		if(fsc != fsc2)
			std::cout << "Checksum Error!" << std::endl;
	
		std::cout << "Size: " << pd.byte_sequence.size() << std::endl;
		std::cout << "Destination Address: " << pd.destination_address << std::endl;
		std::cout << "Source Address: " << pd.source_address << std::endl;
		for(int i = 0; i < pd.repeater_addresses.size(); i++) std::cout << "Repeater-" << (i+1) << ": " << pd.repeater_addresses[i] << std::endl;
	
		std::cout << std::endl << "Data: " << std::endl;

		int i = 0;
		while(i < pd.byte_sequence.size()){
			for(int j = 0; j < 20; j++){
				if(i < pd.byte_sequence.size()){
					if(std::isprint(pd.byte_sequence[i])) std::cout << pd.byte_sequence[i];
					else std::cout << '.';
				} else std::cout << ' ';
					i++;
			}
			i -= 20;
			std::cout << "  |  " << std::hex;
			for(int j = 0; j < 20; j++){
				if(i < pd.byte_sequence.size()){
					unsigned char val = pd.byte_sequence[i];
					std::cout << " " << std::setfill('0') << std::setw(2) << int(val);
					i++;
				}
			}
			std::cout << std::dec << std::endl;
		}

		std::cout << std::endl << "FCS: " << std::hex << fsc << std::dec << std::endl;
		std::cout.flush();

	}
}

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string filename = util::getArgument("--filename");

	std::string raw = util::getArgument("--raw");
	bool raw_bool = false;
	if(raw.compare("NULL")){
		if(raw.compare("true") == 0)
			raw_bool = true;
		else if(raw.compare("false") == 0)
			raw_bool = false;
		else {
			std::cerr << "Raw argument invalid" << std::endl;
			return -1;
		}
	}

	std::string sample_rate = util::getArgument("--sample-rate");
	int sample_rate_int = 44100;
	if(sample_rate.compare("NULL"))
		sample_rate_int = atoi(sample_rate.c_str());

	std::string bit_rate = util::getArgument("--bit-rate");
	int bit_rate_int = 1200;
	if(bit_rate.compare("NULL"))
		bit_rate_int = atoi(bit_rate.c_str());

	std::string show_errors = util::getArgument("--show-errors");
	bool show_errors_bool = false;
	if(show_errors.compare("true") == 0)
		show_errors_bool = true;

	std::string noise_floor = util::getArgument("--noise-floor");
	int noise_floor_int = 0;
	if(noise_floor.compare("NULL"))
		noise_floor_int = atoi(noise_floor.c_str());

	std::string offset = util::getArgument("--offset");
	float offset_float = 0.0925;
	if(offset.compare("NULL"))
		offset_float = atof(offset.c_str());

	packet pac(sample_rate_int, bit_rate_int, offset_float, 0, noise_floor_int);

	if(filename.compare("stdin") == 0){
		char c;
		while(std::cin.get(c)){
			if(pac.proccess_byte(c)){
				print_packet(pac.get_last_byte(), show_errors_bool, raw_bool);
			}
		}
	} else {
		std::fstream input_file(filename.c_str(), std::fstream::in | std::fstream::binary);
		if(!input_file.is_open()){
			std::cout << "Error: Couldn't open input file." << std::endl;
			exit(0);
		}
		char wbuf[1];
		while(!input_file.eof()){
			input_file.read(wbuf, 1);
			if(pac.proccess_byte(wbuf[0])){
				print_packet(pac.get_last_byte(), show_errors_bool, raw_bool);
			}
		}
	}

	std::cout << std::endl;

	return 0;

}
