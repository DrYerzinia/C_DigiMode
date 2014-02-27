#include "UARTRecord.h"

UART uart;

void int_handler(){

	fprintf(stderr, "Interrupt received\n");

	if(UART_is_open(&uart))
		UART_close(&uart);

	exit(0);

}

static char doc[] = "UARTRecord -- Record audio data from a UART interface";

static char args_doc[] = "";

static struct argp_option options[] = {
		{"serial-port", 's', "FILE", OPTION_ARG_OPTIONAL, "Serial Port to read from"},
		{"baud-rate", 'b', "INT", OPTION_ARG_OPTIONAL, "Baud rate to set serial port to"},
		{0}
};

/**
 * Array for arguments to packet executable
 */
struct arguments {

	char *device_name;

	unsigned int baud_rate;

};

static error_t parse_opt(int key, char *arg, struct argp_state *state){

	// remove cast in full C
	struct arguments *arguments = ((struct arguments*)(state->input));

	switch(key){
		case 's':
			arguments->device_name = arg;
			break;
		case 'b':
			arguments->baud_rate = atoi(arg);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv){

	signal(SIGINT, int_handler);

	struct arguments arguments;

	arguments.device_name = NULL;
	arguments.baud_rate = 230400;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	if(arguments.device_name == NULL){
		fprintf(stderr, "Please specify a serial port to read from\n");
		return -1;
	}

	int status = UART_open(&uart, arguments.device_name, arguments.baud_rate);

	if(status != 0){
		fprintf(stderr, "Could not open serial port, terminating\n");
		return -1;
	}

	char buffer[BUFFER_SIZE];

	while(true){

		int bytes_read = UART_read(&uart, buffer, BUFFER_SIZE);
		if(bytes_read <= 0){
			fprintf(stderr, "Error reading! %d\n", bytes_read);
			break;
		}

		int bytes_written = fwrite(buffer, sizeof(char), bytes_read, stdout);
		if(bytes_written != bytes_read){
			fprintf(stderr, "Error writing!\n");
			break;
		}

		fflush(stdout);

	}

	fprintf(stderr, "Loop broken, closing...\n");

	UART_close(&uart);

	return 0;

}
