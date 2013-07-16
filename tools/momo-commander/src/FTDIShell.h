#ifndef __FTDIShell_h
#define __FTDIShell_h

#include "FTDIConnection.h"
#include "Shell.h"

#include <pthread.h>

class FTDIShell : public Shell
{
public:
	FTDIShell( FTDIConnection& connection )
		: Shell( false )
		, m_connection( connection )
		, exiting( false )
	{
		SetPrompt( "" );//m_connection.SerialNumber().c_str() );
		if (m_connection.Active()) {
			if ( !Activate() );
		}
	}

	~FTDIShell()
	{
		m_connection.Close();
		exiting = true;
		pthread_join( m_readerThread, NULL );
	}
	virtual CMDRES::CODE Startup( const ArgList& args );

	bool Activate();
	FTDIConnection& Connection() const { return m_connection; }

	bool exiting;
private:
	FTDIConnection& m_connection;
	pthread_t m_readerThread;
};

#endif