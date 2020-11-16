#pragma once

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unordered_map>
#include <queue>
#include "PacketFactory.h"
#include "../Socket/UDPSocket.h"
#include "../Socket/SocketUtil.h"
#include "../Socket/SocketAddress.h"

class NetworkManager
{
public:

	enum States
	{
		kLobby, kWaitingRoom, kStarting, kPlaying
	};

	class ReceivedPacket
	{
	public:
		ReceivedPacket(std::unique_ptr<GamePacket> packet, std::unique_ptr<SocketAddress> address)
			:
			_packet(std::move(packet)),
			_from_address(std::move(address))
		{}

		const GamePacket& getPacket() const { return *_packet; }
		const SocketAddress& getFromAddress() const { return *_from_address; }

	private:
		std::unique_ptr<GamePacket> _packet;
		std::unique_ptr<SocketAddress> _from_address;
	};


	NetworkManager() : _socket(nullptr), _address(nullptr), _state(kLobby) {}
	virtual ~NetworkManager()
	{
		SocketUtil::cleanUp(); 
	}

	void recv();
	void send(GamePacket& packet, const SocketAddress& address);

	void handleQueuedPackets();
	virtual void handlePacketByType(const GamePacket& packet, const SocketAddress& from) = 0;

protected:
	std::unique_ptr<UDPSocket> _socket;
	std::unique_ptr<SocketAddress> _address;
	std::queue< ReceivedPacket, std::list< ReceivedPacket > > _packetQ;

	States _state;
};

