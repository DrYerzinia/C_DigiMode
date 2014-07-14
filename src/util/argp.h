
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

#include <ctype.h>

#define OPTION_ARG_OPTIONAL 1

#define ARGP_ERR_UNKNOWN -1
#define ARGP_ERR_HELP -2

struct argp_option {

	const char *name;

	int key;

	const char *arg;

	int flags;

	const char *doc;
	int group;

};

#ifndef __error_t_defined
typedef int error_t;
#endif

struct argp_state {

	const struct argp *root_argp;

	int argc;
	char **argv;

	int next;

	unsigned flags;

	unsigned arg_num;

	int quoted;

	void *input;
	void **child_inputs;

	void *hook;

	char *name;

	FILE *err_stream;
	FILE *out_stream;

	void *pstate;

};

typedef error_t (*argp_parser_t) (int __key, char *__arg, struct argp_state *__state);

struct argp {

	struct argp_option *options;

	argp_parser_t parser;

	const char *args_doc;

	const char *doc;

};

error_t argp_parse(struct argp *argp, int argc, char **argv, unsigned flags, int *arg_index,  void *input);
