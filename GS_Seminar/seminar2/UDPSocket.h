#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include "SocketAddress.h"
#include "SocketUtil.h"

class UDPSocket
{
public:
	// UDP socket의 생성
	static UDPSocket* create(SocketUtil::AddressFamily family);
	~UDPSocket();

	// 주소 등록,
	// 더 이상 sockaddr을 통해 하지 않고, SocketAddress를 통해 수행한다.
	int bind(const SocketAddress& to_addr);

	// 해당 주소로 데이터를 전송
	// 마찬가지로 SocketAddress를 통해 수행한다.
	int sendTo(const void* data, int length, const SocketAddress& to_addr);

	// 상대방으로부터 데이터를 받아온다.
	// 받아온 데는는 data 버퍼에 저장된다.
	// 데이터를 전송한 상대방의 주소는 from_addr에 저장된다.
	// 받을 때 데이터의 최대 length를 지정한다.
	int receiveFrom(void* data, int max_length, SocketAddress& from_addr);
	
	int setNoneBlockingMode(bool flag);

private:
	UDPSocket(SOCKET socket) : _socket(socket) {}
	SOCKET _socket;
};

