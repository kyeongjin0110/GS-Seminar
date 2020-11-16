#pragma once
// UDP
// UDP ����� ������ ������ ����� �������� �̷������.
// �׷��Ƿ� ������ ���� UDP�� ���ۿ����� �����ϰ��� �Ѵ�.

// ���� = packet
// �ּ� = socket address
// ��ü�� = socket

// ������ ������ ���ؼ��� ������ ���� ����� �ּҸ� ����� �Ѵ�.
// ������ ������ Ư���� ������ ���� ������ ���ſ��θ� Ȯ���� ����� ����.
// ���� �߰��� ������ �нǵ� ���� �ִ�.
// ������ �ŷ��� �� ���� ���۹���̰�,
// �̿� ���������� UDP���� �׷��ϴ�.


// UDP ����� ������ ���� �������� �̷������.
//
//	   Server								   Client
//
//	1. create socket(��ü��)					1. create socket(��ü��)
//	2. bind(�ּ� ���)				 
//	4. recvfrom(������ ��ٸ�)			<-		3. sendto(�ּҰ� ���� ������ ����)
//	4. sendto(�ּҰ� ���� ������ ����)	->		5. recvfrom(������ ��ٸ�)
//	5. close								6. close
//

// UDP���� ������ Ŭ���̾�Ʈ�� ����Ǿ� ���� �ʴ�.
// �� TCP������ listen, accept�� ���� ������ ���ʿ��ϴ�.
// �׷��Ƿ� UDP������ ������ Ŭ���̾�Ʈ�� �ϳ��� ���ϸ� ������ �ȴ�.
// TCP������ ������ �����ؾ� �ϴ� Ư�� ������,
// �ϳ��� ������ 10���� Ŭ���̾�Ʈ�� ����ϱ� ���ؼ���
// ������ ������ ���� �ܿ��� 10���� ������ �� �ʿ��ϴ�.
// ������ UDP���� socket�� ��ü��� ���� ������ �ϱ� ������,
// �ϳ��� ���� ���� client�鿡�� ������, �ޱ⸦ �� �� �ִ�.

// �� ������ Ŭ���̾�Ʈ ���� ���� ���̰� ���� ����.
// ������ ���̴� bind�ε� ���⼭ �ñ����� ���� �� �ִ� �κ���,
// �� Ŭ���̾�Ʈ�� ��쿡�� bind�� ���� �ּ��Ҵ��� ���� ������ �ʴ����̴�.

// Ŭ���̾�Ʈ�� ���, ������ ������ ��
// sendto �Լ�ȣ�� �������� �ּ������� �Ҵ���� �ʾҴٸ�,
// ù sendto�� �ϴ� ������ ȣ��Ʈ�� ip��ȣ�� ������� �ʴ� port��ȣ�� �Ҵ�ȴ�.

// �׸��� recvfrom �Լ��� ���� �ּ� ������ �Բ� packet�� �޾ұ� ������,
// ���Ŀ� ���� ������ �ٽ� ������ ���� ����, �ش� �ּҷ� �ٽ� �����ָ� �ȴ�.

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
		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			cout << "socket() ���� ����. error code : " << WSAGetLastError() << endl;
		return sock;
	}

	// UDP ����� �������� ���� �������� �����ؼ� �Լ�(sendto)�� ȣ���Ѵ�.
	// UDP�� ��� ������ �����ϰ� ���� �ʿ䰡 ���� ������, �̷��� ����� ����Ѵ�.
	void sendProcess(struct sockaddr_in* address, SOCKET socket, string message)
	{
		// Message ����
		int ret =
			sendto(
				socket,								// ��ü�뿡
				message.c_str(),					// ������
				message.size(),
				0,
				(struct sockaddr *) address,		// <- �ּҷ�
				sizeof(struct sockaddr_in));		// ������.

		// ���� �� �۾�
		if (ret == SOCKET_ERROR)
			cout << "sendto() ���� ����  error code : " << WSAGetLastError() << endl;
	}

	// UDP ����� recvfrom �Լ��� ���� data�� data�� �� �ּҿ� ���� �������� ���´�.
	void recvProcess(struct sockaddr_in* address, SOCKET socket)
	{
		// Message ����
		char recvMessage[BufferSize] = { 0 };
		int len = sizeof(struct sockaddr_in);

		int ret =
			recvfrom(
				socket,								// ��ü�뿡��
				recvMessage,						// ������
				BufferSize,
				0,
				(struct sockaddr *) address,		// <- �ּҷ� ��
				&len);								// ������.

		// ���� �� �۾�
		if (ret == SOCKET_ERROR)
			cout << "recvfrom() ����  error code : " << WSAGetLastError() << endl;

		// ���� ���
		cout << "size : " << ret << "  data : " << recvMessage << endl;
	}

	void UDPClient()
	{
		cout << "UDP Client" << endl;

		// Window socket �ʱ�ȭ
		initWindowSocket();

		// Socket ����
		SOCKET mySocket = createSocket();
		
		// Address ����ü �ʱ�ȭ
		struct sockaddr_in otherAddress;
		memset((char *)&otherAddress, 0, sizeof(otherAddress));
		otherAddress.sin_family = AF_INET;
		otherAddress.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(otherAddress.sin_addr.s_addr));


		// ��� ����
		while (1)
		{
			cout<<"Message �Է� : ";
			string message;
			cin >> message;

			// Server�� �۽�
			sendProcess(&otherAddress, mySocket, message);
			
			// Server�κ��� ����
			recvProcess(&otherAddress, mySocket);
		}

		closesocket(mySocket);
		WSACleanup();
	}

	void UDPServer()
	{
		cout << "UDP Server" << endl;
	
		// Window socket �ʱ�ȭ
		initWindowSocket();

		// Socket ����
		SOCKET mySocket = createSocket();

		// Address ����ü �ʱ�ȭ
		struct sockaddr_in myAddress;
		myAddress.sin_family = AF_INET;
		myAddress.sin_port = htons(Port);
		InetPton(AF_INET, IP.c_str(), &(myAddress.sin_addr.s_addr));

		

		// Socket�� �ּ� �Ҵ�
		if (bind(mySocket, (struct sockaddr*)&myAddress, sizeof(myAddress)) == SOCKET_ERROR)
			std::cout << "�ּ� �Ҵ� ����" << endl;

		while (1)
		{
			int len = sizeof(struct sockaddr_in);
			
			// Client�κ��� messge ����
			recvProcess(&myAddress, mySocket);

			// Client�� message �۽�
			sendProcess(&myAddress, mySocket, "SERVER MSG");
		}

		closesocket(mySocket);
		WSACleanup();
	}
}

