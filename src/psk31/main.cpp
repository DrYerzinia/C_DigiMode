// main.cpp

#include <iostream>
#include <fstream>

#include <vector>

#include <cstring>

#include <stdlib.h>

#include "psk31_tdtl.h"
#include "psk31_wcpd.h"
#include "psk31_wpdd.h"

#include "../util/Util.h"

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string filename = util::getArgument("--filename");

	std::string sample_rate = util::getArgument("--sample-rate");
	int sample_rate_int = 22050;
	if(sample_rate.compare("NULL")){
		sample_rate_int = atoi(sample_rate.c_str());
	}

	std::string bit_rate = util::getArgument("--bit-rate");
	float bit_rate_int = 31.25;
	if(bit_rate.compare("NULL")){
		bit_rate_int = atof(bit_rate.c_str());
	}

	std::string psk31freq = util::getArgument("--psk31freq");
	int psk31freq_int = 1900;
	if(psk31freq.compare("NULL")){
		psk31freq_int = atoi(psk31freq.c_str());
	}

	std::string demodmode = util::getArgument("--demod-mode");

	psk31 *psk;

	if(demodmode.compare("tdtl") == 0) psk = new psk31_tdtl(sample_rate_int, bit_rate_int, psk31freq_int);
	else if(demodmode.compare("wcpd") == 0) psk = new psk31_wcpd(sample_rate_int, bit_rate_int, psk31freq_int);
	else if(demodmode.compare("wpdd") == 0) psk = new psk31_wpdd(sample_rate_int, bit_rate_int, psk31freq_int);
	else psk = new psk31_tdtl(sample_rate_int, bit_rate_int, psk31freq_int);

	if(filename.compare("stdin") == 0){
		char c;
		while(std::cin.get(c)){
			if(psk->proccess_byte(c)){
				char lb = psk->getLastByte();
				if(lb == '\r') std::cout << '\n';
				std::cout << lb;
				std::cout.flush();
			}
		}

	} else {
		std::fstream input_file(filename.c_str(), std::fstream::in | std::fstream::binary);
		std::vector<char> input_data;
		char buffer[1000];
		while(!input_file.eof()){
			input_file.read(buffer, 1000);
			int count = input_file.gcount();
			for(int i = 0; i < count; i++){
				if(psk->proccess_byte(buffer[i])){
					char lb = psk->getLastByte();
					if(lb == '\r') std::cout << '\n';
					std::cout << lb;
					std::cout.flush();
				}
			}
		}
		input_file.close();
	}


	delete psk;

	std::cout << '\n';

	return 0;

}

