#include "CommanderShell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

CMDRES::CODE echoCommand(int argc, char** argv) {
	for (int i=1; i<argc; ++i ) {
		printf( "%s ", argv[i] );
	}
	printf("\n");
	return CMDRES::kSuccess;
}

CMDRES::CODE exitCommand(int argc, char** argv) {
	return CMDRES::kQuit;
}

CommanderShell::CommanderShell()
	: m_prompt( "momo-commander" )
{
	RegisterCommand( "echo", echoCommand );
	RegisterCommand( "exit", exitCommand );
}

int parse(char *line, char **argv)
{
	int argc = 0;
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          ++ argc;
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = '\0';                 /* mark the end of argument list  */
 	return argc-1;
}
CMDRES::CODE CommanderShell::ExecuteCommand( int argc, char **argv ) {
	for ( CommandMap::const_iterator i = m_commands.begin(); i != m_commands.end(); ++i )
	{
		if ( i->name == argv[0] )
			return i->handler( argc, argv );
	}

	printf( "Unknown command '%s'", argv[0] );
	return CMDRES::kError;
}

void CommanderShell::Prompt() {
	printf("%s$ ", m_prompt.c_str() );
}
CMDRES::CODE CommanderShell::Do( int in_argc, char** in_argv ) {
	char line[1024];
	char  *argv[8];;
	int argc;
	line[0] = '\0';
	do {
		if (*line == '\0' || *line == '\n') {
			Prompt();
			continue;
		}
		argc = parse(line, argv);
		switch( ExecuteCommand(argc, argv) ) {
			case CMDRES::kQuit:
				printf("GOODBYE!!\n");
				return CMDRES::kSuccess;
				break;
			case CMDRES::kError:
				printf("(!)\n");
				break;
			default:
				break;
		};
		Prompt();
	} while (fgets(line, sizeof(line), stdin) != 0);
}

void CommanderShell::RegisterCommand( const char* name, command_handler handler ) {
	CommandMapEntry entry;
	entry.name = name;
	entry.handler = handler;
	m_commands.push_back( entry );
}