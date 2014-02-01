//Util.cpp

#include "Util.h"

uint8_t util::readUInt8(std::istream &in){
	char tmp;
	in.read(&tmp, 1);
	return (uint8_t) tmp;
}

uint16_t util::readUInt16(std::istream &in){
	uint16_t tmp16int = 0;
	unsigned char tmp;
	for(int i = 0; i < 2; i++){
		in >> tmp;
		tmp16int += uint16_t(tmp * pow(0x100, i));
	}
	return tmp16int;
}

uint32_t util::readUInt32(std::istream &in){
        uint32_t tmp32int = 0;
        unsigned char tmp;
        for(int i = 0; i < 4; i++){
                in >> tmp;
                tmp32int += uint32_t(tmp * pow(0x100, i));
        }
	return tmp32int;
}

int16_t util::readInt16(std::istream &in){
	return (int16_t) util::readUInt16(in);
}

int32_t util::readInt32(std::istream &in){
	return (int32_t) util::readUInt32(in);
}

void util::writeUInt8(std::ostream &out, uint8_t wri){
	out.write((char*) &wri, 1);
}

void util::writeUInt16(std::ostream &out, uint16_t wri){
	char *tmp = (char*) &wri;
	for(int i = 0; i < 2; i++){
		out.write((char*) tmp, 1);
		tmp++;
	}
}

void util::writeUInt32(std::ostream &out, uint32_t wri){
	char *tmp = (char*) &wri;
	for(int i = 0; i < 4; i++){
		out.write(tmp, 1);
		tmp++;
	}
}

void util::writeInt16(std::ostream &out, int16_t wri){
	writeUInt16(out, wri);
}

void util::writeInt32(std::ostream &out, int32_t wri){
	writeUInt32(out, wri);
}

std::string util::intToStr(int i){
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string util::getArgument(std::string key){
	std::map<std::string, std::string>::iterator it = commandlineargs.find(key);
	if(it != commandlineargs.end()) return it->second;
	else return std::string("NULL");
}

void util::readInArguments(std::string args){
	std::stringstream ss(args);

	char buffer[100];
	while(!ss.eof()){
		ss.getline(buffer, 100, ' ');
		if(buffer[0] == '-'){
			std::string key(buffer);
			ss.getline(buffer, 100, ' ');
			std::string arg(buffer);
			commandlineargs.insert(std::make_pair(key, arg));
		}
	}
}

void util::writeDebugStream(std::string msg){
	if(debugfstream.is_open()){
		debugfstream << msg;
		debugfstream.flush();
	} else {
		debugfstream.open("debug");
		writeDebugStream(msg);
	}
}

void util::closeDebugStream(){
	if(debugfstream.is_open()) debugfstream.close();
}

std::ofstream util::debugfstream;
std::map<std::string, std::string> util::commandlineargs;
bool util::verbose;
