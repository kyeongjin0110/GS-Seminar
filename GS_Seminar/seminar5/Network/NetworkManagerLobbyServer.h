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

class NetworkManagerLobbyServer : public NetworkManager
{
public:
	static std::unique_ptr<NetworkManagerLobbyServer> instance;
	static void staticInit(uint16_t port);

	NetworkManagerLobbyServer(uint16_t port);
	virtual ~NetworkManagerLobbyServer() {}

	bool init();
	void update();


	virtual void handlePacketByType(const GamePacket& packet, const SocketAddress& from) override;

	void handleMessagePacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleHelloPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleDisconnectionPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRequestStartPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleReadyPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);

private:

	void insertClient(int id, const SocketAddress& address, const std::string& name)
	{
		_clients.emplace(id, address);
		_address_to_id.emplace(address, id);
		_id_to_name.emplace(id, name);
		
		std::cout << "connected client  address : " << address.toString()
			<< "   id : " << id
			<< "   name : " << name
			<< std::endl;
	}

	void removeClient(const SocketAddress& address)
	{
		auto i = _address_to_id.find(address);
		int id = i->second;
		auto j = _id_to_name.find(id);
		std::string name = j->second;

		_clients.erase(id);
		_address_to_id.erase(address);
		_id_to_name.erase(id);

		std::cout << "diconnected client  address : " << address.toString() 
			<< "   id : " << id
			<< "   name : "<< name
			<< std::endl;
	}

	std::unordered_map<int, SocketAddress> _clients;
	std::unordered_map<SocketAddress, int> _address_to_id;
	std::unordered_map<int, std::string> _id_to_name;

	int _appointed_id;
	std::set<int> _ready_set;
	int _time;
};

