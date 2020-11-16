#include "Socket\UDPSocket.h"
#include <iostream>
using namespace std;

void server()
{
	cout << "server start!" << endl;
	UDPSocket* socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

	// 磊扁 林家
	SocketAddress* address = SocketAddress::createFromString("10.10.0.28:8000");
	socket->bind(*address);

	SocketAddress client_address;
	char buffer[1024] = {0};

	// block 
	socket->receiveFrom(buffer, 1024, client_address);

	std::cout << "recv from : " << client_address.toString() << "  data : " << buffer << endl;

	Sleep(10000);
}

void client()
{
	cout << "client start!" << endl;
	UDPSocket* socket = UDPSocket::create(SocketUtil::AddressFamily::INET);

	string str = "hello im client!";


	// 辑滚 林家
	SocketAddress* server_addr = SocketAddress::createFromString("127.0.0.1:8000");

	socket->sendTo(str.c_str(), str.size(), *server_addr);
}


int main()
{
	SocketUtil::staticInit();

	server();
	//client();

	SocketUtil::cleanUp();
	return 0;
}
