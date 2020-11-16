#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <stdint.h>
#include <string>
#include <memory>

class SocketAddress
{
public:
	// Static factory 함수 
	// 사용 -> createFromString("127.0.0.1:8000");
	static SocketAddress* createFromString(const std::string& addr);

	// IP(unsigned int 형태), port를 이용한 생성자
	SocketAddress(uint32_t ip, uint16_t port)
	{
		asSockAddrIn()->sin_family = AF_INET;
		asIPv4() = htonl(ip);
		asSockAddrIn()->sin_port = htons(port);
	}

	// sockaddr를 이용한 생성자
	SocketAddress(const sockaddr& addr)
	{
		memcpy(&_sockaddr, &addr, sizeof(sockaddr));
	}

	// Default 생성자
	SocketAddress()
	{
		asSockAddrIn()->sin_family = AF_INET;
		asIPv4() = INADDR_ANY;
		asSockAddrIn()->sin_port = 0;
	}

	// 대입 연산자
	bool operator==(const SocketAddress& other) const
	{
		return
			(_sockaddr.sa_family == AF_INET) &&
			(asSockAddrIn()->sin_port == other.asSockAddrIn()->sin_port) &&
			(asIPv4() == other.asIPv4());
	}

	// Hash code
	// SocketAddress 내부 데이터를 이용하여 유일한 값을 만든다.
	size_t getHash() const
	{
		return 
			(asIPv4()) |
			((static_cast< uint32_t >(asSockAddrIn()->sin_port)) << 13) |
			(_sockaddr.sa_family);
	}

	uint32_t				getSize()	const	{ return sizeof(sockaddr); }

	std::string				toString()	const;

private:
	friend class UDPSocket;
	friend class TCPSocket;

	sockaddr _sockaddr;

#if _WIN32
	// IPv4로 반환 (참조를 리턴하기 때문에 실제 객체의 데이터를 변경할 수 있다.)
	uint32_t&			asIPv4()				{ return *reinterpret_cast< uint32_t* >( &asSockAddrIn()->sin_addr.S_un.S_addr ); }
	const uint32_t&		asIPv4()		const	{ return *reinterpret_cast< const uint32_t* >( &asSockAddrIn()->sin_addr.S_un.S_addr ); }
#else
	uint32_t&			asIPv4()				{ return asSockAddrIn()->sin_addr.s_addr; }
	const uint32_t&		asIPv4()		const	{ return asSockAddrIn()->sin_addr.s_addr; }
#endif

	// sockaddr_in으로 반환 (참조를 리턴하기 때문에 실제 객체의 데이터를 변경할 수 있다.)
	sockaddr_in*		asSockAddrIn()			{ return reinterpret_cast< sockaddr_in* >( &_sockaddr); }
	const sockaddr_in*	asSockAddrIn()	const	{ return reinterpret_cast< const sockaddr_in* >( &_sockaddr); }

};

// STL hash를 위한 지원
namespace std
{
	template<> struct hash< SocketAddress >
	{
		size_t operator()( const SocketAddress& addr ) const
		{
			return addr.getHash();
		}
	};
}