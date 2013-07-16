#include "FTDIManager.h"

#define BUF_SIZE 0x10
#define MAX_DEVICES		5

FTDIManager::FTDIManager()
{}

FTDIManager::~FTDIManager()
{}

bool FTDIManager::Refresh() {
	FT_STATUS	ftStatus;
	char cBufLD[MAX_DEVICES][64];
	char * 	pcBufLD[MAX_DEVICES + 1];
	int	i, deviceCount;
	
	if ( ActiveConnectionCount() > 0 ) {
		printf("There are active connections, can't refresh the list.");
		return false;
	}

	m_connections.clear(); // Closes any currently open connections!!
	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;
	
	ftStatus = FT_ListDevices(pcBufLD, &deviceCount, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
	
	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
		return false;
	}

	for (i = 0; i < MAX_DEVICES && i < deviceCount; ++i)
	{
		m_connections.push_back( cBufLD[i] );
	}

	return true;
}

unsigned int FTDIManager::ActiveConnectionCount() {
	unsigned int count = 0;
	FTDIConnectionIterator i = m_connections.begin();
	while (i != m_connections.end() ) {
		if ( i->Active() )
			++count;
		++i;
	}
	return count;
}
/*
static void dumpBuffer(unsigned char *buffer, int elements)
{
	int j;

	printf(" [");
	for (j = 0; j < elements; j++)
	{
		if (j > 0)
			printf(", ");
		printf("0x%02X", (unsigned int)buffer[j]);
	}
	printf("]\n");
}
int test( int argc, char **argv ) {
	unsigned char cBufWrite[BUF_SIZE];
	unsigned char * pcBufRead = NULL;
	FT_STATUS ftStatus;
	DWORD dwBytesWritten;
	DWORD dwRxSize;
	DWORD dwBytesRead;


	printf("Calling FT_Write with this write-buffer:\n");
	dumpBuffer(cBufWrite, BUF_SIZE);
	
	ftStatus = FT_Write(ftHandle, cBufWrite, BUF_SIZE, &dwBytesWritten);
	if (ftStatus != FT_OK) {
		printf("Error FT_Write(%d)\n", (int)ftStatus);
		return CMDRES::kError;
	}
	if (dwBytesWritten != (DWORD)BUF_SIZE) {
		printf("FT_Write only wrote %d (of %d) bytes\n", 
		       (int)dwBytesWritten, 
		       BUF_SIZE);
		return CMDRES::kError;
	}
	sleep(1);
	return CMDRES::kSuccess;
	
	dwRxSize = 0;
	while ((dwRxSize < BUF_SIZE) && (ftStatus == FT_OK)) {
		ftStatus = FT_GetQueueStatus(ftHandle, &dwRxSize);
	}
	if(ftStatus == FT_OK) {
		pcBufRead = (unsigned char*)realloc(pcBufRead, dwRxSize);
		memset(pcBufRead, 0xFF, dwRxSize);
		printf("Calling FT_Read with this read-buffer:\n");
		dumpBuffer(pcBufRead, dwRxSize);
		ftStatus = FT_Read(ftHandle, pcBufRead, dwRxSize, &dwBytesRead);
		if (ftStatus != FT_OK) {
			printf("Error FT_Read(%d)\n", (int)ftStatus);
			return CMDRES::kError;
		}
		if (dwBytesRead != dwRxSize) {
			printf("FT_Read only read %d (of %d) bytes\n",
			       (int)dwBytesRead,
			       (int)dwRxSize);
			return CMDRES::kError;
		}
		printf("FT_Read read %d bytes.  Read-buffer is now:\n",
		       (int)dwBytesRead);
		dumpBuffer(pcBufRead, (int)dwBytesRead);
		if (0 != memcmp(cBufWrite, pcBufRead, BUF_SIZE)) {
			printf("Error: read-buffer does not match write-buffer.\n");
			return CMDRES::kError;
		}
		printf("%s test passed.\n", cBufLD[current_device]);
		return CMDRES::kSuccess;
	}
	else {
		printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);	
		return CMDRES::kError;
	}
}
*/