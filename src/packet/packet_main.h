/**
 * @file	packet_main.h
 * @author	Michael Marques <dryerzinia@gmail.com>
 * @brief	Program for demodulating APRS packets from raw data files or stdin
 *
 * @defgroup Packet APRS Packet demodulator
 *
 * Program for demodulating APRS packets from raw data files or stdin.
 * By default this program listens on stdin for Signed 8-bit audio samples at
 * a sample rate of 44100 Hz with APRS packets encoded at 1200 bps on tones
 * 1200Hz/2200Hz.
 * You can specify raw 8-bit audio files to process directly with the filename
 * option
 * You can also adjust the bit rate of the data, sample rate of the audio,
 * frequency of the two AFSK tones, offset multiplier, and noise floor.
 */

#include <stdlib.h>

#include <stddef.h>

#include <ctype.h>
#include <stdio.h>

#include <argp.h>

#include "AFSK_Demodulator.h"
