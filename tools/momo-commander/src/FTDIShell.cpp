#include "FTDIShell.h"

#define BUF_SIZE 10

void DoRead( const FTDIShell& shell ) { 
	/* Read */
	unsigned char * pcBufRead = NULL;
	DWORD	dwRxSize = 0;
	DWORD   dwBytesRead;
	FT_STATUS	ftStatus;

	ftStatus = FT_GetQueueStatus(shell.Connection().DeviceHandle(), &dwRxSize);
	if(ftStatus == FT_OK) {
		if (dwRxSize == 0)
			return;
		pcBufRead = (unsigned char*)malloc(dwRxSize+1);
		ftStatus = FT_Read(shell.Connection().DeviceHandle(), pcBufRead, dwRxSize, &dwBytesRead);
		if (ftStatus != FT_OK) {
			printf("Error FT_Read(%d)\n", (int)ftStatus);
			return;
		}
		printf("FT_Read read %d bytes.", (int)dwBytesRead );
		pcBufRead[dwRxSize] = '\0';
		printf("%s", pcBufRead);
	}
	else {
		printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);	
	}
	if (pcBufRead)
		free( pcBufRead );
}

void* ReadLoop( void* arg ) {
	const FTDIShell& shell = *( (const FTDIShell*)arg );

	EVENT_HANDLE eh;
	FT_STATUS	ftStatus;

	pthread_mutex_init(&eh.eMutex, NULL);
	pthread_cond_init(&eh.eCondVar, NULL);

	ftStatus = FT_SetEventNotification(shell.Connection().DeviceHandle(), FT_EVENT_RXCHAR, (PVOID)&eh);
	if(ftStatus != FT_OK) {
		printf("Failed to set events\n");
		return NULL;
	}

	while (1) {
		pthread_mutex_lock(&eh.eMutex);
		pthread_cond_wait(&eh.eCondVar, &eh.eMutex);
		pthread_mutex_unlock(&eh.eMutex);

		DoRead( shell );
	}
}

void FTDIShell::Activate() {
	if ( !m_connection.Active() )
		m_connection.Open();
	pthread_create(&m_readerThread, NULL, &ReadLoop, (void*)this);
}