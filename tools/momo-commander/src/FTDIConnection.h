#ifndef __FTDIConnection_h
#define __FTDIConnection_h

#include <string>
#include "ftd2xx.h"

class FTDIConnection
{
public:
	FTDIConnection(const char* serialNumber)
		: m_deviceSerialNumber( serialNumber )
		, m_deviceHandle( NULL )
		, m_baudRate( 115200 )
	{}
	FTDIConnection( const FTDIConnection& source )
		: m_deviceSerialNumber( source.m_deviceSerialNumber )
		, m_deviceHandle( 0 )
		, m_baudRate( source.m_baudRate )
	{
	}
	~FTDIConnection()
	{
		Close();
	}

	bool Active() const { return m_deviceHandle != 0;}

	bool Open();
	void Close();

	unsigned int GetBAUD() const    { return m_baudRate; }
	void SetBAUD(unsigned int baud) { m_baudRate = baud; }

	const std::string& SerialNumber() const { return m_deviceSerialNumber; }
	FT_HANDLE DeviceHandle() const { return m_deviceHandle; }
private:
	std::string m_deviceSerialNumber;
	FT_HANDLE m_deviceHandle;

	unsigned int m_baudRate;
};

bool operator==( const FTDIConnection& a, const FTDIConnection& b );

#endif