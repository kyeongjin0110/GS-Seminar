#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManager.h"
#include "PacketFactory.h"
using namespace std;

void NetworkManager::recv()
{
	unique_ptr<GamePacket> packet(new GamePacket());
	unique_ptr<SocketAddress> address(new SocketAddress());

	int read_bytes = _socket->receiveFrom(packet->getData(), GamePacket::MAX_DATA_LENGTH, *address);

	// Nothing to read
	if (read_bytes == 0)
	{}
	// Disconnected
	else if (read_bytes == -WSAECONNRESET)
	{
		packet.reset(new GamePacket(PacketFactory::kDisconnection));
		_packetQ.push(ReceivedPacket(move(packet), move(address)));
	}
	// Something to read
	else if (read_bytes > 0)
	{
		packet->decodeHeader();
		_packetQ.push(ReceivedPacket(move(packet), move(address)));
	}
	else
	{
		// uhoh, error? exit or just keep going?
	}
}

void NetworkManager::send(GamePacket& packet, const SocketAddress& address)
{
	_socket->sendTo(packet.getData(), packet.getLength(), address);
}

void NetworkManager::handleQueuedPackets()
{
	while (!_packetQ.empty())
	{
		const ReceivedPacket& recv_packet = _packetQ.front();
		handlePacketByType(recv_packet.getPacket(), recv_packet.getFromAddress());
		_packetQ.pop();
	}
}