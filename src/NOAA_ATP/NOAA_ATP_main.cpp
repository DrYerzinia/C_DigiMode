// NOAA_ATP_main.cpp

#include <iostream>
#include <fstream>

#include <stdlib.h>

#include "NOAA_ATP.h"

#include "../util/Util.h"

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string filename = util::getArgument("--filename");

	std::string sample_rate = util::getArgument("--sample-rate");
	int sample_rate_int = 11025;
	if(sample_rate.compare("NULL")){
		sample_rate_int = atoi(sample_rate.c_str());
	}

	std::string bit_rate = util::getArgument("--bit-rate");
	int bit_rate_int = 1200;
	if(bit_rate.compare("NULL")){
		bit_rate_int = atoi(bit_rate.c_str());
	}

	NOAA_ATP atp(sample_rate_int);

	if(filename.compare("stdin") == 0){
		char c;
		while(std::cin.get(c)){
			if(atp.proccess_byte(c)){
				//
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
			if(atp.proccess_byte(wbuf[0])){
				//
			}
		}
	}	
	return 0;

}
