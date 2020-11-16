#include "UDPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"
#include <iostream>

// UDPSocket class�� �����ϱ� ���� ������ �Լ�
UDPSocket* UDPSocket::create(SocketUtil::AddressFamily family)
{
	// ���� UDP socket ����
	SOCKET s = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);

	if (s != INVALID_SOCKET)
	{
		// ���� SOCKET ������ �̿��Ͽ� ����ϱ� ���� UDPSocket ��ü�� �����Ͽ� ��ȯ
		return new UDPSocket(s);
	}
	else
	{
		SocketUtil::reportError("UDPSocket::create");
		return nullptr;
	}
}


// UDP ����� bind �Լ� interfacing
// SocketAddress�� ����Ѵ�.
int UDPSocket::bind(const SocketAddress& to_addr)
{
	// ���� bind �Լ��� ���� socket�� �ּҸ� �Ҵ��Ѵ�.
	int error = ::bind(_socket, &to_addr._sockaddr, to_addr.getSize());
	if (error != 0)
	{
		SocketUtil::reportError("UDPSocket::bind");
		return SocketUtil::getLastError();
	}
	
	return NO_ERROR;
}

// UDP ����� sendto �Լ� interfacing
// SocketAddress�� ����Ѵ�.
// data���� ���� �����͸� ��Ƽ� ���ڷ� �־��ش�.
// length���� data�� ���̸� �־��ش�.

int UDPSocket::sendTo(const void* data, int length, const SocketAddress& to_addr)
{
	int sent_length = ::sendto(_socket, static_cast<const char*>(data), length, 0, &to_addr._sockaddr, to_addr.getSize());
	if(sent_length <= 0)
	{
		// we'll return error as negative number to indicate less than requested amount of bytes sent...
		SocketUtil::reportError("UDPSocket::sendTo");
		return -SocketUtil::getLastError();
	}
	else
	{
		return sent_length;
	}
}

// UDP ����� recvfrom �Լ� interfacing
// SocketAddress�� ����Ѵ�.
// data�� ���� �����͸� �޾ƿ´�.
// max_length���� �޾ƿ� �ִ� ���̸� �����Ѵ�.
int UDPSocket::receiveFrom(void* data, int max_length, SocketAddress& from_addr)
{
	socklen_t fromLength = from_addr.getSize();
	
	int read_length = 
		::recvfrom(
			_socket,
			static_cast<char*>(data),
			max_length, 0, &from_addr._sockaddr, &fromLength);

	if (read_length >= 0)
	{
		return read_length;
	}
	else
	{
		int error = SocketUtil::getLastError();
		if (error == WSAEWOULDBLOCK)
		{
			return 0;
		}
		else if (error == WSAECONNRESET)
		{
			// (�߿�) ������ sendTo�� �ϰ� �־�� �Ѵ�
			// �� error�� client���� socket�� closed �Ǿ���,
			// ���� ȣ��Ʈ������ ������ ������ �ʾ��� �� �߻��Ѵ�.
			SocketUtil::log("client connection closed!");
			SocketUtil::reportError("client connection closed!");
			return -WSAECONNRESET;
		}
		else
		{
			SocketUtil::reportError("UDPSocket::ReceiveFrom");
			return -error;
		}
	}
}

UDPSocket::~UDPSocket()
{
#if _WIN32
	closesocket(_socket);
#else
	close( mSocket );
#endif
}


int UDPSocket::setNoneBlockingMode(bool flag)
{
#if _WIN32
	u_long arg = flag ? 1 : 0;
	int result = ioctlsocket(_socket, FIONBIO, &arg);
#else
	int flags = fcntl( mSocket, F_GETFL, 0 );
	flags = inShouldBeNonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK);
	int result = fcntl( mSocket, F_SETFL, flags );
#endif
	
	if( result == SOCKET_ERROR )
	{
		SocketUtil::reportError("UDPSocket::SetNonBlockingMode");
		return SocketUtil::getLastError();
	}
	else
	{
		return NO_ERROR;
	}
}

