#ifndef __FTDIShell_h
#define __FTDIShell_h

#include "FTDIConnection.h"
#include "CommanderShell.h"

#include <pthread.h>

class FTDIShell : public CommanderShell
{
public:
	FTDIShell( FTDIConnection& connection )
		: CommanderShell()
		, m_connection( connection )
	{
		SetPrompt( m_connection.SerialNumber().c_str() );
		if (m_connection.Active()) {
			Activate();
		}
	}

	~FTDIShell()
	{
		m_connection.Close();
		pthread_kill( m_readerThread, 0 );
	}

	void Activate();
	FTDIConnection& Connection() const { return m_connection; }
private:
	FTDIConnection& m_connection;
	pthread_t m_readerThread;
};

#endif