#include <iostream>
#include "UDPSocket.h"
#include "TCPSocket.h"
using namespace std;

namespace udp
{
	// UDP 서버
	void Server()
	{
		cout << "UDP Server start!" << endl;

		// UDP socket 생성
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// 자신의 주소 생성
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// 자신의 주소를 socket에 등록
		udp_socket->bind(*sock_address);

		// 주소를 담을 임시 공간
		SocketAddress from_address;
		
		// recieve
		char buffer[128] = { 0 };
		udp_socket->receiveFrom(buffer, 128, from_address);

		cout <<"recv from : "<<from_address.toString()<<"   data : "<< buffer << endl;;

		delete udp_socket;
		delete sock_address;
	}

	// UDP 클라이언트
	void Client()
	{
		cout << "UDP Client start!" << endl;

		// UDP socket 생성
		UDPSocket* udp_socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

		// 보낼 주소
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
	// TCP 서버
	void Server()
	{
		cout << "TCP Server start!" << endl;

		// TCP socket 생성
		TCPSocket* tcp_socket = TCPSocket::create(SocketUtil::AddressFamily::INET);

		// 자신의 주소 생성
		SocketAddress* sock_address = SocketAddress::createFromString("127.0.0.1:8000");

		// 자신의 주소를 socket에 등록
		tcp_socket->bind(*sock_address);

		// socket을 수동 대기모드로 설정
		// 최대 32개의 socket까지 받아들임
		tcp_socket->listen(32);

		// 새 클라이언트의 접속을 기다림
		SocketAddress from_address;
		TCPSocket* client = tcp_socket->accept(from_address);

		// 새 클라이언트로부터 recieve
		char buffer[128] = { 0 };
		client->receive(buffer, 128);
		
		cout << "recv from : " << from_address.toString() << "   data : " << buffer << endl;;

		delete tcp_socket;
		delete sock_address;
	}

	// TCP 클라이언트
	void Client()
	{
		cout << "TCP Client start!" << endl;

		// TCP socket 생성
		TCPSocket* tcp_client = TCPSocket::create(SocketUtil::AddressFamily::INET);

		// 보낼 주소
		SocketAddress* to_address = SocketAddress::createFromString("127.0.0.1:8000");

		// 연결 시도
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
	// winsock 초기화
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