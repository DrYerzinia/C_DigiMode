// filter.h

#include <vector>
#include <list>

#include <cmath>

#ifndef PI
#define PI 3.14159265
#endif

class filter {

public:

	filter();

	virtual ~filter();

	virtual bool proccess_byte(char byte) = 0;

	virtual float get_next_datapoint() = 0;

	virtual void set_up_lowpass(int sample_width, float frequency_cutoff) = 0;
	virtual void set_up_highpass(int sample_width, float frequency_cutoff) = 0;
	virtual void set_up_bandpass(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high) = 0;
	virtual void set_up_bandreject(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high) = 0;
	virtual void set_up_90deg() = 0;
	virtual void set_up_notch(int sample_width, float frequency_cutoff) = 0;

	void set_sample_rate(float sr);

protected:

	int filter_width;
	float sample_rate;

	std::list<char> filter_buffer;
	std::vector<float> filter_coefficents;

	std::list<float> output_buffer;

};
