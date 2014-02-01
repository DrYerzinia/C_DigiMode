// FIR_filter.h

#include "filter.h"

class FIR_filter : public filter {

public:

	FIR_filter();

	virtual ~FIR_filter();

	virtual bool proccess_byte(char byte);

	virtual float get_next_datapoint();

	virtual void set_up_lowpass(int sample_width, float frequency_cutoff);
	virtual void set_up_highpass(int sample_width, float frequency_cutoff);
	virtual void set_up_bandpass(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high);
	virtual void set_up_bandreject(int sample_width, float frequency_cutoff_low, float frequency_cutoff_high);
	virtual void set_up_90deg();
	virtual void set_up_notch(int sample_width, float frequency_cutoff);

protected:

};
