#include "UART.h"

int UART_open(UART *self, char *device_name, unsigned int baud_rate){

	self->fd = open(device_name, O_RDWR | O_NOCTTY);
	if(self->fd < 0){
		perror("Unable to open UART");
		return -1;
	}

	tcgetattr(self->fd, &(self->tio));

	self->tio.c_cflag = CS8 | CLOCAL | CREAD;
	self->tio.c_iflag = IGNPAR;
	self->tio.c_oflag = 0;
	self->tio.c_lflag = 0;

	/* Set baud rate
	 */
	speed_t standard_baud_rate = B0;
	switch(baud_rate){
		case 50:
			standard_baud_rate = B50;
			break;
		case 75:
			standard_baud_rate = B75;
			break;
		case 110:
			standard_baud_rate = B110;
			break;
		case 134:
			standard_baud_rate = B134;
			break;
		case 150:
			standard_baud_rate = B150;
			break;
		case 200:
			standard_baud_rate = B200;
			break;
		case 300:
			standard_baud_rate = B300;
			break;
		case 600:
			standard_baud_rate = B600;
			break;
		case 1200:
			standard_baud_rate = B1200;
			break;
		case 1800:
			standard_baud_rate = B1800;
			break;
		case 2400:
			standard_baud_rate = B2400;
			break;
		case 4800:
			standard_baud_rate = B4800;
			break;
		case 9600:
			standard_baud_rate = B9600;
			break;
		case 19200:
			standard_baud_rate = B19200;
			break;
		case 38400:
			standard_baud_rate = B38400;
			break;
		case 57600:
			standard_baud_rate = B57600;
			break;
		case 115200:
			standard_baud_rate = B115200;
			break;
		case 230400:
			standard_baud_rate = B230400;
			break;
	}

	if(standard_baud_rate == B38400){

		// Ensure 38400 is actually 38400
		struct serial_struct ss;
		ioctl(self->fd, TIOCGSERIAL, &ss);
		ss.flags &= ~ASYNC_SPD_MASK;
		ioctl(self->fd, TIOCSSERIAL, &ss);

	} else if(standard_baud_rate == B0){

		standard_baud_rate = B38400;

		struct serial_struct ss;

		ioctl(self->fd, TIOCGSERIAL, &ss);

		ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;

		ss.custom_divisor = ((float)(ss.baud_base + (baud_rate / 2)) / baud_rate) + 0.5;

		unsigned int actual_rate = ss.baud_base / ss.custom_divisor;

		fprintf(stderr, "Actual baud rate set to %d\n", actual_rate);
		fprintf(stderr, "Error of %f%%\n", fabs((float)(baud_rate - actual_rate) / ((float)(baud_rate + actual_rate) / 2.0F)) * 100.0F);

		ioctl(self->fd, TIOCSSERIAL, &ss);

	}

	cfsetispeed(&(self->tio), standard_baud_rate);
	cfsetispeed(&(self->tio), standard_baud_rate);

	/* Wait until there is at least 1 byte and
	 * have an indefinite timeout
	 */
	self->tio.c_cc[VMIN] = 1;
	self->tio.c_cc[VTIME] = 0;

	tcflush(self->fd, TCIFLUSH);
	tcsetattr(self->fd, TCSANOW, &(self->tio));

	return 0;

}

bool UART_is_open(UART *self){

	if(self->fd < 0)
		return false;

	return true;

}

int UART_close(UART *self){

	// Ensure any custom baud rate is reset to the normal speed
	struct serial_struct ss;
	ioctl(self->fd, TIOCGSERIAL, &ss);
	ss.flags &= ~ASYNC_SPD_MASK;
	ioctl(self->fd, TIOCSSERIAL, &ss);

	int status = close(self->fd);
	if(status == -1){
		perror("Unable to close socket");
		return -1;
	}

	self->fd = -1;

	return 0;

}

int UART_read(UART *self, char *buffer, int length){

	int bytes_read = read(self->fd, buffer, length);
	return bytes_read;

}

int UART_write(UART *self, char *buffer, int length){

	int bytes_written = write(self->fd, buffer, length);
	if(bytes_written != length){
		perror("Did not write length bytes");
		return -1;
	}

	return 0;

}
