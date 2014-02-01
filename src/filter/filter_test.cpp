// filter_test.cpp

#include <iostream>
#include <fstream>

#include <cstring>

#include <stdlib.h>

#include "FIR_filter.h"

#include "../util/Util.h"

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string fn = util::getArgument("-fn");
	std::string filename = "dat/psk/0_5000hz_sweep.raw";
	if(fn.compare("NULL")){
		filename = fn;
	}

	std::string of = util::getArgument("-of");
	std::string output_filename = "dat/data_filtered.raw";
	if(of.compare("NULL")){
		output_filename = of;
	}

	FIR_filter test_filter;

	// filter parameteres ////////////////////////////////////////////////

	std::string ft = util::getArgument("-ft");
	std::string filter_type = "highpass";
	if(filter_type.compare("NULL")){
		filter_type = ft;
	}
	
	int points = 127;
	float low_cutoff_freq = 800;
	float high_cutoff_freq = 900;
	float cutoff_freq = 2500;

	std::string pts = util::getArgument("-pts");
	if(pts.compare("NULL")){
		points = atoi(pts.c_str());
	}
	
	std::string co = util::getArgument("-co");
	if(co.compare("NULL")){
		cutoff_freq = atof(co.c_str());
	}
	
	std::string lco = util::getArgument("-lco");
	if(lco.compare("NULL")){
		low_cutoff_freq = atof(lco.c_str());
	}


	std::string hco = util::getArgument("-hco");
	if(hco.compare("NULL")){
		high_cutoff_freq = atof(hco.c_str());
	}

	test_filter.set_sample_rate(22050);

	// Setup Filter ///////////////////////////////////////////////////////

	if(filter_type.compare("bandpass") == 0){
		test_filter.set_up_bandpass(points, low_cutoff_freq, high_cutoff_freq);
	} else if(filter_type.compare("lowpass") == 0){
		test_filter.set_up_lowpass(points, cutoff_freq);
	} else if(filter_type.compare("highpass") == 0){
		test_filter.set_up_highpass(points, cutoff_freq);
	}  else if(filter_type.compare("bandstop") == 0){
		test_filter.set_up_bandreject(points, low_cutoff_freq, high_cutoff_freq);
	} else if(filter_type.compare("90deg") == 0){
		test_filter.set_up_90deg();
	} else {
		std::cout << "Invalid Filter Type: " << filter_type << std::endl;
		exit(0);
	}

	// Filter Data and Write Data ////////////////////////////////////////

	std::ostream *output_file;
	if(output_filename.compare("stdout") == 0) output_file = &std::cout;
	else output_file = new std::fstream(output_filename.c_str(), std::fstream::out | std::fstream::binary);

	char wbuf[1];
	float max = 0.0001;

	std::istream *data_file;

	if(filename.compare("stdin") == 0){
		data_file = &std::cin;
	} else {
		data_file = new std::fstream(filename.c_str(), std::fstream::in | std::fstream::binary);

		if(!((std::fstream*)data_file)->is_open()){
			std::cout << "Error: couldn't open input file" << std::endl;
			exit(0);
		}
	}

	while(!data_file->eof()){		
		data_file->read(wbuf, 1);
		if(test_filter.proccess_byte(wbuf[0])){
			float dat = test_filter.get_next_datapoint();
			if(max < dat)
				max = dat;
			wbuf[0] = (char) dat*(127/max);
			output_file->write(wbuf, 1);
			output_file->flush();
		}
	}

	if(filename.compare("stdin") != 0) ((std::fstream*)data_file)->close();

	if(output_filename.compare("stdout") != 0){
		((std::fstream*)output_file)->close();
		delete output_file;
	}

	// The End ////////////////////////////////////////////////////////////

	return 0;

}
