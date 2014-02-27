###############################################################################
# C_DigiMode                                                                  #
#                                                                             #
# A library for decoding various Digital radio modes with a sound card        #
#  written in C                                                               #
#                                                                             #
# Author Michael Marques <dryerzinia@gmail.com>                               #
#                                                                             #
###############################################################################

CC=gcc
CFLAGS=-O3 -Wall

.PHONY: build disable-float
build: create-dirs all

all: bin/psk31 bin/filter_test bin/packet bin/sound_device bin/NOAA_ATP bin/mixer bin/forward

create-dirs:
	mkdir -p bin
	mkdir -p build
	mkdir -p build/obj

###############################################################################
# Configuration Options                                                       #
###############################################################################

# Enables debug code
enable-debug:
	@echo "Debug enabled"
	$(eval DEFINES+=-D__DEBUG)

# For hardware without FPU's uses Integer only math to gain speed at cost of some accuracy
disable-float:
	@echo "Floats disabled"
	$(eval DEFINES+=-DDISABLE_FLOAT)

# Compiles faster less accurate algorithms for low end hardware
fast-algorithms:
	@echo "Faster Less accurate algorithms enabled"
	$(eval DEFINES+=-DFAST_ALGORITHMS)

###############################################################################
# Mixes 2 raw audio files                                                     #
###############################################################################

bin/mixer: build/obj/mixer.o
	$(CC) $(CFLAGS) -o $@ build/obj/mixer.o

build/obj/mixer.o: src/mixer/mixer.c
	$(CC) $(CFLAGS) -o $@ -c src/mixer/mixer.c

###############################################################################
# Record audio data from a Serial Device                                      #
###############################################################################

bin/UARTRecord: build/obj/UARTRecord.o build/obj/UART.o
	$(CC) $(CFLAGS) -o $@ build/obj/UARTRecord.o build/obj/UART.o

build/obj/UARTRecord.o: src/sound_device/UARTRecord.c
	$(CC) $(CFLAGS) -o $@ -c src/sound_device/UARTRecord.c

build/obj/UART.o: src/io/uart/UART.c
	$(CC) $(CFLAGS) -o $@ -c src/io/uart/UART.c

###############################################################################
# PSK31 Demodulator                                                           #
###############################################################################

bin/psk31: build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o
	g++ -O3 -o $@ build/obj/main.o build/obj/psk31.o build/obj/Util.o build/obj/psk31_tdtl.o build/obj/psk31_wcpd.o build/obj/psk31_wpdd.o

build/obj/main.o: src/psk31/main.cpp
	g++ -O3 -o $@ -c src/psk31/main.cpp

build/obj/psk31.o: src/psk31/psk31.cpp
	g++ -O3 -o $@ -c src/psk31/psk31.cpp

build/obj/psk31_tdtl.o: src/psk31/psk31_tdtl.cpp
	g++ -O3 -o $@ -c src/psk31/psk31_tdtl.cpp

build/obj/psk31_wcpd.o: src/psk31/psk31_wcpd.cpp
	g++ -O3 -o $@ -c src/psk31/psk31_wcpd.cpp

build/obj/psk31_wpdd.o: src/psk31/psk31_wpdd.cpp
	g++ -O3 -o $@ -c src/psk31/psk31_wpdd.cpp

###############################################################################
# Finite Impulse Response filter                                              #
###############################################################################

bin/filter_test: build/obj/filter_test.o build/obj/filter.o build/obj/FIR_filter.o build/obj/Util.o
	g++ -O3 -o $@ build/obj/filter_test.o build/obj/filter.o build/obj/FIR_filter.o build/obj/Util.o

build/obj/filter_test.o: src/filter/filter_test.cpp
	g++ -O3 -o $@ -c src/filter/filter_test.cpp

build/obj/filter.o: src/filter/filter.cpp
	g++ -O3 -o $@ -c src/filter/filter.cpp

build/obj/FIR_filter.o: src/filter/FIR_filter.cpp
	g++ -O3 -o $@ -c src/filter/FIR_filter.cpp

###############################################################################
# Gets Microphone and pipes to stdout or file                                 #
###############################################################################

bin/sound_device: build/obj/sound_device.o
	$(CC) $(CFLAGS) -o $@ build/obj/sound_device.o -lpulse -lpulse-simple

build/obj/sound_device.o: src/sound_device/sound_device.c
	$(CC) $(CFLAGS) -o $@ -c src/sound_device/sound_device.c

###############################################################################
# NOAA ATP decoder                                                            #
###############################################################################

bin/NOAA_ATP: build/obj/NOAA_ATP_main.o build/obj/NOAA_ATP.o build/obj/Util.o
	g++ -O3 -o $@ build/obj/NOAA_ATP_main.o build/obj/NOAA_ATP.o build/obj/Util.o

build/obj/NOAA_ATP_main.o: src/NOAA_ATP/NOAA_ATP_main.cpp
	g++ -O3 -o $@ -c src/NOAA_ATP/NOAA_ATP_main.cpp

build/obj/NOAA_ATP.o: src/NOAA_ATP/NOAA_ATP.cpp
	g++ -O3 -o $@ -c src/NOAA_ATP/NOAA_ATP.cpp

###############################################################################
# Network fowarder for data                                                   #
###############################################################################

bin/forward: build/obj/forward.o
	$(CC) -O3 -o $@ build/obj/forward.o

build/obj/forward.o: src/net/forward.c
	$(CC) -O3 -o $@ -c src/net/forward.c

###############################################################################
# AFSK Demodulator / APRSPacket Decoder                                       #
###############################################################################

bin/packet: build/obj/char_array_expandable.o build/obj/float_ring_buffer.o build/obj/char_ring_buffer.o build/obj/APRSPacket.o build/obj/AFSK_Demodulator.o build/obj/packet_main.o build/obj/crcccitt.o
	$(CC) $(CFLAGS) -lm $(DEFINES) -o $@ build/obj/char_array_expandable.o build/obj/float_ring_buffer.o build/obj/char_ring_buffer.o build/obj/APRSPacket.o build/obj/AFSK_Demodulator.o build/obj/packet_main.o build/obj/crcccitt.o

build/obj/packet_main.o: src/packet/packet_main.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/packet_main.c

build/obj/AFSK_Demodulator.o: src/packet/AFSK_Demodulator.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/AFSK_Demodulator.c

build/obj/APRSPacket.o: src/packet/APRSPacket.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/APRSPacket.c

build/obj/char_ring_buffer.o: src/packet/char_ring_buffer.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/char_ring_buffer.c

build/obj/float_ring_buffer.o: src/packet/float_ring_buffer.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/float_ring_buffer.c

build/obj/char_array_expandable.o: src/packet/char_array_expandable.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c src/packet/char_array_expandable.c

###############################################################################
# CRC                                                                         #
###############################################################################

build/obj/crcccitt.o: src/crc/crcccitt.c
	$(CC) $(CFLAGS) -o $@ -c src/crc/crcccitt.c

###############################################################################
# Math                                                                        #
###############################################################################

build/obj/trig_table.o: src/math/trig_table.c
	$(CC) $(CFLAGS) -o $@ -c src/math/trig_table.c

###############################################################################
# Utility functions                                                           #
###############################################################################

build/obj/Util.o: src/util/Util.cpp
	g++ -O3 -o $@ -c src/util/Util.cpp

###############################################################################
# Cleans built objects                                                        #
###############################################################################

clean:
	rm -rf bin
	rm -rf build

