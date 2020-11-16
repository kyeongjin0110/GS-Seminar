#pragma once
// UDP
// UDP 통신은 편지와 굉장히 흡사한 과정으로 이루어진다.
// 그러므로 편지를 통해 UDP의 동작원리를 설명하고자 한다.

// 편지 = packet
// 주소 = socket address
// 우체통 = socket

// 편지를 보내기 위해서는 편지에 보낼 대상의 주소를 적어야 한다.
// 하지만 편지의 특성상 보내고 나서 상대방의 수신여부를 확인할 방법이 없다.
// 또한 중간에 편지가 분실될 수도 있다.
// 편지는 신뢰할 수 없는 전송방법이고,
// 이와 마찬가지로 UDP또한 그러하다.


// UDP 통신은 다음과 같은 과정으로 이루어진다.
//
//	   Server								   Client
//
//	1. create socket(우체통)					1. create socket(우체통)
//	2. bind(주소 등록)				 
//	4. recvfrom(편지를 기다림)			<-		3. sendto(주소가 적힌 편지를 보냄)
//	4. sendto(주소가 적힌 편지를 보냄)	->		5. recvfrom(편지를 기다림)
//	5. close								6. close
//

// UDP에서 서버와 클라이언트는 연결되어 있지 않다.
// 즉 TCP에서의 listen, accept와 같은 과정은 불필요하다.
// 그러므로 UDP에서는 서버건 클라이언트건 하나의 소켓만 있으면 된다.
// TCP에서는 연결을 유지해야 하는 특성 때문에,
// 하나의 서버가 10개의 클라이언트와 통신하기 위해서는
// 문지기 역할의 소켓 외에도 10개의 소켓이 더 필요하다.
// 하지만 UDP에서 socket은 우체통과 같은 역할을 하기 때문에,
// 하나만 갖고도 여러 client들에게 보내기, 받기를 할 수 있다.

// 즉 서버와 클라이언트 간의 역할 차이가 별로 없다.
// 유일한 차이는 bind인데 여기서 궁금증이 생길 수 있는 부분은,
// 왜 클라이언트의 경우에는 bind를 통해 주소할당을 따로 해주지 않느냐이다.

// 클라이언트의 경우, 서버에 전송할 때
// sendto 함수호출 시점까지 주소정보가 할당되지 않았다면,
// 첫 sendto를 하는 시점에 호스트의 ip번호와 사용하지 않는 port번호가 할당된다.

// 그리고 recvfrom 함수를 통해 주소 정보와 함께 packet을 받았기 때문에,
// 이후에 서버 측에서 다시 응답을 보낼 때는, 해당 주소로 다시 보내주면 된다.

#include <string>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <stdio.h>
using namespace std;

namespace _ex1
{
	const int BufferSize = 256;
	const int Port = 9999;
	const std::string IP = "127.0.0.1";

	// window에서 socket을 사용하기 위한 초기작업
	void initWindowSocket() 
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR)
			cout << "실패. error code : " << WSAGetLastError() << endl;
	}

	// UDP socket 생성 과정
	SOCKET createSocket()
	{
		SOCKET sock;
		// UDP socket 생성
		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			cout << "socket() 생성 실패. error code : " << WSAGetLastError() << endl;
		return sock;
	}

	// UDP 통신은 목적지에 대한 정보까지 포함해서 함수(sendto)를 호출한다.
	// UDP의 경우 연결을 유지하고 있을 필요가 없기 때문에, 이러한 방법을 사용한다.
	void sendProcess(struct sockaddr_in* address, SOCKET socket, string message)
	{
		// Message 전송
		int ret =
			sendto(
				socket,								// 우체통에
				message.c_str(),					// 편지를
				message.size(),
				0,
				(struct sockaddr *) address,		// <- 주소로
				sizeof(struct sockaddr_in));		// 보낸다.

		// 오류 시 작업
		if (ret == SOCKET_ERROR)
			cout << "sendto() 전속 실패  error code : " << WSAGetLastError() << endl;
	}

	// UDP 통신은 recvfrom 함수를 통해 data와 data가 온 주소에 대한 정보까지 얻어온다.
	void recvProcess(struct sockaddr_in* address, SOCKET socket)
	{
		// Message 수신
		char recvMessage[BufferSize] = { 0 };
		int len = sizeof(struct sockaddr_in);

		int ret =
			recvfrom(
				socket,								// 우체통에서
				recvMessage,						// 편지를
				BufferSize,
				0,
				(struct sockaddr *) address,		// <- 주소로 된
				&len);								// 꺼낸다.

		// 오류 시 작업
		if (ret == SOCKET_ERROR)
			cout << "recvfrom() 실패  error code : " << WSAGetLastError() << endl;

		// 응답 출력
		cout << "size : " << ret << "  data : " << recvMessage << endl;
	}

	void UDPClient()
	{
		cout << "UDP Client" << endl;

		// Window socket 초기화
		initWindowSocket();

		// Socket 생성
		SOCKET mySocket = createSocket();
		
		// Address 구조체 초기화
		struct sockaddr_in otherAddress;
		memset((char *)&otherAddress, 0, sizeof(otherAddress));
		otherAddress.sin_family = AF_INET;
		otherAddress.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(otherAddress.sin_addr.s_addr));


		// 통신 시작
		while (1)
		{
			cout<<"Message 입력 : ";
			string message;
			cin >> message;

			// Server로 송신
			sendProcess(&otherAddress, mySocket, message);
			
			// Server로부터 수신
			recvProcess(&otherAddress, mySocket);
		}

		closesocket(mySocket);
		WSACleanup();
	}

	void UDPServer()
	{
		cout << "UDP Server" << endl;
	
		// Window socket 초기화
		initWindowSocket();

		// Socket 생성
		SOCKET mySocket = createSocket();

		// Address 구조체 초기화
		struct sockaddr_in myAddress;
		myAddress.sin_family = AF_INET;
		myAddress.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(myAddress.sin_addr.s_addr));

		

		// Socket에 주소 할당
		if (bind(mySocket, (struct sockaddr*)&myAddress, sizeof(myAddress)) == SOCKET_ERROR)
			std::cout << "주소 할당 실패" << endl;

		while (1)
		{
			int len = sizeof(struct sockaddr_in);
			
			// Client로부터 messge 수신
			recvProcess(&myAddress, mySocket);

			// Client로 message 송신
			sendProcess(&myAddress, mySocket, "SERVER MSG");
		}

		closesocket(mySocket);
		WSACleanup();
	}
}

