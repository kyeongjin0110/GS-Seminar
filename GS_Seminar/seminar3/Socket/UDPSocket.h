#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include "SocketAddress.h"
#include "SocketUtil.h"

// UDP ����� ������ ������ �� �ִ�. 
// packet -> ����
// socket -> ��ü��
// ip address -> �ּ�

// UDP ����� ������ ���� �������� �̷������.
//
//	   Server								   Client
//
//	1. create socket(��ü��)					1. create socket(��ü��)
//	2. bind(�ּ� ���)				 
//	4. recvfrom(������ ��ٸ�)			<-		3. sendto(�ּҰ� ���� ������ ����)
//	5. sendto(�ּҰ� ���� ������ ����)	->		6. recvfrom(������ ��ٸ�)
//	7. close								7. close

// UDP ����� ������ �����ϱ� ������, ��żӵ��� UDP�� �� ������.

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


	SOCKET _socket;

private:
	UDPSocket(SOCKET socket) : _socket(socket) {}
	//SOCKET _socket;
};

