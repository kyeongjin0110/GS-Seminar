#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include "PacketFactory.h"
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"

class NetworkManagerClient
{
public:
	static std::unique_ptr<NetworkManagerClient> instance;
	static void staticInit(const std::string& server_addr, const std::string& client_name);

	NetworkManagerClient(const std::string& client_name);
	~NetworkManagerClient()
	{}

	bool init(const std::string& server_addr);
	void update();

	void recv();
	void send(GamePacket& packet, const SocketAddress& address);

	void handlePacketByType(const GamePacket& packet, const SocketAddress& from);

	void handleOkayPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleMessagePacket(const SocketAddress& from, const uint8_t* buffer, size_t length);

private:

	std::unique_ptr<UDPSocket> _socket;
	std::unique_ptr<SocketAddress> _address;
	std::unique_ptr<SocketAddress> _server_address;

	int _id;
	std::string _client_name;
	std::string _input_message;
};

