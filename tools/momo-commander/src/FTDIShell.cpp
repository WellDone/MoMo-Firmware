#include "FTDIShell.h"

#define BUF_SIZE 10
MoMoResponseStatus searchChars[3] = { kMoMoPending, kMoMoSuccess, kMoMoFailure };

void* ReadLoop( void* arg ) {
	FTDIShell::Syncronizer& syncronizer = *( (FTDIShell::Syncronizer*)arg );
	setvbuf(stdout, NULL, _IONBF, 0);

	while (1) {
		std::string data = syncronizer.shell.Connection().Read();
		pthread_mutex_lock(&syncronizer.mutex);
		if ( syncronizer.echo )
			printf( "%s", data.c_str() );
		if ( syncronizer.capture ) {
			for (int i=0; i<3; ++i)
			{
				size_t index = data.find((char)searchChars[i]);
				if ( index != std::string::npos )
				{
					syncronizer.response_data += data.substr(0,index);
					syncronizer.response_status = searchChars[i];
					syncronizer.capture = false;
					pthread_cond_signal(&syncronizer.done_condition);
					break;
				}
			}
			if ( syncronizer.response_status == kMoMoNone )
				syncronizer.response_data += data;
		}

		pthread_mutex_unlock(&syncronizer.mutex);
		if ( syncronizer.exiting ) {
			break;
		}
	}
	return 0;
}

FTDIShell::FTDIShell( FTDIConnection& connection, FILE* input )
	: Shell( input, false )
	, m_connection( connection )
	, m_syncronizer( *this )
{
	SetPrompt( "FSU" );
	if (m_connection.Active()) {
		if ( !Activate() );
	}
}

FTDIShell::~FTDIShell()
{
	pthread_mutex_lock( &m_syncronizer.mutex );
	m_syncronizer.exiting = true;
	pthread_mutex_unlock( &m_syncronizer.mutex );
	pthread_join( m_readerThread, NULL );
	m_connection.Close();
}

std::pair<MoMoResponseStatus, std::string> FTDIShell::WaitForResponse()
{
	std::pair<MoMoResponseStatus, std::string> result;
	pthread_mutex_lock( &m_syncronizer.mutex );
	while ( m_syncronizer.capture )
	{
		pthread_cond_wait( &m_syncronizer.done_condition, &m_syncronizer.mutex );
	}
	result.first = m_syncronizer.response_status;
	result.second = m_syncronizer.response_data;

	m_syncronizer.capture = false;
	m_syncronizer.response_status = kMoMoNone;
	m_syncronizer.response_data.erase();
	pthread_mutex_unlock( &m_syncronizer.mutex );
	return result;
}

void FTDIShell::StartCapture()
{
	pthread_mutex_lock( &m_syncronizer.mutex );
	m_syncronizer.capture = true;
	pthread_mutex_unlock( &m_syncronizer.mutex );
}

CMDRES::CODE forward_command( const Shell& shell, const ArgList& args )
{
	FTDIShell& ftdiShell = const_cast<FTDIShell&>( reinterpret_cast<const FTDIShell&>(shell) );
	std::string command;
	for ( ArgList::const_iterator i = args.begin(); i != args.end(); ++i )
	{
		if ( i != args.begin() )
			command += " ";
		command += *i;
	}
	command += "\n";
	while (true)
	{
		ftdiShell.StartCapture();
		if ( !ftdiShell.Connection().Write( command.c_str() ) )
			return CMDRES::kError;
		std::pair<MoMoResponseStatus, std::string> response = ftdiShell.WaitForResponse();
		switch (response.first)
		{
			case kMoMoPending:
				printf("> ");
				if ( !ftdiShell.TryGetLine( command ) )
					return CMDRES::kError;
				command += "\n";
				printf("%s", command.c_str() );
				break;
			case kMoMoSuccess:
				return CMDRES::kSuccess;
			case kMoMoFailure:
				return CMDRES::kError;
			default:
				return CMDRES::kError; // Bad news
		}
	}
	return CMDRES::kError;
}

bool FTDIShell::Activate() {
	if ( !m_connection.Active() )
		m_connection.Open();
	pthread_create(&m_readerThread, NULL, &ReadLoop, (void*)&m_syncronizer);
	sleep(1);
	//RegisterCommand( "*", forward_command );
	m_syncronizer.echo = false;
	StartCapture();
	if ( !Connection().Write( "help\n" ) ) {
		m_connection.Close();
	} else {
		std::pair<MoMoResponseStatus, std::string> response = WaitForResponse();
		if ( response.first != kMoMoSuccess ) {
			m_connection.Close();
		}
		else
		{
			size_t old_pos, pos = -1;
			while ( true )
			{
				old_pos = pos+1;
				pos = response.second.find( "\n", old_pos );
				if ( pos == std::string::npos) {
					//RegisterCommand( response.second.substr( old_pos ), forward_command );
					break;
				}
				RegisterCommand( response.second.substr( old_pos, pos-old_pos ), forward_command );
			}
		}
	}
	m_syncronizer.echo = true;
	StartCapture();
	Connection().Write( "echo \"You're connected to a MoMo FSU!\n" );
	WaitForResponse();
}

CMDRES::CODE FTDIShell::Startup( const ArgList& args ) {
	if ( !m_connection.Active() ) {
		printf( "\033[31mThe connection is not active.\033[0m" );
		return CMDRES::kError;
	}
	if ( !args.empty() ) {
		forward_command( *this, args );
		return CMDRES::kQuit;
	}
	else
	{
		return CMDRES::kSuccess;
	}
}