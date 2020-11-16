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
		kDefault, kLobby, kWaitingRoom, kStarting, kPlaying, kSize
	};
	static std::string state_name[States::kSize];
	static void setStateNames();

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

	template <typename Map>
	static int genUniqueID(const Map& map)
	{
		int max_id = 0;
		for (auto e : map)
		{
			max_id = max(max_id, e.first);
		}
		return max_id + 1;
	}

	NetworkManager() : _socket(nullptr), _address(nullptr), _state(kDefault) 
	{
		PacketFactory::setPacketNames();
		setStateNames();
	}

	virtual ~NetworkManager()
	{
		SocketUtil::cleanUp(); 
	}

	void changeState(States state);

	void recv();
	void send(GamePacket& packet, const SocketAddress& address);

	void handleQueuedPackets();
	virtual void handlePacketByType(const GamePacket& packet, const SocketAddress& from);

protected:
	std::unique_ptr<UDPSocket> _socket;
	std::unique_ptr<SocketAddress> _address;
	std::queue< ReceivedPacket, std::list< ReceivedPacket > > _packetQ;
	States _state;
	std::function<void(const SocketAddress&, const uint8_t*, size_t)>
		_handle_packets_map[States::kSize][PacketFactory::PacketType::kSize];
};

