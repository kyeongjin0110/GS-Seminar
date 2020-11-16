#include <iostream>
#include <sstream>
#include <cassert>
#include <conio.h>
#include "NetworkManagerClient.h"
#include "PacketFactory.h"
using namespace std;
using namespace std::placeholders;

unique_ptr<NetworkManagerClient> NetworkManagerClient::instance = nullptr;

void NetworkManagerClient::staticInit(const string& server_addr, const string& client_name)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerClient(client_name, server_addr));
	instance->init();
}

NetworkManagerClient::NetworkManagerClient(const string& client_name, const string& server_addr)
	:
	_server_address(SocketAddress::createFromString(server_addr)),
	_id(0),
	_client_name(client_name),
	_input_message("")
{
	_address.reset(new SocketAddress());
	
	// Set with handler functions.
	_handle_packets_map[States::kDefault][PacketFactory::kJoined] = bind(&NetworkManagerClient::handleJoinedPacket, this, _1, _2, _3);
	
	_handle_packets_map[States::kLobby][PacketFactory::kIntro] = bind(&NetworkManagerClient::handleIntroPacket, this, _1, _2, _3);
	_handle_packets_map[States::kLobby][PacketFactory::kJoined] = bind(&NetworkManagerClient::handleJoinedPacket, this, _1, _2, _3);
	_handle_packets_map[States::kLobby][PacketFactory::kMessage] = bind(&NetworkManagerClient::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kLobby][PacketFactory::kNotifyDisconnected] = bind(&NetworkManagerClient::handleNotifyDisconnectedPacket, this, _1, _2, _3);
	_handle_packets_map[States::kLobby][PacketFactory::kRoomIsCreated] = bind(&NetworkManagerClient::handleRoomIsCreatedPacket, this, _1, _2, _3);
	_handle_packets_map[States::kLobby][PacketFactory::kRoomInfo] = bind(&NetworkManagerClient::handleRoomInfoPacket, this, _1, _2, _3);

	_handle_packets_map[States::kWaitingRoom][PacketFactory::kIntro] = bind(&NetworkManagerClient::handleIntroPacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kMessage] = bind(&NetworkManagerClient::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kNotifyDisconnected] = bind(&NetworkManagerClient::handleNotifyDisconnectedPacket, this, _1, _2, _3);
	_handle_packets_map[States::kWaitingRoom][PacketFactory::kEnterStarting] = bind(&NetworkManagerClient::handleEnterStartingPacket, this, _1, _2, _3);
	
	_handle_packets_map[States::kStarting][PacketFactory::kEnterPlaying] = bind(&NetworkManagerClient::handleEnterPlayingPacket, this, _1, _2, _3);
	
	_handle_packets_map[States::kPlaying][PacketFactory::kMessage] = bind(&NetworkManagerClient::handleMessagePacket, this, _1, _2, _3);
	_handle_packets_map[States::kPlaying][PacketFactory::kNotifyDisconnected] = bind(&NetworkManagerClient::handleNotifyDisconnectedPacket, this, _1, _2, _3);
}


bool NetworkManagerClient::init()
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));

	// 자신의 주소 등록
	_socket->bind(*_address);

	GamePacket& packet = PacketFactory::createHelloPacket(_id, _client_name);
	send(packet, *_server_address);

	return true;
}


void NetworkManagerClient::update()
{
	recv();
	processInput();
	handleQueuedPackets();
}

