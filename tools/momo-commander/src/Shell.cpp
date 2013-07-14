#include "Shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

CMDRES::CODE echoCommand(const Shell& shell, const ArgList& args) {
	for ( ArgList::const_iterator i = args.begin()+1; i != args.end(); ++i ) {
		printf( "%s ", *i );
	}
	printf("\n");
	return CMDRES::kSuccess;
}

CMDRES::CODE exitCommand(const Shell& shell, const ArgList& args) {
	return CMDRES::kQuit;
}

CMDRES::CODE commandNotFound(const Shell& shell, const ArgList& args) {
	if (args.size() == 0) {
		return CMDRES::kSuccess;
	}
	printf( "Unknown command '%s'", args[0] );
	return CMDRES::kError;
}

Shell::Shell( bool addEchoCommand /* = true */ )
	: m_prompt( "momo-commander" )
{
	RegisterCommand( "*", commandNotFound );
	RegisterCommand( "exit", exitCommand );
	if ( addEchoCommand )
		RegisterCommand( "echo", echoCommand );
}

void Shell::PrintHelp() {
	printf( "Available commands are\n" );
	for ( CommandMap::const_iterator i = m_commands.begin(); i != m_commands.end(); ++i ) {
		printf( " > %s\n", i->name.c_str() );
	}
}

void parse( ArgList& args, char* line)
{
    while ( *line != '\0' ) {
		while ( *line == ' ' || *line == '\t' || *line == '\n' )
			*line++ = '\0';
		if ( *line != '\0' )
			args.push_back(line);
		while ( *line != '\0' && *line != ' ' && 
			    *line != '\t' && *line != '\n' )  
			line++;
	}
}
CMDRES::CODE Shell::ExecuteCommand( const ArgList& args ) {
	if ( args.size() > 0 ) {
		if ( std::string("help") == args[0] ) {
			PrintHelp();
			return CMDRES::kSuccess;
		}
		for ( CommandMap::const_iterator i = m_commands.begin(); i != m_commands.end(); ++i )
		{
			if ( i->name == args[0] )
				return i->handler( *this, args );
		}
	}

	return m_catchAllHandler( *this, args );
}

void Shell::Prompt() {
	if ( !m_prompt.empty() )
		printf("\033[36m%s\033[0m$ ", m_prompt.c_str() );
}
CMDRES::CODE Shell::Do( const ArgList& in_args ) {
	char line[1024];
	line[0] = '\0';
	ArgList args;

	if ( Startup( in_args ) != CMDRES::kSuccess )
		return CMDRES::kError;
	Prompt();
	while  (fgets(line, sizeof(line), stdin) != 0) {
		line[ strlen(line)-1 ] = '\0'; // Drop the \n
		args.clear();
		parse(args, line);
		switch( ExecuteCommand(args) ) {
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
	}
}

void Shell::RegisterCommand( const std::string& name, command_handler handler ) {
	if ( name == "*" ) {
		m_catchAllHandler = handler;
		return;
	}
	for ( CommandMap::const_iterator i = m_commands.begin(); i != m_commands.end(); ++i )
	{
		if ( i->name == name )
			return;
	}
	CommandMapEntry entry;
	entry.name = name;
	entry.handler = handler;
	m_commands.push_back( entry );
}