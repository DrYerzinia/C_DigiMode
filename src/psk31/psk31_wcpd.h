// psk31_wcpd.h

#pragma once

#include "psk31.h"

class psk31_wcpd : public psk31 {

public:

	psk31_wcpd();
	psk31_wcpd(float sample_rate, float bit_rate, float psk31freq);

	~psk31_wcpd();

	/* Returns True if a new byte has been decoded
	*/
	virtual bool proccess_byte(char next_data_point);

protected:

	virtual void init();

};

