// mixer.cpp

#include <stdbool.h>
#include <stdio.h>
#include <argp.h>

const char *program_version = "Mixer 1.0";
const char *program_bug_address = "<dryerzinia@gmail.com>";

static char doc[] = "Mixer -- A program for mixing 2 raw audio files together";

static char args_doc[] = "";

static struct argp_option options[] = {
		{"input-file-1", 'i', "FILE", 0, "First raw audio file to mix"},
		{"input-file-2", 'k', "FILE", 0, "Second raw audio file to mix"},
		{"output-file", 'o', "FILE", 0, "Mixed output audio file"},
		{0}
};

struct arguments {

	char *in_1_filename;
	char *in_2_filename;

	char *out_filename;

};

static error_t parse_opt(int key, char *arg, struct argp_state *state){

	// remove cast in full C
	struct arguments *arguments = ((struct arguments*)(state->input));

	switch(key){
		case 'i':
			arguments->in_1_filename = arg;
			break;
		case 'k':
			arguments->in_2_filename = arg;
			break;
		case 'o':
			arguments->out_filename = arg;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv){

	struct arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	FILE *data_file1 = NULL;
	FILE *data_file2 = NULL;

	data_file1 = fopen(arguments.in_1_filename, "r");
	if(data_file1 == NULL){
		fprintf(stderr, "Error: couldn't open input file 1\n");
		return -1;
	}

	data_file2 = fopen(arguments.in_2_filename, "r");
	if(data_file2 == NULL){
		fprintf(stderr, "Error: couldn't open input file 2\n");
		fclose(data_file1);
		return -1;
	}

	FILE *output_file = fopen(arguments.out_filename, "w");
	if(output_file == NULL){
		fprintf(stderr, "Error: unable to create output file\n");
		fclose(data_file1);
		fclose(data_file2);
		return -1;
	}

	bool eof1 = false, eof2 = false;
	char byte1, byte2;

	while(!eof1 && !eof2){

		byte1 = getc(data_file1);
		byte2 = getc(data_file2);

		fputc(byte1*byte2/16384, output_file);

		eof1 = feof(data_file1);
		eof2 = feof(data_file2);

		if(eof1 != eof2){
			fprintf(stderr, "Error: input file sizes do not match!\n");
			return -1;
		}

	}

	fclose(data_file1);
	fclose(data_file2);
	fclose(output_file);

	return 0;

}
