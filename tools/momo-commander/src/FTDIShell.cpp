#include "FTDIShell.h"

#define BUF_SIZE 10

void* ReadLoop( void* arg ) {
	const FTDIShell& shell = *( (const FTDIShell*)arg );

	//EVENT_HANDLE eh;
	FT_STATUS	ftStatus;

	//pthread_mutex_init(&eh.eMutex, NULL);
	//pthread_cond_init(&eh.eCondVar, NULL);

	//ftStatus = FT_SetEventNotification(shell.Connection().DeviceHandle(), FT_EVENT_RXCHAR, (PVOID)&eh);
	//if(ftStatus != FT_OK) {
	//	printf("Failed to set events\n");
	//	return NULL;
	//}

	setvbuf(stdout, NULL, _IONBF, 0);

	while (1) {
		//pthread_mutex_lock(&eh.eMutex);
		//pthread_cond_wait(&eh.eCondVar, &eh.eMutex);
		//pthread_mutex_unlock(&eh.eMutex);

		printf( "%s", shell.Connection().Read().c_str() );
		sleep(1);
		if ( shell.exiting )
			break;
	}
}

CMDRES::CODE forward_command( const Shell& shell, const ArgList& args )
{
	const FTDIShell& ftdiShell = reinterpret_cast<const FTDIShell&>(shell);
	std::string command;
	for ( ArgList::const_iterator i = args.begin(); i != args.end(); ++i )
	{
		if ( i != args.begin() )
			command += " ";
		command += *i;
	}
	command += "\n";
	if ( ftdiShell.Connection().Write( command.c_str() ) )
		return CMDRES::kSuccess;
	else
		return CMDRES::kError;
}

bool FTDIShell::Activate() {
	if ( !m_connection.Active() )
		m_connection.Open();
	pthread_create(&m_readerThread, NULL, &ReadLoop, (void*)this);
	RegisterCommand( "*", forward_command );
	if ( !Connection().Write( "echo \"You're connected to a MoMo FSU!\n" ) )
		m_connection.Close();
}

CMDRES::CODE FTDIShell::Startup( const ArgList& args ) {
	if ( !m_connection.Active() ) {
		printf( "\033[31mThe connection is not active.\033[0m" );
		return CMDRES::kError;
	}
	return CMDRES::kSuccess;
}