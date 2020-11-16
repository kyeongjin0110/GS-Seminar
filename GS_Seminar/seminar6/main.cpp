#include <iostream>
#include "Network/NetworkManagerLobbyServer.h"
#include "Network/NetworkManagerRoomServer.h"
#include "Network/NetworkManagerClient.h"


int main(int argc, char **argv)
{
	if (argc == 3)
	{
		// Lobby, Room, Client
		std::string what = argv[1];

		if (what == "Lobby")
		{
			std::cout << "Lobby Server" << std::endl;

			std::string port = argv[2];
			NetworkManagerLobbyServer::staticInit(atoi(port.c_str()));
			while (1)
				NetworkManagerLobbyServer::instance->update();
		}
		else if (what == "Client")
		{
			std::cout << "Client" << std::endl;

			std::string name;
			std::cout << "User name : ";
			std::cin >> name;

			std::string address = argv[2];
			NetworkManagerClient::staticInit(address, name);
			while (1)
				NetworkManagerClient::instance->update();
		}
	}
	else if (argc == 4)
	{
		std::string what = argv[1];
		if (what == "Room")
		{
			std::cout << "Room Server" << std::endl;
			std::cout
				<< argv[0] << " "
				<< argv[1] << " "
				<< argv[2] << " "
				<< argv[3] << std::endl;

			std::string number = argv[2];
			std::string address = argv[3];

			auto pos = address.find_last_of(':');
			std::string host, port;
			if (pos != std::string::npos)
			{
				host = address.substr(0, pos);
				port = address.substr(pos + 1);
				if (host == "0.0.0.0")
					host = "127.0.0.1";
			}
			else
			{
				std::cout <<"address error : "<< address << std::endl;
			}

			NetworkManagerRoomServer::staticInit(atoi(number.c_str()), host + ":" + port);
			while (1)
				NetworkManagerRoomServer::instance->update();
		}
	}
	std::cout << "argument for lobby server : test.exe Lobby 8000" << std::endl;
	std::cout << "argument for room server : test.exe Room 13 127.0.0.1:8000" << std::endl;
	std::cout << "argument for client : test.exe Client 127.0.0.1:8000" << std::endl;

	return 0;
}
