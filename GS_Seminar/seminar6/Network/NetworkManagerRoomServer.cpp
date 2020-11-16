#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManagerRoomServer.h"
#include "PacketFactory.h"
using namespace std;
using namespace std::placeholders;

std::unique_ptr<NetworkManagerRoomServer> NetworkManagerRoomServer::instance = nullptr;

void NetworkManagerRoomServer::staticInit(int number, const std::string& server_addr)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerRoomServer(number, server_addr));
	instance->init();
}

NetworkManagerRoomServer::NetworkManagerRoomServer(int number, const std::string& server_addr)
	:
	_lobby_server_address(SocketAddress::createFromString(server_addr)),
	_appointed_id(0),
	_time(0),
	_room_number(number)
{
	_state = kWaitingRoom;
	
	// Start with any port available.
	_address.reset(new SocketAddress());

	_handle_packets_map[States::kWaitingRoom][PacketFactory::kHello] = bind(&NetworkManagerRoomServer::handleHelloPacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kMessage] = bind(&NetworkManagerRoomServer::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kDisconnection] = bind(&NetworkManagerRoomServer::handleDisconnectionPacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kRequestStart] = bind(&NetworkManagerRoomServer::handleRequestStartPacket, this, _1, _2, _3);
	
	_handle_packets_map[States::kStarting][PacketFactory::kReady] = bind(&NetworkManagerRoomServer::handleReadyPacket, this, _1, _2, _3);

	_handle_packets_map[States::kPlaying][PacketFactory::kMessage] = bind(&NetworkManagerRoomServer::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kPlaying][PacketFactory::kDisconnection] = bind(&NetworkManagerRoomServer::handleDisconnectionPacket, this, _1, _2, _3);
}

// Let's put exception occuring functions here.
bool NetworkManagerRoomServer::init()
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);

	// Notify to lobby server that room created successfully.
	GamePacket& packet = PacketFactory::createRoomIntroPacket(_room_number, _address->toString());
	send(packet, *_lobby_server_address);

	return true;
}


void NetworkManagerRoomServer::update()
{
	recv();

	if (_state == kWaitingRoom)
	{
	}
	else if (_state == kStarting)
	{
		_time++;
		if (_time > 100)
		{
			// for all send EnterPlayingState
		}
	}
	else if (_state == kPlaying)
	{
	}

	handleQueuedPackets();
}



void NetworkManagerRoomServer::handleHelloPacket(
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
	
	// Appoint new room master
	auto find = _clients.find(_appointed_id);
	bool change = false;

	if (find != end(_clients))
	{
		change = false;
	}
	else
	{
		change = true;
		_appointed_id = client_id;
		std::cout << "appointed : " << _appointed_id << std::endl;
	}

	// Make other users data
	std::vector<std::pair<int, std::string>> users;
	for (const auto& c : _clients)
	{
		int id = c.first;
		users.push_back(std::make_pair(id, _id_to_name[id]));
	}
	// Response to joined client with other users data
	GamePacket& joined = PacketFactory::createJoinedPacket(users, client_id, _appointed_id, change);
	send(joined, from);

	// Response to all other clients
	GamePacket& intro = PacketFactory::createIntroPacket(client_id, name, _appointed_id, change);
	for (auto c : _clients)
		if(c.first != client_id)
			send(intro, c.second);
}

void NetworkManagerRoomServer::handleMessagePacket(
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

void NetworkManagerRoomServer::handleDisconnectionPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	auto iter = _address_to_id.find(from);
	int id = iter->second;
	bool change = false;

	removeClient(from);

	// 방장이 나간경우
	if (id == _appointed_id)
	{
		// 남은 인원이 있는 경우 방장 교체
		if (_clients.size() > 0)
		{
			_appointed_id = begin(_clients)->first;
			change = true;
		}
		// 아무도 없는 경우
		else
		{
			std::cout << "close this room" << std::endl;
		}
	}

	GamePacket& packet = PacketFactory::createNotifyDisconnectedPacket(id, _appointed_id, change);

	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerRoomServer::handleRequestStartPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	changeState(kStarting);

	GamePacket& packet = PacketFactory::createPacket(PacketFactory::kEnterStarting);
	for (auto c : _clients)
		send(packet, c.second);
}

void NetworkManagerRoomServer::handleReadyPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	int id = _address_to_id[from];
	_ready_set.insert(id);

	std::cout << id << " is ready!" << std::endl;

	// if all clients are ready.
	if (_ready_set.size() == _clients.size())
	{
		changeState(kPlaying);

		GamePacket& packet = PacketFactory::createPacket(PacketFactory::kEnterPlaying);
		for (auto c : _clients)
			send(packet, c.second);
	}
}