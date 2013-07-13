#include <stdio.h>
#include <string.h>

namespace CMDRES {
	enum command_result_code {
		kSuccess = 0,
		kError = -1,
		kQuit = 256
	};
}

int parse( const char* line ) {
	if ( strcmp( line, "exit\n" ) == 0 )
		return CMDRES::kQuit;
	else if ( strcmp( line, "err\n" ) == 0 )
		return CMDRES::kError;
	return CMDRES::kSuccess;
}

int do_shell() {
	char line[1024];
	do {
		switch( parse(line) ) {
			case CMDRES::kQuit:
				printf("GOODBYE!!\n");
				return 0;
				break;
			case CMDRES::kError:
				printf("An error occurred.\n");
				break;
		};

		printf("momo-commander$ ");
	} while (fgets(line, sizeof(line), stdin) != 0);
}

int main( int argc, char **argv ) {
	if ( argc == 1 ) {
		return do_shell();
	}
	else if ( argc == 2) {
		if ( strcmp(argv[0],"config") == 0 ) {
			// configure things
		}
	}
	return 0;
}