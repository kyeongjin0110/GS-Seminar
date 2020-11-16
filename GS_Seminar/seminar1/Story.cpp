// OSI 7 계층
//	국제 표준 네트워크 모델로 프로토콜을 기능별로 나눈 것
//	Application
//	Presentation 
//	Session 
//	Transport		
//	Network		
//	Data Link	
//	Physical			


// OSI 5
//	Application		Http		code : https://github.com/NOLFXceptMe/http/blob/master/HTTPClient.cpp
//	Transport		TCP, UDP	code : http://lxr.linux.no/linux+v2.6.37/net/ipv4/tcp_ipv4.c
//	Network			IP
//	Data Link		Ethernet
//	Physical		USB, Bluetooth, Ethernet Physical Layer


// 주목해야 할 부분은 network(IP), transport layer(TCP, UDP)

// Network layer : IP (Internet Protocol)
//	Logical communication between hosts. (by packet switching)

// IP layer의 역할
//	송신자의 IP층 
//	패킷을 받아서 주소를 해석하고 경로를 결정하여 다음 호스트로 전송
//
//	수신자의 IP층
//	패킷에 있는 수신자의 주소와 자신의 주소를 비교
//	일치하지 않으면 다음 호스트로 전달
//	수신 주소와 일치하면 패킷을 TCP층에 전달
//	이 과정이 반복되어 결국 도착 위치까지 패킷이 도달하게 됨.

//  -IP 프로토콜은 패킷의 전송 경로에 대해서는 책임지지만,
//	패킷의 도달 여부, 손실 여부 등은 책임지지 않는다.

// IP routing process
//
//		end(1)		-		end(2)		-		end(3)
//		1 : x				1 : 1				1 : 2
//		2 : 2			 	2 : x				2 : 2
//		3 : 2			 	3 : 3				3 : x
//		4 : 2			 	4 : 4				4 : 3
//							  
//							  |
//
//							end(4)
//							1 : 2		
//							2 : 2
//							3 : 2
//							4 : x


// Transport layer : TCP, UDP
//	Logical communication between processes.

// Transport layer의 역할
//	양 종단 간 데이터를 주고 받는 데 있어서 신뢰성 혹은,
//  효율성을 갖는 전송을 지원해준다.
// 
//	-TCP
//		reliable, order, error check, port
//	-UDP
//		data integrity(check sum), port

// Packet's life  
//	End point	->	Router (store & forward) ->  Router	(s & f)	->	EndPoint	
//	Application									 Application	
//	Transport									 Transport	
//	Network			Network						 Network			Network		
//	Data Link		Data Link					 Data Link			Data Link	
//	Physical		Physical					 Physical			Physical	

// Port and processes
// 각 종단(end point)이 Ip layer 까지만 있다고 가정하면 store && forward를 통해서,
// packet은 목적지에 도달할 수 있을 것이다.
// 하지만, 도달한 data는 아직 신뢰성이 보장되지 않는 형태이다. (순서, 누락, 변질 등등)
// 그리고 만약 한 종단에서 여러 process를 통해 여러 개의 통신을 수행한다면 어떻게 될까??

// 같은 종단 내에서는 새로운 통신마다 다른 port를 할당한다.
// 그렇게 함으로써 여러 연결을 유지하며 통신이 가능한 것이다.


// Socket
// Application 수준에서의 종착점, (전화기와 비슷한 역할을 한다.)
// Transport layer에 접근하기 위한 표준된 방법.

/*

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#define WIN32_LEAN_AND_MEAN

using namespace std;

int main()
{
	// window socket parameter
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740506(v=vs.85).aspx


	// 주로 사용되는 parameter
	// -af
	//	AF_INET			IPv4
	//	AF_INET6		IPv6
	//
	// -type
	//	SOCK_STREAM		순서, 안정성을 보장하는 stream packet (TCP)
	//	SOCK_DGRAM		datagram packet (UDP)
	//
	// -protocol
	//  IPPROTO_UDP			요구사항 SOCK_DGRAM		UDP datagram을 래핑하는 packet
	//	IPPROTO_TCP			요구사항 SOCK_STREAM		TCP datagram을 래핑하는 packet
	//  IPPROTO_IP || 0		x						주어진 type에 맞는 protocol 사용


	// UDP 소켓
	SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// TCP 소켓
	SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

	// OS 간 차이
	// 1. return value
	//	-window
	//		window socket의 return value SOCKET 은 포인터이고,
	//		socket의 상태를 저장하는 위치를 가르키고 있다.
	//	-linux
	//		linux socket의 return value는 int로써 파일 디스크립터 번호를 나타낸다.
	//		socket의 return의 의미가 다르다는 것은 큰 약점이다.

	// 2. header
	//	-window
	//		window에서 socket을 사용하기 위해
	//		#include <WinSock2.h> 을 선언해야 한다.
	//			윈도우에는 <winsock.h> 라는 구 버전이 있는데, 여러모로 불안정하다.
	//			하지만 이 떄문에 winsock2.h와 충돌이 생기고 이를 방지하기 위해,
	//			다음 메크로를 사용해야 한다.
	//			#define WIN32_LEAN_AND_MEAN
	//	
	//		추가적으로 address 변환을 위해 다음 헤더를 사용해야 한다.
	//		#include <Ws2tcpip.h>

	//	-linux
	//		#include <sys/socket.h>,
	//		#include <netinet/in.h>,
	//		#include <arpa/inet.h>
	//		#include <netdb.h>
	//		#include <errno.h>

	// 3. library
	//	-window
	//		WSAStartup() 함수로 socket library 활성화
	//		WSACleanup() 함수로 정리
	//	-linux
	//		default로 다 사용 가능

	// 4. error
	//	-window
	//		SOCKET_ERROR MECRO, WSAGetLastError();
	//	-linux
	//		errno MECRO, perror(), stderror() .. 사용
	// 두 경우 모두 함수의 return으로 error의 메크로 값이 반환되는데,
	// 현재 실행중인 thread의 에러 값이 반환되므로, 즉시 체크하는 것이 중요하다.

	return 0;
}

*/