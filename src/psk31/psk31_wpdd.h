// psk31_wpdd.h

#pragma once

#include "psk31.h"

class psk31_wpdd : public psk31 {

public:

	psk31_wpdd();
	psk31_wpdd(float sample_rate, float bit_rate, float psk31freq);

	~psk31_wpdd();

	/* Returns True if a new byte has been decoded
	*/
	virtual bool proccess_byte(char next_data_point);

private:

	virtual void init();

};

