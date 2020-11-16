#pragma once


// TCP
// TCP ����� ��ȭ��� ������ ����� �������� �̷������.
// �׷��Ƿ� ��ȭ�� ���� TCP�� ���ۿ����� �����ϰ��� �Ѵ�.

// ��ȭ�� �Ŵ� ������ �����غ���.

// ������							�۽���
//
// ��ȭ�� ����					��ȭ�� ����	
// ��ȭ��ȣ ���					��ȭ��ȣ �Է� && ��ȭ�� �Ǵ�.
// ��ȭ�⸦ ��ȭ ���̺� ����		��ȭ�Ѵ�.
// ��ȭ ���� �︮�� ��ȭ�⸦ ���.	
// ��ȭ�Ѵ�.

// Server						Client
// 1. socket ����				1. socket ����
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

	// window���� socket�� ����ϱ� ���� �ʱ��۾�
	void initWindowSocket()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR)
			cout << "����. error code : " << WSAGetLastError() << endl;
	}

	// UDP socket ���� ����
	SOCKET createSocket()
	{
		SOCKET sock;
		// UDP socket ����
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			cout << "socket() ���� ����. error code : " << WSAGetLastError() << endl;
		return sock;
	}

	void TCPServer()
	{
		cout << "TCP Server" << endl;

		// Window socket �ʱ�ȭ
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

		//������ ���� ������ ����
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

		// Window socket �ʱ�ȭ
		initWindowSocket();

		SOCKET socket = createSocket();

		struct sockaddr_in address;
		memset((char *)&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(address.sin_addr.s_addr));

		if (SOCKET_ERROR == connect(socket, (struct sockaddr*)&address, sizeof(address)))
		{
			cout << "���� ����"<<endl;
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

