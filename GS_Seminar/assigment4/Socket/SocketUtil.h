#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <string>
#include <vector>

class SocketUtil
{
public:
	enum AddressFamily
	{
		INET = AF_INET,
		INET6 = AF_INET6
	};

	// 윈도우 socket을 사용하기 위한 초기화 작업
	static bool			staticInit();

	// 윈도우 socket 사용 후 정리
	static void			cleanUp();

	// 윈도우 socket 에러 처리
	static void			reportError( const char* inOperationDesc );
	static int			getLastError();
	static void			log(const char* inFormat, ...);
};