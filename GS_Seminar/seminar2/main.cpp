#include <iostream>
#include "UDPSocket.h"
#include "TCPSocket.h"
using namespace std;

namespace udp
{
	// UDP ����
	void Server()
	{
		cout << "UDP Server start!" << endl;

		// UDP socket ����
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// �ڽ��� �ּ� ����
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// �ڽ��� �ּҸ� socket�� ���
		udp_socket->bind(*sock_address);

		// �ּҸ� ���� �ӽ� ����
		SocketAddress from_address;
		
		// recieve
		char buffer[128] = { 0 };
		udp_socket->receiveFrom(buffer, 128, from_address);

		cout <<"recv from : "<<from_address.toString()<<"   data : "<< buffer << endl;;

		delete udp_socket;
		delete sock_address;
	}

	// UDP Ŭ���̾�Ʈ
	void Client()
	{
		cout << "UDP Client start!" << endl;

		// UDP socket ����
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// ���� �ּ�
		SocketAddress* to_address = SocketAddress::createFromString("127.0.0.1:8000");
		
		// send
		std::string message = "Hello UDP world!";
		udp_socket->sendTo(message.c_str(), message.size(), *to_address);

		delete udp_socket;
		delete to_address;
	}
}

namespace tcp
{
	// TCP ����
	void Server()
	{
		cout << "TCP Server start!" << endl;

		// TCP socket ����
		TCPSocket* tcp_socket = TCPSocket::create(SocketUtil::AddressFamily::INET);

		// �ڽ��� �ּ� ����
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// �ڽ��� �ּҸ� socket�� ���
		tcp_socket->bind(*sock_address);

		// socket�� ���� ������ ����
		// �ִ� 32���� socket���� �޾Ƶ���
		tcp_socket->listen(32);

		// �� Ŭ���̾�Ʈ�� ������ ��ٸ�
		SocketAddress from_address;
		TCPSocket* client = tcp_socket->accept(from_address);

		// �� Ŭ���̾�Ʈ�κ��� recieve
		char buffer[128] = { 0 };
		client->receive(buffer, 128);
		
		cout << "recv from : " << from_address.toString() << "   data : " << buffer << endl;;

		delete tcp_socket;
		delete sock_address;
	}

	// TCP Ŭ���̾�Ʈ
	void Client()
	{
		cout << "TCP Client start!" << endl;

		// TCP socket ����
		TCPSocket* tcp_client = TCPSocket::create(SocketUtil::AddressFamily::INET);

		// ���� �ּ�
		SocketAddress* to_address = SocketAddress::createFromString("127.0.0.1:8000");

		// ���� �õ�
		tcp_client->connect(*to_address);

		// send
		std::string message = "Hello TCP world!";
		tcp_client->send(message.c_str(), message.size());


		delete tcp_client;
		delete to_address;
	}
}


int main(int argc, char * argv[])
{
	// winsock �ʱ�ȭ
	SocketUtil::staticInit();

	if (argc == 2)
	{
		string str = argv[1];
		if (str == "UDPClient")
		{
			udp::Client();
			return 0;
		}
		else if (str == "UDPServer")
		{
			udp::Server();
			return 0;
		}
		else if (str == "TCPClient")
		{
			tcp::Client();
			return 0;
		}
		else if (str == "TCPServer")
		{
			tcp::Server();
			return 0;
		}
	}

	std::cout << "input argument = [\"UDPClient\" , \"UDPServer\", \"TCPClient\", \"TCPServer\"]";

	SocketUtil::cleanUp();

	return 0;
}