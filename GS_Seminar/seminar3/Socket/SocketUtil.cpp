#include "SocketUtil.h"
#include <iostream>

bool SocketUtil::staticInit()
{
#if _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ( iResult != NO_ERROR )
	{
		reportError ("Starting Up");
		return false;
	}
#endif
	return true;
}

void SocketUtil::cleanUp()
{
#if _WIN32
	WSACleanup();
#endif
}


void SocketUtil::reportError( const char* inOperationDesc )
{
#if _WIN32
	LPVOID lpMsgBuf;
	DWORD errorNum = GetLastError();
	
	FormatMessage(
				  FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  errorNum,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR) &lpMsgBuf,
				  0, NULL );
	
	log( "Error %s: %d - %s", inOperationDesc, errorNum, lpMsgBuf );
#else
	log( "Error: %hs", inOperationDesc );
#endif
}

int SocketUtil::getLastError()
{
#if _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void SocketUtil::log(const char* inFormat, ...)
{
	//not thread safe...
	static char temp[4096];

	va_list args;
	va_start(args, inFormat);

#if _WIN32
	_vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
	vsnprintf(temp, 4096, inFormat, args);
#endif
	OutputDebugString(temp);
	OutputDebugString("\n");
	std::cout << temp << std::endl;
}

