#include "FTDIConnection.h"

bool FTDIConnection::Open() {
	FT_STATUS ftStatus;

	if((ftStatus = FT_OpenEx( const_cast<char*>(m_deviceSerialNumber.c_str()), FT_OPEN_BY_SERIAL_NUMBER, &m_deviceHandle)) != FT_OK){
		/* 
			This can fail if the ftdi_sio driver is loaded
	 		use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
	 	*/
		printf("Error FT_OpenEx(%d), device %s\n", (int)ftStatus, m_deviceSerialNumber.c_str() );
		printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
		printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
		return false;
	}

	printf("Opened device %s\n", m_deviceSerialNumber.c_str() );

	if((ftStatus = FT_SetBaudRate(m_deviceHandle, m_baudRate)) != FT_OK) {
		printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int)ftStatus, m_deviceSerialNumber.c_str() );
		return false;
	}
	return true;
}

void FTDIConnection::Close() {
	if ( !m_deviceHandle )
		return;
	FT_Close(m_deviceHandle);
	m_deviceHandle = NULL;
	printf("Closed device %s\n", m_deviceSerialNumber.c_str());
}

bool operator==( const FTDIConnection& a, const FTDIConnection& b )
{
	return a.SerialNumber() == b.SerialNumber();
}