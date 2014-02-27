/**
 * @file	UARTRecord.h
 * @author	Michael Marques <dryerzinia@gmail.com>
 * @brief	Pipes audio from UART to stdout
 * @ingroup sound_device
 *
 * Program to take audio data from a serial port and pipe it to stdout
 */

#ifndef UARTRECORD_H
#define UARTRECORD_H

#include "../io/uart/UART.h"

#include <stdio.h>
#include <stdbool.h>

#include <signal.h>

#include <argp.h>

#define BUFFER_SIZE 25

#endif
