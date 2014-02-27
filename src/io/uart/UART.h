/**
 * @file	UART.h
 * @author	Michael Marques <dryerzinia@gmail.com>
 * @brief	Object for controlling serial ports
 * @defgroup UART UART Control
 *
 * Contains struct and functions to easily manipulate serial ports
 */

#ifndef UART_H
#define UART_H

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

#include <unistd.h>

// fabs
#include <math.h>

#include <linux/serial.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <termios.h>

typedef struct {

	int fd;
	struct termios tio;

} UART;

int UART_open(UART *self, char *device_name, unsigned int baud_rate);
bool UART_is_open(UART *self);
int UART_close(UART *self);
int UART_read(UART *self, char *buffer, int length);
int UART_write(UART *self, char *buffer, int length);

#endif
