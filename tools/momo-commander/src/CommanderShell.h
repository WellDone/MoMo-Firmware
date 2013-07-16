#ifndef __CommanderShell_h
#define __CommanderShell_h

#include "Shell.h"
#include "FTDIManager.h"

class CommanderShell : public Shell
{
public:
	CommanderShell();

	virtual CMDRES::CODE Startup( const ArgList& args );

	FTDIManager& FTDI() const { return const_cast<FTDIManager&>( m_ftdi ); }
private:
	FTDIManager m_ftdi;
};
#endif