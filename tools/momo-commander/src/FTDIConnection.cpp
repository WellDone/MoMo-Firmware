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
		printf("Make sure the VCP FTDI driver is not also installed, it conflicts with D2XX.\n");
		return false;
	}

	printf("Opened device %s\n", m_deviceSerialNumber.c_str() );

	if((ftStatus = FT_SetBaudRate(m_deviceHandle, m_baudRate)) != FT_OK) {
		printf("Error FT_SetBaudRate(%d), device = %s\n", (int)ftStatus, m_deviceSerialNumber.c_str() );
		return false;
	}

	if((ftStatus = FT_SetDataCharacteristics(m_deviceHandle, 0x8, 0x0, 0x0 )) != FT_OK) {
		printf("Error FT_SetDataCharacteristics(%d), device = %s\n", (int)ftStatus, m_deviceSerialNumber.c_str() );
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

std::string FTDIConnection::Read()
{
	unsigned char * pcBufRead = NULL;
	DWORD	dwRxSize = 0;
	DWORD   dwBytesRead;
	FT_STATUS	ftStatus;

	std::string ret;

	ftStatus = FT_GetQueueStatus(m_deviceHandle, &dwRxSize);
	if(ftStatus == FT_OK) {
		if (dwRxSize > 0) {
			pcBufRead = (unsigned char*)malloc(dwRxSize);
			ftStatus = FT_Read(m_deviceHandle, pcBufRead, dwRxSize, &dwBytesRead);
			if (ftStatus != FT_OK) {
				printf("Error FT_Read(%d)\n", (int)ftStatus);
			} else {
				//printf("FT_Read read %d bytes.", (int)dwBytesRead );
				ret = std::string( (const char*)pcBufRead, (size_t)dwRxSize );
			}
		}
	}
	else {
		printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);	
	}
	if (pcBufRead)
		free( pcBufRead );
	return ret;
}

bool FTDIConnection::Write( const std::string& data )
{
	FT_STATUS	ftStatus;
	DWORD   dwBytesWritten;

	ftStatus = FT_Write(m_deviceHandle, (char*)data.c_str(), data.size(), &dwBytesWritten);
	if (ftStatus != FT_OK) {
		printf("Error FT_Write(%d)\n", (int)ftStatus);
		return false;
	}
	if (dwBytesWritten != (DWORD)data.size() ) {
		printf( "FT_Write only wrote %d (of %d) bytes\n", 
		        (int)dwBytesWritten, 
		        (int)data.size() );
		return false;
	}
	return true;
}



bool operator==( const FTDIConnection& a, const FTDIConnection& b )
{
	return a.SerialNumber() == b.SerialNumber();
}