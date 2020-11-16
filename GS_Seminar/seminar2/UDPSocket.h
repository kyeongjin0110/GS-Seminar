#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include "SocketAddress.h"
#include "SocketUtil.h"

class UDPSocket
{
public:
	// UDP socket�� ����
	static UDPSocket* create(SocketUtil::AddressFamily family);
	~UDPSocket();

	// �ּ� ���,
	// �� �̻� sockaddr�� ���� ���� �ʰ�, SocketAddress�� ���� �����Ѵ�.
	int bind(const SocketAddress& to_addr);

	// �ش� �ּҷ� �����͸� ����
	// ���������� SocketAddress�� ���� �����Ѵ�.
	int sendTo(const void* data, int length, const SocketAddress& to_addr);

	// �������κ��� �����͸� �޾ƿ´�.
	// �޾ƿ� ���´� data ���ۿ� ����ȴ�.
	// �����͸� ������ ������ �ּҴ� from_addr�� ����ȴ�.
	// ���� �� �������� �ִ� length�� �����Ѵ�.
	int receiveFrom(void* data, int max_length, SocketAddress& from_addr);
	
	int setNoneBlockingMode(bool flag);

private:
	UDPSocket(SOCKET socket) : _socket(socket) {}
	SOCKET _socket;
};

