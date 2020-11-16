#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManager.h"
#include "PacketFactory.h"
using namespace std;

std::string NetworkManager::state_name[States::kSize];

void NetworkManager::setStateNames()
{
	state_name[kDefault] = "Default";
	state_name[kLobby] = "Lobby";
	state_name[kWaitingRoom] = "WaitingRoom";
	state_name[kStarting] = "Starting";
	state_name[kPlaying] = "Playing";
}

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

void NetworkManager::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
{
	cout << endl;
	cout << "handled packet : [" << PacketFactory::packet_name[packet.getType()]
		<< "]  in state : \"" << state_name[_state] << "\"" << endl;
	if (_handle_packets_map[_state][packet.getType()])
	{
		_handle_packets_map[_state][packet.getType()](from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		cout << "can't handle this packet : " << packet.getType() << std::endl;;
	}
	cout << endl;
}

void NetworkManager::changeState(States state)
{
	cout << "state changed from : \"" << state_name[_state] << "\" to \"" << state_name[state] << "\"." << endl;
	_state = state;

	if (!(kDefault <= _state && _state <kSize))
	{
		cout << "invalid state!" << endl;
	}
}