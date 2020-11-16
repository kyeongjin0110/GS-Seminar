#include <iostream>
#include "Network/NetworkManagerServer.h"
#include "Network/NetworkManagerClient.h"

int main()
{
	SocketUtil::staticInit();
	
	/*std::cout << "Server" << std::endl;
	NetworkManagerServer::staticInit(8000);
	while (1)
		NetworkManagerServer::instance->update();*/
		

	std::cout << "Client" << std::endl;
	std::cout<<"Your name : "<<std::endl;
	std::string name;
	std::cin >> name;
	NetworkManagerClient::staticInit("127.0.0.1:8000", name);
	while (1)
		NetworkManagerClient::instance->update();
		


	SocketUtil::cleanUp();
	return 0;
}

