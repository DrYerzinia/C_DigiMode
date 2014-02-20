CC=gcc
CFLAGS=-O3

all: bin/psk31 bin/filter_test bin/packet bin/sound_device bin/NOAA_ATP bin/mixer bin/forward

bin/forward: build/obj/forward.o
	$(CC) -O3 -o bin/forward build/obj/forward.o

bin/mixer: build/obj/mixer.o build/obj/Util.o
	g++ -O3 -o bin/mixer build/obj/mixer.o build/obj/Util.o

bin/psk31: build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o
	g++ -O3 -o bin/psk31 build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o

# Packet

bin/packet: build/obj/char_array_expandable.o build/obj/float_ring_buffer.o build/obj/char_ring_buffer.o build/obj/APRSPacket.o build/obj/AFSK_Demodulator.o build/obj/packet_main.o build/obj/crcccitt.o build/obj/trig_table.o
	$(CC) $(CFLAGS) -o bin/packet build/obj/char_array_expandable.o build/obj/float_ring_buffer.o build/obj/char_ring_buffer.o build/obj/APRSPacket.o build/obj/AFSK_Demodulator.o build/obj/packet_main.o build/obj/crcccitt.o build/obj/trig_table.o

build/obj/packet_main.o: src/packet/packet_main.c
	$(CC) $(CFLAGS) -o build/obj/packet_main.o -c src/packet/packet_main.c

build/obj/AFSK_Demodulator.o: src/packet/AFSK_Demodulator.c
	$(CC) $(CFLAGS) -o build/obj/AFSK_Demodulator.o -c src/packet/AFSK_Demodulator.c

build/obj/APRSPacket.o: src/packet/APRSPacket.c
	$(CC) $(CFLAGS) -o build/obj/APRSPacket.o -c src/packet/APRSPacket.c

build/obj/char_ring_buffer.o: src/packet/char_ring_buffer.c
	$(CC) $(CFLAGS) -o build/obj/char_ring_buffer.o -c src/packet/char_ring_buffer.c

build/obj/float_ring_buffer.o: src/packet/float_ring_buffer.c
	$(CC) $(CFLAGS) -o build/obj/float_ring_buffer.o -c src/packet/float_ring_buffer.c

build/obj/char_array_expandable.o: src/packet/char_array_expandable.c
	$(CC) $(CFLAGS) -o build/obj/char_array_expandable.o -c src/packet/char_array_expandable.c

# CRC

build/obj/crcccitt.o: src/crc/crcccitt.c
	$(CC) $(CFLAGS) -o build/obj/crcccitt.o -c src/crc/crcccitt.c

# Math

build/obj/trig_table.o: src/math/trig_table.c
	$(CC) $(CFLAGS) -o build/obj/trig_table.o -c src/math/trig_table.c

# Other


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

