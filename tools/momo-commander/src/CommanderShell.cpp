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

CMDRES::CODE StartSubShell( FTDIConnectionIterator i, ArgList& args ) {
	FTDIShell sub_shell( *i );
	sub_shell.Activate();
	return sub_shell.Do( args );
}
CMDRES::CODE connect( const Shell& shell, const ArgList& args ) {
	const CommanderShell& cshell = reinterpret_cast<const CommanderShell&>( shell );
	if ( !cshell.FTDI().Refresh() )
		return CMDRES::kError;

	ArgList newArgs;
	std::string deviceID = "";
	for ( int i=0; i<args.size(); ++i )
	{
		if ( strcmp(args[i], "-D") == 0 )
		{
			if ( i+1 < args.size() ) {
				deviceID = args[++i];
			}
			else
			{
				printf("\033[31mNo device ID specified.\033[0m\n");
				return CMDRES::kError;
			}
		} else {
			newArgs.push_back(args[i]);
		}
	}

	FTDIConnectionIterator conn;
	if ( cshell.FTDI().ConnectionCount() == 0 ) {
		printf("\033[31mNo MoMo devices detected!\033[31m\n");
		return CMDRES::kError;
	} else if ( !deviceID.empty() ) {
		for ( conn = cshell.FTDI().ConnectionIteratorBegin(); conn != cshell.FTDI().ConnectionIteratorEnd(); ++conn )
		{
			if ( conn->SerialNumber() == deviceID ) {
				break;
			}
		}
		if ( conn == cshell.FTDI().ConnectionIteratorEnd() )
		{
			printf( "\033[31mInvalid device ID.\033[0m\n" );
			return CMDRES::kError;
		}
	}
	else if ( cshell.FTDI().ConnectionCount() == 1 )
	{
		conn = cshell.FTDI().ConnectionIteratorBegin();
	} else {
		printf( "More than one device detected, please pass a device ID with -D <device>.\n" );
		return CMDRES::kError;
	}

	return StartSubShell( conn, newArgs );
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