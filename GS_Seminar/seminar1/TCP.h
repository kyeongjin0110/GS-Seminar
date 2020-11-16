#pragma once


// TCP
// TCP 통신은 전화기와 굉장히 흡사한 과정으로 이루어진다.
// 그러므로 전화를 통해 TCP의 동작원리를 설명하고자 한다.

// 전화를 거는 과정을 생각해보자.

// 수신자							송신자
//
// 전화기 구매					전화기 구매	
// 전화번호 등록					전화번호 입력 && 전화를 건다.
// 전화기를 전화 케이블에 연결		통화한다.
// 전화 벨이 울리면 수화기를 든다.	
// 통화한다.

// Server						Client
// 1. socket 생성				1. socket 생성
// 2. bind						4. connect
// 3. listen					6. send/recv
// 5. accept					
// 6. send/recv


#include <string>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#include <stdio.h>

using namespace std;

namespace _ex2
{
	const int BufferSize = 256;
	const int Port = 10000;
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
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			cout << "socket() 생성 실패. error code : " << WSAGetLastError() << endl;
		return sock;
	}

	void TCPServer()
	{
		cout << "TCP Server" << endl;

		// Window socket 초기화
		initWindowSocket();


		char buffer[128];
		struct sockaddr_in server_addr, client_addr;
		char temp[20];
		SOCKET server_socket, client_socket;
		int len, msg_size;

		
		server_socket = createSocket();
		
		memset(&server_addr, 0x00, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(server_addr.sin_addr.s_addr));

		if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0)
		{
			printf("Server : Can't bind local address.\n");
			exit(0);
		}

		//소켓을 수동 대기모드로 설정
		if (listen(server_socket, 5) < 0)
		{
			printf("Server : Can't listening connect.\n");
			exit(0);
		}

		memset(buffer, 0x00, sizeof(buffer));
		printf("Server : wating connection request.\n");
		len = sizeof(client_addr);
		while (1)
		{
			client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &len);
			if (client_socket < 0)
			{
				printf("Server: accept failed.\n");
				exit(0);
			}


			inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
			printf("Server : %s client connected.\n", temp);

			msg_size = recv(client_socket, buffer, 1024, 0);
		}


		closesocket(server_socket);
		WSACleanup();
	}

	void TCPClient()
	{
		cout << "TCP Client" << endl;

		// Window socket 초기화
		initWindowSocket();

		SOCKET socket = createSocket();

		struct sockaddr_in address;
		memset((char *)&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(address.sin_addr.s_addr));

		if (SOCKET_ERROR == connect(socket, (struct sockaddr*)&address, sizeof(address)))
		{
			cout << "접속 실패"<<endl;
			exit(1);
		}

		std::string message = "Hello TCP world!";
		int bytes_sent = send(socket, message.c_str(), message.length(), 0);
		if (bytes_sent < 0)
		{
			cout << "TCPSocket::send error" << endl;
			exit(1);
		}

		closesocket(socket);
		WSACleanup();
	}
}

