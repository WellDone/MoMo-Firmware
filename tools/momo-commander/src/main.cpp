#include "CommanderShell.h"

int main( int argc, char **argv ) {
	CommanderShell shell;
	
	ArgList args;
	for ( int i = 0; i<argc; ++i ) {
		args.push_back( argv[i] );
	}

	switch (shell.Do( args ))
	{
		case CMDRES::kError:
			return 1;
		case CMDRES::kSuccess:
		default:
			return 0;
	}
}