void NetworkManagerClient::processInput()
{
	if (_kbhit())
	{
		char ch = _getch();
		cout << ch;

		// Enter 입력 시
		if (ch == 13)
		{
			if (_input_message == "--Start")
			{
				GamePacket& packet = 
					PacketFactory::createPacket(PacketFactory::kRequestStart);
				send(packet, *_server_address);
			}
			else if (_input_message == "--CreateRoom")
			{
				GamePacket& packet =
					PacketFactory::createCreateRoomPacket(_id, _client_name);
				send(packet, *_server_address);
			}
			else if (_input_message == "--JoinRoom")
			{
				int number;
				cout << "room number : ";
				cin >> number;

				GamePacket& packet = PacketFactory::createJoinRoomPacket(number);
				send(packet, *_server_address);
			}
			else if (_input_message == "--ShowRooms")
			{
				GamePacket packet = PacketFactory::createPacket(PacketFactory::kRequestShowRoomInfo);
				send(packet, *_server_address);
			}
			else
			{
				GamePacket& packet = PacketFactory::createMessagePacket(_id, _client_name, _input_message);
				send(packet, *_server_address);
			}

			_input_message = "";
		}
		else
		{
			_input_message += ch;
		}
	}
}


void NetworkManagerClient::handleIntroPacket(
	const SocketAddress& from,
	const uint8_t* buffer, 
	size_t length)
{
	// Verify
	assert(Data::VerifyIntroDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [IntroData]!");

	auto data = Data::GetIntroData(buffer);

	cout
		<< "new client" << endl
		<< "id :  " << data->user()->id() << endl
		<< "name :  " << data->user()->name()->c_str() << endl
		<< "appointedID :  " << data->appointed()->appointedID() << endl
		<< "changed :  " << data->appointed()->changed() << endl
		<< endl;
	
}

void NetworkManagerClient::handleJoinedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyJoinedDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [JoinedData]!");

	auto data = Data::GetJoinedData(buffer);

	// If the client joined to lobby server.
	if (_state == kDefault)
	{
		changeState(kLobby);
		_id = data->joined();
		_socket->setNoneBlockingMode(true);
	}
	// If the client joined to room server.
	else if (_state == kLobby)
	{
		changeState(kWaitingRoom);
	}

	cout
		<< "joined id : " << data->joined() << endl
		<< "appointed id : " << data->appointed()->appointedID() << endl
		<< "changed : " << data->appointed()->changed() << endl
		<< endl;

	cout << "[users list]" << endl;
	for (auto i = data->user()->begin(); i != data->user()->end(); ++i)
	{
		cout << i->id() << " " << i->name()->c_str() << endl;
	}
	cout << endl;
	
}

void NetworkManagerClient::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Data::VerifyMessageDataBuffer(flatbuffers::Verifier(buffer, length)) &&
	"Verify failed [MessageData]!");

	// Process packet logic
	auto data = Data::GetMessageData(buffer);

	cout  << "name : " << data->user()->name()->c_str() << "    message : " << data->msg()->c_str() << endl;
}

void NetworkManagerClient::handleNotifyDisconnectedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	auto data = Data::GetDisconnectedData(buffer);
	
	cout
		<< "appointedID :  " << data->appointed()->appointedID() << endl
		<< "change :  " << data->appointed()->changed() << endl
		<< "disconnectedID :  " << data->disconnectedID() << endl
		<< endl;

}

void NetworkManagerClient::handleEnterStartingPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	changeState(kStarting);
	cout << "loading game world .. " << endl;

	GamePacket& packet = PacketFactory::createPacket(PacketFactory::kReady);
	send(packet, from);
}

void NetworkManagerClient::handleEnterPlayingPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	changeState(kPlaying);
}



void NetworkManagerClient::handleRoomIsCreatedPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	assert(Data::VerifyRoomDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomData]!");

	auto data = Data::GetRoomData(buffer);

	string room_server_addr = data->address()->c_str();
	_server_address.reset(SocketAddress::createFromString(room_server_addr));
	cout << room_server_addr << endl;

	GamePacket& packet = PacketFactory::createHelloPacket(_id, _client_name);
	send(packet, *_server_address);
}


void NetworkManagerClient::handleRoomInfoPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	assert(Data::VerifyRoomsDataBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [RoomsData]!");

	auto data = Data::GetRoomsData(buffer);
	auto frooms = data->rooms();

	cout << "Rooms" << endl;
	for (auto it = frooms->begin(); it != frooms->end(); ++it)
	{
		cout << it->number() << " " << it->address()->c_str() << endl;
	}
	cout << endl;
}