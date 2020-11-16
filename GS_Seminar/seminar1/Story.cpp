// OSI 7 ����
//	���� ǥ�� ��Ʈ��ũ �𵨷� ���������� ��ɺ��� ���� ��
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


// �ָ��ؾ� �� �κ��� network(IP), transport layer(TCP, UDP)

// Network layer : IP (Internet Protocol)
//	Logical communication between hosts. (by packet switching)

// IP layer�� ����
//	�۽����� IP�� 
//	��Ŷ�� �޾Ƽ� �ּҸ� �ؼ��ϰ� ��θ� �����Ͽ� ���� ȣ��Ʈ�� ����
//
//	�������� IP��
//	��Ŷ�� �ִ� �������� �ּҿ� �ڽ��� �ּҸ� ��
//	��ġ���� ������ ���� ȣ��Ʈ�� ����
//	���� �ּҿ� ��ġ�ϸ� ��Ŷ�� TCP���� ����
//	�� ������ �ݺ��Ǿ� �ᱹ ���� ��ġ���� ��Ŷ�� �����ϰ� ��.

//  -IP ���������� ��Ŷ�� ���� ��ο� ���ؼ��� å��������,
//	��Ŷ�� ���� ����, �ս� ���� ���� å������ �ʴ´�.

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

// Transport layer�� ����
//	�� ���� �� �����͸� �ְ� �޴� �� �־ �ŷڼ� Ȥ��,
//  ȿ������ ���� ������ �������ش�.
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
// �� ����(end point)�� Ip layer ������ �ִٰ� �����ϸ� store && forward�� ���ؼ�,
// packet�� �������� ������ �� ���� ���̴�.
// ������, ������ data�� ���� �ŷڼ��� ������� �ʴ� �����̴�. (����, ����, ���� ���)
// �׸��� ���� �� ���ܿ��� ���� process�� ���� ���� ���� ����� �����Ѵٸ� ��� �ɱ�??

// ���� ���� �������� ���ο� ��Ÿ��� �ٸ� port�� �Ҵ��Ѵ�.
// �׷��� �����ν� ���� ������ �����ϸ� ����� ������ ���̴�.


// Socket
// Application ���ؿ����� ������, (��ȭ��� ����� ������ �Ѵ�.)
// Transport layer�� �����ϱ� ���� ǥ�ص� ���.

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


	// �ַ� ���Ǵ� parameter
	// -af
	//	AF_INET			IPv4
	//	AF_INET6		IPv6
	//
	// -type
	//	SOCK_STREAM		����, �������� �����ϴ� stream packet (TCP)
	//	SOCK_DGRAM		datagram packet (UDP)
	//
	// -protocol
	//  IPPROTO_UDP			�䱸���� SOCK_DGRAM		UDP datagram�� �����ϴ� packet
	//	IPPROTO_TCP			�䱸���� SOCK_STREAM		TCP datagram�� �����ϴ� packet
	//  IPPROTO_IP || 0		x						�־��� type�� �´� protocol ���


	// UDP ����
	SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// TCP ����
	SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

	// OS �� ����
	// 1. return value
	//	-window
	//		window socket�� return value SOCKET �� �������̰�,
	//		socket�� ���¸� �����ϴ� ��ġ�� ����Ű�� �ִ�.
	//	-linux
	//		linux socket�� return value�� int�ν� ���� ��ũ���� ��ȣ�� ��Ÿ����.
	//		socket�� return�� �ǹ̰� �ٸ��ٴ� ���� ū �����̴�.

	// 2. header
	//	-window
	//		window���� socket�� ����ϱ� ����
	//		#include <WinSock2.h> �� �����ؾ� �Ѵ�.
	//			�����쿡�� <winsock.h> ��� �� ������ �ִµ�, ������� �Ҿ����ϴ�.
	//			������ �� ������ winsock2.h�� �浹�� ����� �̸� �����ϱ� ����,
	//			���� ��ũ�θ� ����ؾ� �Ѵ�.
	//			#define WIN32_LEAN_AND_MEAN
	//	
	//		�߰������� address ��ȯ�� ���� ���� ����� ����ؾ� �Ѵ�.
	//		#include <Ws2tcpip.h>

	//	-linux
	//		#include <sys/socket.h>,
	//		#include <netinet/in.h>,
	//		#include <arpa/inet.h>
	//		#include <netdb.h>
	//		#include <errno.h>

	// 3. library
	//	-window
	//		WSAStartup() �Լ��� socket library Ȱ��ȭ
	//		WSACleanup() �Լ��� ����
	//	-linux
	//		default�� �� ��� ����

	// 4. error
	//	-window
	//		SOCKET_ERROR MECRO, WSAGetLastError();
	//	-linux
	//		errno MECRO, perror(), stderror() .. ���
	// �� ��� ��� �Լ��� return���� error�� ��ũ�� ���� ��ȯ�Ǵµ�,
	// ���� �������� thread�� ���� ���� ��ȯ�ǹǷ�, ��� üũ�ϴ� ���� �߿��ϴ�.

	return 0;
}

*/