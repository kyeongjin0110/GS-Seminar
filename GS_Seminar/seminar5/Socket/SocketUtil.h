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

	// ������ socket�� ����ϱ� ���� �ʱ�ȭ �۾�
	static bool			staticInit();

	// ������ socket ��� �� ����
	static void			cleanUp();

	// ������ socket ���� ó��
	static void			reportError( const char* inOperationDesc );
	static int			getLastError();
	static void			log(const char* inFormat, ...);
};