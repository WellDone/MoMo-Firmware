#ifndef __ftdi_h
#define __ftdi_h

#include <string>
#include <vector>

#include "ftd2xx.h"
#include "FTDIConnection.h"

typedef std::vector<FTDIConnection> FTDIConnectionList;
typedef FTDIConnectionList::iterator FTDIConnectionIterator;

class FTDIManager {
public:
	FTDIManager();
	~FTDIManager();

	bool Refresh();
	FTDIConnectionIterator ConnectionIteratorBegin() { return m_connections.begin(); };
	FTDIConnectionIterator ConnectionIteratorEnd() { return m_connections.end(); };
	unsigned int ConnectionCount() { return m_connections.size(); }
	unsigned int ActiveConnectionCount();
	//std::vector<const FTDIConnection*> ActiveConnections() { return m_connection.Active(); }

private:
	FTDIConnectionList m_connections;
};

#endif;