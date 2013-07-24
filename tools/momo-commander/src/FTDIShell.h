#ifndef __FTDIShell_h
#define __FTDIShell_h

#include "FTDIConnection.h"
#include "Shell.h"

#include <pthread.h>

#define SYN 0x16
#define ACK 0x06
#define NAK 0x15
#define EOT 0x04
enum MoMoResponseStatus
{
	kMoMoNone = 0,
	kMoMoPending = SYN,
	kMoMoSuccess = ACK,
	kMoMoFailure = NAK
};

class FTDIShell : public Shell
{
public:
	FTDIShell( FTDIConnection& connection, FILE* input = stdin );
	~FTDIShell();
	virtual CMDRES::CODE Startup( const ArgList& args );

	bool Activate();
	FTDIConnection& Connection() const { return m_connection; }
	void StartCapture();
	std::pair<MoMoResponseStatus, std::string> WaitForResponse();

	struct Syncronizer
	{
		pthread_mutex_t    mutex;
		pthread_cond_t     done_condition;
		std::string        response_data;
		MoMoResponseStatus response_status;
		FTDIShell&         shell;
		bool exiting;
		bool capture;
		bool echo;
		Syncronizer( FTDIShell& s ) 
			: exiting( false )
			, capture( false )
			, response_status( kMoMoNone )
			, shell( s )
		{}
	};
private:
	FTDIConnection& m_connection;
	
	pthread_t m_readerThread;
	Syncronizer m_syncronizer;
};

#endif