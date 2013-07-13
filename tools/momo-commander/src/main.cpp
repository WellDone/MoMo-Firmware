#include "CommanderShell.h"
#include "FTDIManager.h"
#include "FTDIShell.h"

static FTDIManager ftdi;
CMDRES::CODE listDevices( int argc, char** argv ) {
	if ( !ftdi.Refresh() )
		return CMDRES::kError;
	printf( "Available FTDI Devices: \n");
	for ( FTDIConnectionIterator i = ftdi.ConnectionIteratorBegin(); i != ftdi.ConnectionIteratorEnd(); ++i)
	{
		printf( " * %s\n", i->SerialNumber().c_str() );
	}
}

CMDRES::CODE StartSubShell( FTDIConnectionIterator i ) {
	printf("Starting subshell...\n");
	FTDIShell sub_shell( *i );
	sub_shell.Activate();
	int argc = 0;
	char** argv = 0;
	return sub_shell.Do( argc, argv );
}
CMDRES::CODE connect(int argc, char** argv) {
	if ( !ftdi.Refresh() )
		return CMDRES::kError;
	if ( ftdi.ConnectionCount() == 0 ) {
		printf("No MoMo devices detected!\n");
		return CMDRES::kError;
	} else if ( argc == 2 ) {
		for ( FTDIConnectionIterator i = ftdi.ConnectionIteratorBegin(); i != ftdi.ConnectionIteratorEnd(); ++i)
		{
			if ( i->SerialNumber() == argv[1] ) {
				return StartSubShell( i );
			}
		}
		printf( "Invalid device ID." );
		return CMDRES::kError;
	} 
	else if ( ftdi.ConnectionCount() == 1 )
	{
		return StartSubShell( ftdi.ConnectionIteratorBegin() );	
	} else {
		printf( "More than one device detected, please pass a device ID." );
		return CMDRES::kError;
	}
	return CMDRES::kSuccess;
}

int main( int argc, char **argv ) {
	CommanderShell shell;
	shell.RegisterCommand( "connect", connect );
	shell.RegisterCommand( "list", listDevices );
	switch (shell.Do( argc, argv ))
	{
		case CMDRES::kError:
			return 1;
		case CMDRES::kSuccess:
		default:
			return 0;
	}
}