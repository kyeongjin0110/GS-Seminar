#include "UDPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"
#include <iostream>

// UDPSocket class를 생성하기 위한 유일한 함수
UDPSocket* UDPSocket::create(SocketUtil::AddressFamily family)
{
	// 실제 UDP socket 생성
	SOCKET s = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);

	if (s != INVALID_SOCKET)
	{
		// 열린 SOCKET 변수를 이용하여 사용하기 편리한 UDPSocket 객체를 생성하여 반환
		return new UDPSocket(s);
	}
	else
	{
		SocketUtil::reportError("UDPSocket::create");
		return nullptr;
	}
}


// UDP 통신의 bind 함수 interfacing
// SocketAddress를 사용한다.
int UDPSocket::bind(const SocketAddress& to_addr)
{
	// 실제 bind 함수를 통해 socket에 주소를 할당한다.
	int error = ::bind(_socket, &to_addr._sockaddr, to_addr.getSize());
	if (error != 0)
	{
		SocketUtil::reportError("UDPSocket::bind");
		return SocketUtil::getLastError();
	}
	
	return NO_ERROR;
}

// UDP 통신의 sendto 함수 interfacing
// SocketAddress를 사용한다.
// data에는 보낼 데이터를 담아서 인자로 넣어준다.
// length에는 data의 길이를 넣어준다.

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

// UDP 통신의 recvfrom 함수 interfacing
// SocketAddress를 사용한다.
// data를 통해 데이터를 받아온다.
// max_length에는 받아올 최대 길이를 지정한다.
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
			// (중요) 하지만 sendTo를 하고 있어야 한다
			// 이 error는 client에서 socket이 closed 되었고,
			// 현재 호스트에서는 연결이 끊기지 않았을 때 발생한다.
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

