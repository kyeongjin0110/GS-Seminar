#pragma once

#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <stdint.h>
#include <Windows.h>
#include "SocketAddress.h"
#include "SocketUtil.h"

class TCPSocket
{
public:
	// TCP socketÀÇ »ý¼º
	static TCPSocket*				create(SocketUtil::AddressFamily family);
	~TCPSocket();

	int								bind(const SocketAddress& to_addr);
	int								connect(const SocketAddress& address);
	int								listen(int back_num = 32);
	TCPSocket*						accept(SocketAddress& from_addr);
	int32_t							send(const void* data, size_t length);
	int32_t							receive(void* data, size_t length);

private:

	TCPSocket( SOCKET socket ) : _socket(socket) {}
	SOCKET		_socket;
};

