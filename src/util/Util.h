// Util.h
#pragma once

#include <stdint.h>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

#include <cmath>

#include <map>

class util {

public:

	static uint8_t readUInt8(std::istream &in);
	static uint16_t readUInt16(std::istream &in);
	static uint32_t readUInt32(std::istream &in);
	static int16_t readInt16(std::istream &in);
	static int32_t readInt32(std::istream &in);

	static void writeUInt8(std::ostream &out, uint8_t);
	static void writeUInt16(std::ostream &out, uint16_t);
	static void writeUInt32(std::ostream &out, uint32_t);
	static void writeInt16(std::ostream &out, int16_t);
	static void writeInt32(std::ostream &out, int32_t);

	static std::string intToStr(int i);
	static std::string timetToStr(float f);
	static std::string getArgument(std::string key);
	static void readInArguments(std::string args);

	static void writeDebugStream(std::string msg);
	static void closeDebugStream();

	static bool verbose;

	static std::ofstream debugfstream;

private:

	static std::map<std::string, std::string> commandlineargs;
};
