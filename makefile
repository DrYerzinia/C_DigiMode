all: bin/psk31 bin/filter_test bin/packet bin/sound_device bin/NOAA_ATP bin/mixer bin/forward

bin/forward: build/obj/forward.o
	gcc -O3 -o bin/forward build/obj/forward.o

bin/mixer: build/obj/mixer.o build/obj/Util.o
	g++ -O3 -o bin/mixer build/obj/mixer.o build/obj/Util.o

bin/psk31: build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o
	g++ -O3 -o bin/psk31 build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o

bin/packet:  build/obj/APRSPacket.o build/obj/packet.o build/obj/packet_main.o
	g++ -O3 -o bin/packet build/obj/APRSPacket.o build/obj/packet.o build/obj/packet_main.o

bin/filter_test: build/obj/filter_test.o build/obj/filter.o build/obj/FIR_filter.o build/obj/Util.o
	g++ -O3 -o bin/filter_test build/obj/filter_test.o build/obj/filter.o build/obj/FIR_filter.o build/obj/Util.o

bin/sound_device: build/obj/sound_device.o build/obj/Util.o
	g++ -O3 -o bin/sound_device build/obj/sound_device.o build/obj/Util.o -lpulse -lpulse-simple

bin/NOAA_ATP: build/obj/NOAA_ATP_main.o build/obj/NOAA_ATP.o build/obj/Util.o
	g++ -O3 -o bin/NOAA_ATP build/obj/NOAA_ATP_main.o build/obj/NOAA_ATP.o build/obj/Util.o

build/obj/forward.o: src/net/forward.c
	gcc -O3 -o build/obj/forward.o -c src/net/forward.c

build/obj/mixer.o: src/mixer/mixer.cpp
	g++ -O3 -o build/obj/mixer.o -c src/mixer/mixer.cpp
	
build/obj/NOAA_ATP_main.o: src/NOAA_ATP/NOAA_ATP_main.cpp
	g++ -O3 -o build/obj/NOAA_ATP_main.o -c src/NOAA_ATP/NOAA_ATP_main.cpp

build/obj/NOAA_ATP.o: src/NOAA_ATP/NOAA_ATP.cpp
	g++ -O3 -o  build/obj/NOAA_ATP.o -c src/NOAA_ATP/NOAA_ATP.cpp

build/obj/sound_device.o: src/sound_device/sound_device.cpp
	g++ -O3 -o build/obj/sound_device.o -c src/sound_device/sound_device.cpp

build/obj/packet.o: src/packet/packet.cpp
	g++ -O3 -o build/obj/packet.o -c src/packet/packet.cpp

build/obj/APRSPacket.o: src/packet/APRSPacket.c
	g++ -O3 -o build/obj/APRSPacket.o -c src/packet/APRSPacket.c

build/obj/packet_main.o: src/packet/packet_main.cpp
	g++ -O3 -o build/obj/packet_main.o -c src/packet/packet_main.cpp

build/obj/filter_test.o: src/filter/filter_test.cpp
	g++ -O3 -o build/obj/filter_test.o -c src/filter/filter_test.cpp

build/obj/main.o: src/psk31/main.cpp
	g++ -O3 -o build/obj/main.o -c src/psk31/main.cpp

build/obj/psk31.o: src/psk31/psk31.cpp
	g++ -O3 -o build/obj/psk31.o -c src/psk31/psk31.cpp

build/obj/psk31_tdtl.o: src/psk31/psk31_tdtl.cpp
	g++ -O3 -o build/obj/psk31_tdtl.o -c src/psk31/psk31_tdtl.cpp

build/obj/psk31_wcpd.o: src/psk31/psk31_wcpd.cpp
	g++ -O3 -o build/obj/psk31_wcpd.o -c src/psk31/psk31_wcpd.cpp

build/obj/psk31_wpdd.o: src/psk31/psk31_wpdd.cpp
	g++ -O3 -o build/obj/psk31_wpdd.o -c src/psk31/psk31_wpdd.cpp

build/obj/filter.o: src/filter/filter.cpp
	g++ -O3 -o build/obj/filter.o -c src/filter/filter.cpp

build/obj/FIR_filter.o: src/filter/FIR_filter.cpp
	g++ -O3 -o build/obj/FIR_filter.o -c src/filter/FIR_filter.cpp

build/obj/Util.o: src/util/Util.cpp
	g++ -O3 -o build/obj/Util.o -c src/util/Util.cpp

clean:
	rm -rf bin/*
	rm -rf build/obj/*

