// NOAA_ATP.h

#include <list>
#include <vector>

#include <cmath>

#define NOAA_SUBCARRIER_FREQ 2400.0
#define SYNC_A_FREQ			 1040.0
#define SYNC_B_FREQ			  832.0

class NOAA_ATP {

public:

	NOAA_ATP();
	NOAA_ATP(int sr);

	~NOAA_ATP();

	void init();

	bool proccess_byte(char input_data_point);

	void set_sample_rate(int sr);

private:

	int sample_count;
	int input_buffer_start;

	float sample_rate;
	float detection_threshold;

	std::list< std::vector<char> > line_data;

	std::list<char> ef_buffer;
	std::list<char> envelope_follower_out;
	std::list<char> sync_pulse_detect_buf;

	std::list<int> sync_a_positions;
	std::list<int> sync_b_positions;

	std::list<int> sync_a_positions_cnf;
	std::list<int> sync_b_positions_cnf;

};
