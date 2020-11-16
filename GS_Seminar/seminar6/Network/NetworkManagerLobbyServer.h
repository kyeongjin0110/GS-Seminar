#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include <set>
#include "PacketFactory.h"
#include "NetworkManager.h"
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"
#include "../Network/Process.h"

class NetworkManagerLobbyServer : public NetworkManager
{
public:
	static std::unique_ptr<NetworkManagerLobbyServer> instance;
	static void staticInit(uint16_t port);

	NetworkManagerLobbyServer(uint16_t port);
	virtual ~NetworkManagerLobbyServer() {}

	bool init();
	void update();

	void handleHelloPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleMessagePacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleDisconnectionPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleCreateRoomPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRoomIntroPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleJoinRoomPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRoomHasDestroyedPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRequestShowRoomInfoPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);

private:

	void insertClient(int id, const SocketAddress& address, const std::string& name)
	{
		_clients.emplace(id, address);
		_clients_addr_to_id.emplace(address, id);
		_id_to_name.emplace(id, name);
		
		std::cout << "connected client  address : " << address.toString()
			<< "   id : " << id
			<< "   name : " << name
			<< std::endl;
	}

	void removeClient(const SocketAddress& address)
	{
		auto i = _clients_addr_to_id.find(address);
		int id = i->second;
		auto j = _id_to_name.find(id);
		std::string name = j->second;

		_clients.erase(id);
		_clients_addr_to_id.erase(address);
		_id_to_name.erase(id);

		std::cout << "diconnected client  address : " << address.toString() 
			<< "   id : " << id
			<< "   name : "<< name
			<< std::endl;
	}

	void createRoom(int number, const std::string& filename)
	{
		std::cout 
			<< "number : " << number
			<< "   filename : " << filename
			<< std::endl;

		Process* p = new Process(filename);
		p->createProcess();
		_room_process.insert(std::make_pair(number, p));
	}

	void insertRoom(const SocketAddress& address, int number, const std::string& filename)
	{
		_rooms.emplace(number, address);
		_clients_addr_to_id.emplace(address, number);

		std::cout
			<< "inserted room  address : " << address.toString()
			<< "   number : " << number
			<< "   filename : " << filename
			<< std::endl;
	}

	std::unordered_map<int, SocketAddress> _clients;
	std::unordered_map<SocketAddress, int> _clients_addr_to_id;
	std::unordered_map<int, std::string> _id_to_name;

	std::unordered_map<int, Process*> _room_process;
	std::unordered_map<int, std::pair<int, SocketAddress>> _room_masters;

	std::unordered_map<int, SocketAddress> _rooms;
	std::unordered_map<SocketAddress, int> _rooms_addr_to_num;
};

