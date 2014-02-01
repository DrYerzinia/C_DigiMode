// mixer.cpp

#include <iostream>
#include <fstream>

#include <vector>

#include <cmath>

#include <cstring>

#include <stdlib.h>

#include "../util/Util.h"

#define PI 3.14159265359

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string fn1 = util::getArgument("-fn1");
	std::string filename1 = "dat/440_Hz_Cut.raw";
	if(fn1.compare("NULL")){
		filename1 = fn1;
	}

	std::string fn2 = util::getArgument("-fn2");
	std::string filename2 = "dat/440_Hz_90.raw";
	if(fn2.compare("NULL")){
		filename2 = fn2;
	}
	
	std::string of = util::getArgument("-of");
	std::string output_filename = "dat/data_mixed.raw";
	if(of.compare("NULL")){
		output_filename = of;
	}

	float lo_freq = 1500;

	std::string lof = util::getArgument("-lof");
	if(lof.compare("NULL")){
		lo_freq = atoi(lof.c_str());
	}
	
	std::ostream *output_file;
	if(output_filename.compare("stdout") == 0) output_file = &std::cout;
	else output_file = new std::fstream(output_filename.c_str(), std::fstream::out | std::fstream::binary);

	char wbuf[1];
	float max = 0.0001;

	std::istream *data_file1;
	std::istream *data_file2;

	data_file1 = new std::fstream(filename1.c_str(), std::fstream::in | std::fstream::binary);
	data_file2 = new std::fstream(filename2.c_str(), std::fstream::in | std::fstream::binary);

	if(!((std::fstream*)data_file1)->is_open()){
		std::cout << "Error: couldn't open input file 1" << std::endl;
		exit(0);
	}

	if(!((std::fstream*)data_file2)->is_open()){
		std::cout << "Error: couldn't open input file 2" << std::endl;
		exit(0);
	}

	std::vector<char> dat1;
	std::vector<char> dat2;
	
	while(!data_file1->eof()){		
		data_file1->read(wbuf, 1);
		dat1.push_back(wbuf[0]);
	}
	while(!data_file2->eof()){		
		data_file2->read(wbuf, 1);
		dat2.push_back(wbuf[0]);
	}

	((std::fstream*)data_file1)->close();
	((std::fstream*)data_file2)->close();

	delete data_file1;
	delete data_file2;
	
	if(dat1.size() != dat2.size()){
		std::cout << "Error: input file sizes do not match!" << std::endl;
		exit(0);
	}
	
	std::vector<float> dato;
	
	float phi = 0;
	
	for(int i = 0; i < dat1.size(); i++){
		float d = (((float)dat1[i])*std::sin(phi)) + (((float)dat2[i])*std::sin(phi-(PI/2.0)));
		dato.push_back(d);
		if(std::abs(d) > max) max = std::abs(d);
		
		phi += 2.0*PI*lo_freq*(1.0/22050.0);
		
	}
	
	for(std::vector<float>::iterator it = dato.begin(); it != dato.end(); it++){
		wbuf[0] = (char) ((*it)*(127/max));
		output_file->write(wbuf, 1);
		output_file->flush();
	}

	
	((std::fstream*)output_file)->close();
	delete output_file;

	return 0;

}
