#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ftd2xx.h"

namespace CMDRES {
	enum command_result_code {
		kSuccess = 0,
		kError = -1,
		kQuit = 256
	};
}

#define BUF_SIZE 0x10
#define MAX_DEVICES		5

unsigned int ctoi( char c ) {
	if ( c < '0' || c > '9')
		return 0;
	else
		return c - '0';
}

char cBufLD[MAX_DEVICES][64];
int  iNumDevs = 0;

int current_device = -1;
FT_HANDLE	ftHandle;

int populateDeviceList() {
	FT_STATUS	ftStatus;
	char * 	pcBufLD[MAX_DEVICES + 1];
	int	i;
	
	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;
	
	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
	
	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
		return CMDRES::kError;
	}
	return CMDRES::kSuccess;
}
int listDevices() {
	int i;
	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ); i++) {
		printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
	}
	return CMDRES::kSuccess;
}
int connectToDevice( int deviceID ) {
	FT_STATUS ftStatus;
	
	if ( deviceID > MAX_DEVICES || deviceID > iNumDevs ) {
		printf("Bad device ID.\n");
		return CMDRES::kError;
	}

	if((ftStatus = FT_OpenEx(cBufLD[deviceID], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle)) != FT_OK){
		/* 
			This can fail if the ftdi_sio driver is loaded
	 		use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
	 	*/
		printf("Error FT_OpenEx(%d), device %d\n", (int)ftStatus, deviceID);
		printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
		printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
		return CMDRES::kError;
	}

	current_device = deviceID;
	printf("Opened device %s\n", cBufLD[current_device]);

	if((ftStatus = FT_SetBaudRate(ftHandle, 115200)) != FT_OK) {
		printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int)ftStatus, cBufLD[current_device]);
		return CMDRES::kError;
	}
	return CMDRES::kSuccess;
}

int disconnect() {
	if ( current_device == -1 )
		return CMDRES::kSuccess;
	FT_Close(ftHandle);
	printf("Closed device %s\n", cBufLD[current_device]);
	current_device = -1;
	return CMDRES::kSuccess;
}

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
	
	/* Write */
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
	
	/* Read */
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

int parse(char *line, char **argv)
{
	int argc = 0;
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          ++ argc;
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = '\0';                 /* mark the end of argument list  */
 	return argc-1;
}
int execute( int argc, char **argv ) {
	if ( strcmp( argv[0], "exit" ) == 0 ) {
		if ( current_device == -1 )
			return CMDRES::kQuit;
		else
			return disconnect();
	}

	if ( current_device == -1 ) {
		if ( strcmp( argv[0], "list" ) == 0 )
			return listDevices();
		else if ( strcmp( argv[0], "refresh" ) == 0 )
			return populateDeviceList();
		else if ( strcmp( argv[0], "connect" ) == 0 ) {
			if ( argc == 2 )
				return connectToDevice( ctoi( *argv[1] ) );
			else
				return CMDRES::kError;
		}
	} else {
		if ( strcmp( argv[0], "test" ) == 0 )
			return test( argc, argv );
	}
	printf( "Unknown command '%s'", argv[0] );
	return CMDRES::kError;
}

inline void prompt() {
	if ( current_device == -1 )
		printf("momo-commander$ ");
	else
		printf("%s$ ", cBufLD[current_device]);
}
int do_shell() {
	char line[1024];
	char  *argv[8];;
	int argc;
	line[0] = '\0';
	do {
		if (*line == '\0' || *line == '\n') {
			prompt();
			continue;
		}
		argc = parse(line, argv);
		switch( execute(argc, argv) ) {
			case CMDRES::kQuit:
				printf("GOODBYE!!\n");
				return 0;
				break;
			case CMDRES::kError:
				printf("(!)\n");
				break;
			default:
				break;
		};
		prompt();
	} while (fgets(line, sizeof(line), stdin) != 0);
}

int init() {
	int res = populateDeviceList();
	if ( res != CMDRES::kSuccess )
		return res;
	res = listDevices();
	if ( res != CMDRES::kSuccess )
		return res;
	if ( iNumDevs == 0 ) {
		printf("No MoMo devices detected!\n");
		return CMDRES::kQuit;
	}
	else if ( iNumDevs == 1 )
	{
		return connectToDevice(0);
	}
	return CMDRES::kSuccess;
}

int main( int argc, char **argv ) {
	if ( init() != CMDRES::kSuccess ) {
		return 1;
	}
	if ( argc == 1 ) {
		return do_shell();
	}
	else if ( argc == 2) {
		if ( strcmp(argv[0],"config") == 0 ) {
			// configure things
		}
	}
	return 0;
}