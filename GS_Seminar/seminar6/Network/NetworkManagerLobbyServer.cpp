#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManagerLobbyServer.h"
#include "PacketFactory.h"
using namespace std;
using namespace std::placeholders;

std::unique_ptr<NetworkManagerLobbyServer> NetworkManagerLobbyServer::instance = nullptr;

void NetworkManagerLobbyServer::staticInit(uint16_t port)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerLobbyServer(port));
	instance->init();
}

NetworkManagerLobbyServer::NetworkManagerLobbyServer(uint16_t port)
{
	_address.reset(new SocketAddress(INADDR_ANY, port));

	// Set with handler functions.
	_handle_packets_map[States::kDefault][PacketFactory::kHello] = bind(&NetworkManagerLobbyServer::handleHelloPacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kMessage] = bind(&NetworkManagerLobbyServer::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kDisconnection] = bind(&NetworkManagerLobbyServer::handleDisconnectionPacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kCreateRoom] = bind(&NetworkManagerLobbyServer::handleCreateRoomPacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kRoomIntro] = bind(&NetworkManagerLobbyServer::handleRoomIntroPacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kJoinRoom] = bind(&NetworkManagerLobbyServer::handleJoinRoomPacket, this, _1, _2, _3);
	_handle_packets_map[States::kDefault][PacketFactory::kRequestShowRoomInfo] = bind(&NetworkManagerLobbyServer::handleRequestShowRoomInfoPacket, this, _1, _2, _3);
}

// Let's put exception occuring functions here.
bool NetworkManagerLobbyServer::init()
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
	return true;
}


void NetworkManagerLobbyServer::update()
{
	recv();
	handleQueuedPackets();
}


void NetworkManagerLobbyServer::handleHelloPacket(
	const SocketAddress& from,
	const uint8_t* buffer, 
	size_t length)
{
	// Verify
	assert(Data::VerifyUserDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [UserData]!");

	// Process packet logic
	int client_id = genUniqueID(_clients);
	auto data = Data::GetUserData(buffer);
	std::string name = data->name()->c_str();

	insertClient(client_id, from, name);
	
	// Response to joined client.
	vector<pair<int, string> > clients;
	for (auto e : _id_to_name)
	{
		int id = e.first;
		string name = e.second;
		clients.emplace_back(id, name);
	}

	GamePacket& join = PacketFactory::createJoinedPacket(clients, client_id, 0, false);
	send(join, from);

	// Response to all other clients.
	GamePacket& intro = PacketFactory::createIntroPacket(client_id, name, 0, false);
	for (auto c : _clients)
		if(c.first != client_id)
			send(intro, c.second);
}

void NetworkManagerLobbyServer::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyMessageDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [MessageData]!");

	// Process packet logic
	auto data = Data::GetMessageData(buffer);
	string name = data->user()->name()->c_str();
	string message = data->msg()->c_str();

	// Response to all
	GamePacket& packet = PacketFactory::createMessagePacket(data->user()->id(), name, message);
	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerLobbyServer::handleDisconnectionPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	auto iter = _clients_addr_to_id.find(from);
	int id = iter->second;

	removeClient(from);

	GamePacket& packet = PacketFactory::createNotifyDisconnectedPacket(id, 0, false);

	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerLobbyServer::handleCreateRoomPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyUserDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [UserData]!");

	// Process packet logic
	auto data = Data::GetUserData(buffer);
	int id = data->id();
	string name = data->name()->c_str();

	int number = genUniqueID(_rooms);
	_room_masters.emplace(number, std::make_pair(id, from));

	stringstream ss;
	ss << number;

	string filename = "GS_Seminar.exe Room " + ss.str() + " " + _address->toString();
	createRoom(number, filename);
}

void NetworkManagerLobbyServer::handleRoomIntroPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyRoomDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomData]!");

	auto data = Data::GetRoomData(buffer);
	int number = data->number();
	std::string address = from.toString();

	insertRoom(from, number, "");

	auto iter = _room_masters.find(number);
	if (iter != std::end(_room_masters))
	{
		// Send that room is created to client who requested to create room.
		GamePacket& packet = PacketFactory::createRoomIsCreatedPacket(number, address);
		send(packet, iter->second.second);
		removeClient(iter->second.second);
	}
}

void NetworkManagerLobbyServer::handleJoinRoomPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyRoomDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomData]!");

	auto data = Data::GetRoomData(buffer);
	int number = data->number();

	auto iter = _rooms.find(number);
	if (iter != std::end(_rooms))
	{
		// Send that room is created to client who requested to create room.
		GamePacket& packet = PacketFactory::createRoomIsCreatedPacket(number, iter->second.toString());
		send(packet, from);
		removeClient(iter->second);
	}
}

void NetworkManagerLobbyServer::handleRoomHasDestroyedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
}

void NetworkManagerLobbyServer::handleRequestShowRoomInfoPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	std::vector<std::pair<int, std::string>> rooms;
	for (auto e : _rooms)
	{
		rooms.push_back(std::make_pair(e.first, e.second.toString()));
	}

	GamePacket& packet = PacketFactory::createRoomInfoPacket(rooms);

	for (auto c : _clients)
		send(packet, c.second);
}