// psk31.cpp

#include "psk31.h"

psk31::psk31(){

}

psk31::psk31(float sample_rate, float bit_rate, float psk31freq){

}

psk31::~psk31(){

}

char psk31::getLastByte(){

	return byte_sequence[byte_sequence.size()-1];

}

std::vector<char> psk31::getDecodedData(){

	return byte_sequence;

}

void psk31::set_sample_rate(float sample_rate){

	this->sample_rate = sample_rate;

	ninty_deg_phase_width = (sample_rate/psk31freq)/4.0;

	bitwidth = sample_rate/bit_rate;

	if(input_data != NULL) delete input_data;

	input_data_width = (int)(bitwidth+ninty_deg_phase_width);
	input_data = new char[input_data_width];

	input_position = 0;

}

void psk31::set_bit_rate(float bit_rate){

	this->bit_rate = bit_rate;

	bitwidth = sample_rate/bit_rate;

	if(input_data != NULL) delete input_data;

	input_data_width = (int)(bitwidth+ninty_deg_phase_width);
	input_data = new char[input_data_width];

	input_position = 0;

}

void psk31::set_psk31freq(float psk31freq){

	this->psk31freq = psk31freq;

	ninty_deg_phase_width = (sample_rate/psk31freq)/4.0;

	if(input_data != NULL) delete input_data;

	input_data_width = (int)(bitwidth+ninty_deg_phase_width);
	input_data = new char[input_data_width];

	input_position = 0;

}

void psk31::set_threshhold_value(float threshhold_value){

	this->threshhold_value = threshhold_value;

}

char psk31::getVaricode(int code){

	std::map<int, char>::iterator it = varicode.find(code);
	if(it != varicode.end()) return it->second;
	return '\0';

}

