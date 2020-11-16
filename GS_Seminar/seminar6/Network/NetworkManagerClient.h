#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include <functional>
#include "NetworkManager.h"
#include "PacketFactory.h"
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"

class NetworkManagerClient : public NetworkManager
{
public:
	static std::unique_ptr<NetworkManagerClient> instance;
	static void staticInit(const std::string& server_addr, const std::string& client_name);

	NetworkManagerClient(const std::string& client_name, const std::string& server_addr);
	virtual ~NetworkManagerClient()
	{}

	bool init();
	void update();
	void processInput();
	
	void handleIntroPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleJoinedPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleMessagePacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleNotifyDisconnectedPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleEnterStartingPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleEnterPlayingPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRoomIsCreatedPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);
	void handleRoomInfoPacket(const SocketAddress& from, const uint8_t* buffer, size_t length);

private:

	std::unique_ptr<SocketAddress> _server_address;

	int _id;
	std::string _client_name;
	std::string _input_message;

};

