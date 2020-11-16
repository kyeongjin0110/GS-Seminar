#include <iostream>
#include "Network/NetworkManagerLobbyServer.h"
#include "Network/NetworkManagerRoomServer.h"
#include "Network/NetworkManagerClient.h"


int main(int argc, char **argv)
{
	if (argc == 2)
	{
		std::string address = argv[1];
		auto it = std::find(std::begin(address), std::end(address), ':');
		if (it != std::end(address))
		{
			std::cout << "Room Server" << std::endl;
			NetworkManagerRoomServer::staticInit(8000);
			while (1)
				NetworkManagerRoomServer::instance->update();
		}
		else
		{
			std::cout << "Client" << std::endl;
			std::cout << "Your name : " << std::endl;
			std::string name;
			std::cin >> name;
			NetworkManagerClient::staticInit("127.0.0.1:8000", name);
			while (1)
				NetworkManagerClient::instance->update();
		}
	}
	else if (argc == 3)
	{
		std::cout << "Lobby Server" << std::endl;
		NetworkManagerLobbyServer::staticInit(8000);
		while (1)
			NetworkManagerLobbyServer::instance->update();
	}
	else
	{
		std::cout << "argument for server : \"8000\"" << std::endl;
		std::cout << "argument for client : \"127.0.0.1:8000\"" << std::endl;
	}


	return 0;
}
