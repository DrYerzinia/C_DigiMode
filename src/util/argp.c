
#include "argp.h"

static void argp_print_help(const char *cmd, struct argp_option *ops){

	printf("Usage: %s [OPTION]...\n", cmd);

	while(*(uint8_t*)ops != 0){

		int llen = 6;
		int has_short = isprint(ops->key);

		char *pch;

		printf("  ");

		// Single char option
		if(has_short)
			printf("-%c", ops->key);
		else
			printf("  ");

		// string option
		if(ops->name != NULL){

			if(has_short)
				printf(", ");
			else
				printf("  ");

			printf("--%s", ops->name);
			llen += strlen(ops->name) + 2;

			if(ops->arg != NULL){
				printf("=%s", ops->arg);
				llen += strlen(ops->arg) + 1;
			}
		}

		// Print description
		if(ops->doc != NULL){

			char doc[strlen(ops->doc)];
			strcpy(doc, ops->doc);

			if(llen > 30){
				printf("\n");
				llen = 0;
			} else if(llen == 30){
				llen -= 2;
			}

			if(llen < 30){
				printf("%*s" "%s", (30-llen), " ", " ");
				llen += (30-llen);
			}

			pch = strtok(doc, " ");

			while(pch != NULL){

				llen += strlen(pch) + 1;
				if(llen >= 80){
					printf("\n");
					llen = -2;
				}

				if(llen < 30){
					printf("%*s" "%s", (30-llen), " ", " ");
					llen += (30-llen);
				}

				printf("%s ", pch);

				pch = strtok(NULL, " ");

			};

		}

		printf("\n");

		ops++;

	};

};

static int argp_parse_argument(const char *name, const char *arg){

	int c = 0;

	while(true){

		// They are equal!
		if(name[c] == '\0' && (arg[c+2] == '\0' || arg[c+2] == '='))
			return c;

		// Finish if one string is a different length or if they have different values
		else if(name[c] == '\0' || arg[c+2] == '\0' || arg[c+2] == '=' || name[c] != arg[c+2])
			return -1;

		c++;

	}

	return -1;

}

error_t argp_parse(struct argp *argp, int argc, char **argv, unsigned flags, int *arg_index,  void *input){

	struct argp_state state;

	state.input = input;

	int i;

	for(i = 1; i < argc; i++){

		if(argv[i][0] == '-'){

			if(argv[i][1] == '-'){

				struct argp_option *op = argp->options;

				int c = -1;

				while(*(uint8_t*)op != 0){

					c = argp_parse_argument(op->name, argv[i]);

					// If we found the option match the parameter
					if(c >= 0)
						break;

					op++;

				}

				if(c >= 0)
					argp->parser(op->key, &argv[i][c+3], &state);

				else {
					// Check if it is help
					c = argp_parse_argument("help", argv[i]);
					if(c >= 0){
						argp_print_help(argv[0], argp->options);
						return ARGP_ERR_HELP;
					} else {
						// Invalid Option
						fprintf(stderr, "%s: unrecognized option '%s'\nTry '%s --help' for more information.\n", argv[0], argv[i], argv[0]);
						return ARGP_ERR_UNKNOWN;
					}
				}

			} else {

				// single character option
				struct argp_option *op = argp->options;

				while(op->arg[0] != 0){

					if(op->key == argv[i][1] && argv[i][2] == '\0'){
						argp->parser(op->key, NULL, &state);
						break;
					}

					op++;

				}

			}

		}

		// Invalid Option
		else {
			return -1;
		}

	}

	return 0;

}
