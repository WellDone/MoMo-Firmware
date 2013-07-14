#include "CommanderShell.h"
#include "FTDIShell.h"

CMDRES::CODE listDevices( const Shell& shell, const ArgList& args ) {
	const CommanderShell& cshell = reinterpret_cast<const CommanderShell&>( shell );
	if ( !cshell.FTDI().Refresh() )
		return CMDRES::kError;
	printf( "Available FTDI Devices: \n");
	if ( cshell.FTDI().ConnectionCount() == 0 ) {
		printf( " * (no devices detected)\n" );
	} else {
		for ( FTDIConnectionIterator i = cshell.FTDI().ConnectionIteratorBegin(); i != cshell.FTDI().ConnectionIteratorEnd(); ++i)
		{
			printf( " * %s\n", i->SerialNumber().c_str() );
		}
	}
}

CMDRES::CODE StartSubShell( FTDIConnectionIterator i ) {
	printf("Starting subshell...\n");
	FTDIShell sub_shell( *i );
	sub_shell.Activate();
	ArgList args;
	return sub_shell.Do( args );
}
CMDRES::CODE connect( const Shell& shell, const ArgList& args ) {
	const CommanderShell& cshell = reinterpret_cast<const CommanderShell&>( shell );
	if ( !cshell.FTDI().Refresh() )
		return CMDRES::kError;
	if ( cshell.FTDI().ConnectionCount() == 0 ) {
		printf("No MoMo devices detected!\n");
		return CMDRES::kError;
	} else if ( args.size() == 2 ) {
		for ( FTDIConnectionIterator i = cshell.FTDI().ConnectionIteratorBegin(); i != cshell.FTDI().ConnectionIteratorEnd(); ++i)
		{
			if ( i->SerialNumber() == args[0] ) {
				return StartSubShell( i );
			}
		}
		printf( "Invalid device ID.\n" );
		return CMDRES::kError;
	} 
	else if ( cshell.FTDI().ConnectionCount() == 1 )
	{
		return StartSubShell( cshell.FTDI().ConnectionIteratorBegin() );	
	} else {
		printf( "More than one device detected, please pass a device ID.\n" );
		return CMDRES::kError;
	}
	return CMDRES::kSuccess;
}

CommanderShell::CommanderShell()
	: Shell()
{
	RegisterCommand( "list", listDevices );
	RegisterCommand( "command", connect );
}

CMDRES::CODE CommanderShell::Startup( const ArgList& args )
{
	if ( std::find( args.begin(), args.end(), std::string("-r") ) != args.end()
	  || std::find( args.begin(), args.end(), std::string("--raw") ) != args.end() ) {
		return CMDRES::kSuccess;
	} else {
		if ( connect( *this, args ) == CMDRES::kError )
			return CMDRES::kSuccess;
		else
			return CMDRES::kQuit;
	}
}