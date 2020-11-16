#include <iostream>
#include <sstream>
#include <cassert>
#include "NetworkManagerRoomServer.h"
#include "PacketFactory.h"
using namespace std;

std::unique_ptr<NetworkManagerRoomServer> NetworkManagerRoomServer::instance = nullptr;

void NetworkManagerRoomServer::staticInit(uint16_t port)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerRoomServer(port));
	instance->init();
}

NetworkManagerRoomServer::NetworkManagerRoomServer(uint16_t port)
	:
	_appointed_id(0),
	_time(0)
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress(INADDR_ANY, port));
}

bool NetworkManagerRoomServer::init()
{
	_state = kWaitingRoom;
	_socket->bind(*_address);
	_socket->setNoneBlockingMode(true);
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


void NetworkManagerRoomServer::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kHello)
	{
		cout << "packet : [HelloPacket]  from : " << from.toString() << endl;
		handleHelloPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kMessage)
	{
		cout << "packet : [MessagePacket]  from : " << from.toString() << endl;
		handleMessagePacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kDisconnection)
	{
		cout << "packet : [Disconnection]  from : " << from.toString() << endl;
		handleDisconnectionPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kRequestStart)
	{
		cout << "packet : [RequestStart]  from : " << from.toString() << endl;
		handleRequestStartPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kReady)
	{
		cout << "packet : [Ready]  from : " << from.toString() << endl;
		handleReadyPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		cout << "can't handle this packet : " << packet.getType() << std::endl;
	}
	cout << endl;
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
	int client_id = _clients.size() + 1;
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
	GamePacket& joined = PacketFactory::createJoinedPacket(users, _appointed_id, change);
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

	// ������ �������
	if (id == _appointed_id)
	{
		// ���� �ο��� �ִ� ��� ���� ��ü
		if (_clients.size() > 0)
		{
			_appointed_id = begin(_clients)->first;
			change = true;
		}
		// �ƹ��� ���� ���
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
	_state = kStarting;
	std::cout << "state changed to \"kStarting\"" << std::endl;

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
		_state = kPlaying;
		std::cout << "state changed to \"kPlaying\"" << std::endl;

		GamePacket& packet = PacketFactory::createPacket(PacketFactory::kEnterPlaying);
		for (auto c : _clients)
			send(packet, c.second);
	}
}