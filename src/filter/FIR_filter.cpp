// FIR_filter.cpp

#include "FIR_filter.h"

FIR_filter::FIR_filter(){
}

FIR_filter::~FIR_filter(){
}

bool FIR_filter::proccess_byte(char byte){

	filter_buffer.push_back(byte);

	if(filter_buffer.size() >= filter_coefficents.size()){

		float result = 0.0f;

		int n = filter_coefficents.size()-1;
		for(std::list<char>::iterator it = filter_buffer.begin(); it != filter_buffer.end(); it++){
			result += (*it)*filter_coefficents[n];
			n--;
		}

		output_buffer.push_back(result);

		filter_buffer.pop_front();

		return true;

	}

	return false;

}

float FIR_filter::get_next_datapoint(){
	float ret = output_buffer.front();
	output_buffer.pop_front();
	return ret;
}

void FIR_filter::set_up_lowpass(int sample_width, float frequency_cutoff){

	filter_width = sample_width;
	sample_width--;
	
	float ncof = (2*PI*frequency_cutoff)/sample_rate;
	
	for(int n = 0; n <= sample_width; n++){
		float hd = 0.0;
		if(n != sample_width/2.0){
			hd = sin(ncof*(n-(sample_width/2.0)))/(PI*(n-(sample_width/2.0)));
		} else {
			hd = ncof/PI;
		}
		filter_coefficents.push_back(hd);
	}

}

void FIR_filter::set_up_highpass(int sample_width, float frequency_cutoff){

	filter_width = sample_width;
	sample_width--;
	
	float ncof = (2*PI*frequency_cutoff)/sample_rate;
	
	for(int n = 0; n <= sample_width; n++){
		float hd = 0.0;
		if(n != sample_width/2.0){
			hd = -1.0f*sin(ncof*(n-(sample_width/2.0)))/(PI*(n-(sample_width/2.0)));
		} else {
			hd = 1.0f-(ncof/PI);
		}
		filter_coefficents.push_back(hd);
	}

}

void FIR_filter::set_up_bandpass(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high){

	filter_width = sample_width;
	sample_width--;

	float ncofh = (2*PI*frequency_cutoff_high)/sample_rate;
	float ncofl = (2*PI*frequency_cutoff_low)/sample_rate;
	
	for(int n = 0; n <= sample_width; n++){
		float hd = 0;
		if(n != sample_width/2.0){
			float p1 = sin((ncofh)*(n-sample_width/2.0))/(PI*(n-sample_width/2.0));
			float p2 = sin((ncofl)*(n-sample_width/2.0))/(PI*(n-sample_width/2.0));
			hd = p1 - p2;
		} else {
			hd = (ncofh-ncofl)/PI;
		}
		filter_coefficents.push_back(hd);
	}

}

void FIR_filter::set_up_bandreject(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high){

	filter_width = sample_width;
	sample_width--;

	float ncofh = (2*PI*frequency_cutoff_high)/sample_rate;
	float ncofl = (2*PI*frequency_cutoff_low)/sample_rate;
	
	for(int n = 0; n <= sample_width; n++){
		float hd = 0;
		if(n != sample_width/2.0){
			float p1 = sin((ncofl)*(n-sample_width/2.0))/(PI*(n-sample_width/2.0));
			float p2 = sin((ncofh)*(n-sample_width/2.0))/(PI*(n-sample_width/2.0));
			hd = p1 - p2;
		} else {
			hd = 1.0f-(ncofl-ncofh)/PI;
		}
		filter_coefficents.push_back(hd);
	}

}

void FIR_filter::set_up_90deg(){

	filter_width = 127;

	filter_coefficents.push_back(2.503121911517990E-6);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.467786344978520E-5);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(7.484699163654970E-5);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.596837912674640E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.864937276857820E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(4.632222226606030E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(6.984697946461290E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.001519849822480E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.382386078574780E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.851888652848380E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.421771503790630E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(3.104877305139560E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(3.915403054026880E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(4.869268379450160E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(5.984642635654640E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(7.282698315603690E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(8.788692244120680E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.053353099178890E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.255606861640420E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.490654256738710E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.765183485812880E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.088376811308900E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.473266379128460E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.939048538621940E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(3.515249980308180E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(4.249738451966580E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(5.225469384343460E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(6.599555525290060E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(8.709303063859860E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(1.243271717911720E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(2.099099626265340E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(6.338084440848620E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-6.338084440848620E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.099099626265330E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.243271717911710E-1);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-8.709303063859750E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-6.599555525290240E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-5.225469384343290E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-4.249738451966730E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-3.515249980308210E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.939048538621830E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.473266379128410E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.088376811308890E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.765183485812880E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.490654256738840E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.255606861640340E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.053353099178780E-2);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-8.788692244120770E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-7.282698315602160E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-5.984642635654830E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-4.869268379450160E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-3.915403054026460E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-3.104877305139610E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.421771503790740E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.851888652847970E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.382386078574310E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.001519849822290E-3);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-6.984697946463930E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-4.632222226607710E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.864937276857430E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-1.596837912673990E-4);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-7.484699163655550E-5);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.467786344977910E-5);
	filter_coefficents.push_back(0.000000000000000E0);
	filter_coefficents.push_back(-2.503121911518370E-6);

}

void FIR_filter::set_up_notch(int sample_width, float frequency_cutoff){
}