void psk31::initVaricode(){

	varicode.insert(std::make_pair(0x02AB,	'\0'));		// NULL						0b1010101011
	varicode.insert(std::make_pair(0x02DB,	'S'));		// Start of Header			0b1011011011
	varicode.insert(std::make_pair(0x02ED,	'S'));		// Start of Text			0b1011101101
	varicode.insert(std::make_pair(0x0377,	'E'));		// End of Text				0b1101110111
	varicode.insert(std::make_pair(0x02EB,	'E'));		// End of Transmission		0b1011101011
	varicode.insert(std::make_pair(0x035F,	'E'));		// Enquiry					0b1101011111
	varicode.insert(std::make_pair(0x02EF,	'A'));		// Acknowledgement			0b1011101111
	varicode.insert(std::make_pair(0x02FD,	'B'));		// Bell						0b1011111101
	varicode.insert(std::make_pair(0x02FF,	0x08));		// Backspace				0b1011111111
	varicode.insert(std::make_pair(0x00EF,	'\t'));		// Horizontal Tab			0b11101111
	varicode.insert(std::make_pair(0x001D,	'\n'));		// Line feed				0b11101
	varicode.insert(std::make_pair(0x036F,	'\v'));		// Vertical Tab				0b1101101111
	varicode.insert(std::make_pair(0x02DD,	'\f'));		// Form feed				0b1011011101
	varicode.insert(std::make_pair(0x001F,	'\r'));		// Carriage return			0b11111
	varicode.insert(std::make_pair(0x0375,	'S'));		// Shift Out				0b1101110101
	varicode.insert(std::make_pair(0x03AB,	'S'));		// Shift In					0b1110101011
	varicode.insert(std::make_pair(0x02F7,	'D'));		// Data Link Escape			0b1011110111
	varicode.insert(std::make_pair(0x02F5,	'D'));		// Device Control 1 (XON)	0b1011110101
	varicode.insert(std::make_pair(0x03AD,	'D'));		// Device Control 2			0b1110101101
	varicode.insert(std::make_pair(0x03AF,	'D'));		// Device Control 3 (XOFF)	0b1110101111
	varicode.insert(std::make_pair(0x035B,	'D'));		// Device Control 4			0b1101011011
	varicode.insert(std::make_pair(0x036B,	'N'));		// Negative Acknowledgement	0b1101101011
	varicode.insert(std::make_pair(0x036D,	'S'));		// Sychronus Idle			0b1101101101
	varicode.insert(std::make_pair(0x0357,	'E'));		// End of Trans. Block		0b1101010111
	varicode.insert(std::make_pair(0x037B,	'C'));		// Cancel					0b1101111011
	varicode.insert(std::make_pair(0x037D,	'E'));		// End of Medium			0b1101111101
	varicode.insert(std::make_pair(0x03B7,	'S'));		// Substitute				0b1110110111
	varicode.insert(std::make_pair(0x0355,	'E'));		// Escape					0b1101010101
	varicode.insert(std::make_pair(0x035D,	'F'));		// File Seperator			0b1101011101
	varicode.insert(std::make_pair(0x03BB,	'G'));		// Group Seperator			0b1110111011
	varicode.insert(std::make_pair(0x02FB,	'R'));		// Record Separator			0b1011111011
	varicode.insert(std::make_pair(0x037F,	'U'));		// Unit Separator			0b1101111111
	varicode.insert(std::make_pair(0x03B5,	0x7F));		// Delete					0b1110110101
	varicode.insert(std::make_pair(0x0001,	' '));		//							0b1
	varicode.insert(std::make_pair(0x01FF,	'!'));		//							0b111111111
	varicode.insert(std::make_pair(0x015F,	'\"'));		//							0b101011111
	varicode.insert(std::make_pair(0x01F5,	'/'));		//							0b111110101
	varicode.insert(std::make_pair(0x01DB,	'$'));		//							0b111011011
	varicode.insert(std::make_pair(0x02D5,	'%'));		//							0b1011010101
	varicode.insert(std::make_pair(0x02BB,	'&'));		//							0b1010111011
	varicode.insert(std::make_pair(0x017F,	'\''));		//							0b101111111
	varicode.insert(std::make_pair(0x00FB,	'('));		//							0b11111011
	varicode.insert(std::make_pair(0x00F7,	')'));		//							0b11110111
	varicode.insert(std::make_pair(0x016F,	'*'));		//							0b101101111
	varicode.insert(std::make_pair(0x01DF,	'+'));		//							0b111011111
	varicode.insert(std::make_pair(0x0075,	','));		//							0b1110101
	varicode.insert(std::make_pair(0x0035,	'-'));		//							0b110101
	varicode.insert(std::make_pair(0x0057,	'.'));		//							0b1010111
	varicode.insert(std::make_pair(0x01AF,	'/'));		//							0b110101111
	varicode.insert(std::make_pair(0x00B7,	'0'));		//							0b10110111
	varicode.insert(std::make_pair(0x00BD,	'1'));		//							0b10111101
	varicode.insert(std::make_pair(0x00ED,	'2'));		//							0b11101101
	varicode.insert(std::make_pair(0x00FF,	'3'));		//							0b11111111
	varicode.insert(std::make_pair(0x0177,	'4'));		//							0b101110111
	varicode.insert(std::make_pair(0x015B,	'5'));		//							0b101011011
	varicode.insert(std::make_pair(0x016B,	'6'));		//							0b101101011
	varicode.insert(std::make_pair(0x01AD,	'7'));		//							0b110101101
	varicode.insert(std::make_pair(0x01AB,	'8'));		//							0b110101011
	varicode.insert(std::make_pair(0x01B7,	'9'));		//							0b110110111
	varicode.insert(std::make_pair(0x00F5,	':'));		//							0b11110101
	varicode.insert(std::make_pair(0x01BD,	';'));		//							0b110111101
	varicode.insert(std::make_pair(0x01ED,	'<'));		//							0b111101101
	varicode.insert(std::make_pair(0x0055,	'='));		//							0b1010101
	varicode.insert(std::make_pair(0x01D7,	'>'));		//							0b111010111
	varicode.insert(std::make_pair(0x02AF,	'?'));		//							0b1010101111
	varicode.insert(std::make_pair(0x02BD,	'@'));		//							0b1010111101
	varicode.insert(std::make_pair(0x007D,	'A'));		//							0b1111101
	varicode.insert(std::make_pair(0x00EB,	'B'));		//							0b11101011
	varicode.insert(std::make_pair(0x00AD,	'C'));		//							0b10101101
	varicode.insert(std::make_pair(0x00B5,	'D'));		//							0b10110101
	varicode.insert(std::make_pair(0x0077,	'E'));		//							0b1110111
	varicode.insert(std::make_pair(0x00DB,	'F'));		//							0b11011011
	varicode.insert(std::make_pair(0x00FD,	'G'));		//							0b11111101
	varicode.insert(std::make_pair(0x0155,	'H'));		//							0b101010101
	varicode.insert(std::make_pair(0x007F,	'I'));		//							0b1111111
	varicode.insert(std::make_pair(0x01FD,	'J'));		//							0b111111101
	varicode.insert(std::make_pair(0x017D,	'K'));		//							0b101111101
	varicode.insert(std::make_pair(0x00D7,	'L'));		//							0b11010111
	varicode.insert(std::make_pair(0x00BB,	'M'));		//							0b10111011
	varicode.insert(std::make_pair(0x00DD,	'N'));		//							0b11011101
	varicode.insert(std::make_pair(0x00AB,	'O'));		//							0b10101011
	varicode.insert(std::make_pair(0x00D5,	'P'));		//							0b11010101
	varicode.insert(std::make_pair(0x01DD,	'Q'));		//							0b111011101
	varicode.insert(std::make_pair(0x00AF,	'R'));		//							0b10101111
	varicode.insert(std::make_pair(0x006F,	'S'));		//							0b1101111
	varicode.insert(std::make_pair(0x006D,	'T'));		//							0b1101101
	varicode.insert(std::make_pair(0x0157,	'U'));		//							0b101010111
	varicode.insert(std::make_pair(0x01B5,	'V'));		//							0b110110101
	varicode.insert(std::make_pair(0x015D,	'W'));		//							0b101011101
	varicode.insert(std::make_pair(0x0175,	'X'));		//							0b101110101
	varicode.insert(std::make_pair(0x017B,	'Y'));		//							0b101111011
	varicode.insert(std::make_pair(0x02AD,	'Z'));		//							0b1010101101
	varicode.insert(std::make_pair(0x01F7,	'['));		//							0b111110111
	varicode.insert(std::make_pair(0x01EF,	'\\'));		//							0b111101111
	varicode.insert(std::make_pair(0x01FB,	']'));		//							0b111111011
	varicode.insert(std::make_pair(0x02BF,	'^'));		//							0b1010111111
	varicode.insert(std::make_pair(0x016D,	'_'));		//							0b101101101
	varicode.insert(std::make_pair(0x02DF,	'`'));		//							0b1011011111
	varicode.insert(std::make_pair(0x000B,	'a'));		//							0b1011
	varicode.insert(std::make_pair(0x005F,	'b'));		//							0b1011111
	varicode.insert(std::make_pair(0x002F,	'c'));		//							0b101111
	varicode.insert(std::make_pair(0x002D,	'd'));		//							0b101101
	varicode.insert(std::make_pair(0x0003,	'e'));		//							0b11
	varicode.insert(std::make_pair(0x003D,	'f'));		//							0b111101
	varicode.insert(std::make_pair(0x005B,	'g'));		//							0b1011011
	varicode.insert(std::make_pair(0x002B,	'h'));		//							0b101011
	varicode.insert(std::make_pair(0x000D,	'i'));		//							0b1101
	varicode.insert(std::make_pair(0x01EB,	'j'));		//							0b111101011
	varicode.insert(std::make_pair(0x00BF,	'k'));		//							0b10111111
	varicode.insert(std::make_pair(0x001B,	'l'));		//							0b11011
	varicode.insert(std::make_pair(0x003B,	'm'));		//							0b111011
	varicode.insert(std::make_pair(0x000F,	'n'));		//							0b1111
	varicode.insert(std::make_pair(0x0007,	'o'));		//							0b111
	varicode.insert(std::make_pair(0x003F,	'p'));		//							0b111111
	varicode.insert(std::make_pair(0x01BF,	'q'));		//							0b110111111
	varicode.insert(std::make_pair(0x0015,	'r'));		//							0b10101
	varicode.insert(std::make_pair(0x0017,	's'));		//							0b10111
	varicode.insert(std::make_pair(0x0005,	't'));		//							0b101
	varicode.insert(std::make_pair(0x0037,	'u'));		//							0b110111
	varicode.insert(std::make_pair(0x007B,	'v'));		//							0b1111011
	varicode.insert(std::make_pair(0x006B,	'w'));		//							0b1101011
	varicode.insert(std::make_pair(0x00DF,	'x'));		//							0b11011111
	varicode.insert(std::make_pair(0x005D,	'y'));		//							0b1011101
	varicode.insert(std::make_pair(0x01D5,	'z'));		//							0b111010101
	varicode.insert(std::make_pair(0x02B7,	'{'));		//							0b1010110111
	varicode.insert(std::make_pair(0x01BB,	'|'));		//							0b110111011
	varicode.insert(std::make_pair(0x02B5,	'}'));		//							0b1010110101
	varicode.insert(std::make_pair(0x02D7,	'~'));		//							0b1011010111
}


