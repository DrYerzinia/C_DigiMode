
#include "argp.h"

error_t argp_parse (struct argp *argp, int argc, char **argv, unsigned flags, int *arg_index,  void *input){

	struct argp_state state;

	state.input = input;

	int i;

	for(i = 1; i < argc; i++){

		if(argv[i][0] == '-'){

			if(argv[i][1] == '-'){

				struct argp_option *op = argp->options;

				bool found = false;

				int c = 0;

				while(op->arg[0] != 0){

					c = 0;

					while(true){

						// They are equal!
						if(op->name[c] == '\0' && (argv[i][c+2] == '\0' || argv[i][c+2] == '=')){
							found = true;
							break;
						}

						// Finish if one string is a different length or if they have different values
						else if(op->name[c] == '\0' || argv[i][c+2] == '\0' || argv[i][c+2] == '=' || op->name[c] != argv[i][c+2]){
							break;
						}

						c++;

					}

					// If we found the option match the parameter
					if(found){
						break;
					}

					op++;

				}

				if(found){
					argp->parser(op->key, &argv[i][c+3], &state);
				}

				// Invalid Option
				else {
					return -1;
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
