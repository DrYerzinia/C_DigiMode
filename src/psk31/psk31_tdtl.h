// psk31.h

#pragma once

#include "psk31.h"

class psk31_tdtl : public psk31 {

public:

	psk31_tdtl();
	psk31_tdtl(float sample_rate, float bit_rate, float psk31freq);

	~psk31_tdtl();

	/* Returns True if a new byte has been decoded
	*/
	virtual bool proccess_byte(char next_data_point);

private:

	virtual void init();

};

