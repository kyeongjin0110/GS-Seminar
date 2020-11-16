#include <iostream>
#include <sstream>
#include <cassert>
#include <conio.h>
#include "NetworkManagerClient.h"
#include "PacketFactory.h"

using namespace std;

std::unique_ptr<NetworkManagerClient> NetworkManagerClient::instance = nullptr;

void NetworkManagerClient::staticInit(const string& server_addr, const std::string& client_name)
{
	SocketUtil::staticInit();
	instance.reset(new NetworkManagerClient(client_name));
	instance->init(server_addr);
}

NetworkManagerClient::NetworkManagerClient(const std::string& client_name)
	:
	_id(0),
	_client_name(client_name),
	_input_message("")
{
	_socket.reset(UDPSocket::create(SocketUtil::AddressFamily::INET));
	_address.reset(new SocketAddress());
}


bool NetworkManagerClient::init(const string& server_addr)
{
	_state = kLobby;

	// 자신의 주소 등록
	_socket->bind(*_address);

	// Server의 주소를 생성한다.
	_server_address.reset(SocketAddress::createFromString(server_addr));

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


void NetworkManagerClient::handlePacketByType(
	const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kIntro)
	{
		cout << "packet : [IntroPacket]  from : " << from.toString() << endl;
		handleIntroPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kJoined)
	{
		cout << "packet : [JoinedPacket]  from : " << from.toString() << endl;
		handleJoinedPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kMessage)
	{
		cout << "packet : [MessagePacket]  from : " << from.toString() << endl;
		handleMessagePacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kNotifyDisconnected)
	{
		cout << "packet : [NotifyDisconnectedPacket]  from : " << from.toString() << endl;
		handleNotifyDisconnectedPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kEnterStarting)
	{
		cout << "packet : [EnterStarting]  from : " << from.toString() << endl;
		handleEnterStartingPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kEnterPlaying)
	{
		cout << "packet : [EnterPlayingPacket]  from : " << from.toString() << endl;
		handleEnterPlayingPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		std::cout << "can't handle this packet : " << packet.getType() << std::endl;;
	}
	cout << endl;
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
	
	std::cout
		<< "new client" << std::endl
		<< "id :  " << data->user()->id() << std::endl
		<< "name :  " << data->user()->name()->c_str() << std::endl
		<< "appointedID :  " << data->appointed()->appointedID() << std::endl
		<< "changed :  " << data->appointed()->changed() << std::endl
		<< std::endl;
	
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
	
	// Process packet logic
	_socket->setNoneBlockingMode(true);

	std::cout
		<< "appointed id : " << data->appointed()->appointedID() << std::endl
		<< "changed : " << data->appointed()->changed() << std::endl
		<< std::endl;

	std::cout << "[users list]" << std::endl;
	for (auto i = data->user()->begin(); i != data->user()->end(); ++i)
	{
		std::cout << i->id() << " " << i->name()->c_str() << std::endl;
	}
	std::cout << std::endl;
	
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
	
	std::cout
		<< "appointedID :  " << data->appointed()->appointedID() << std::endl
		<< "change :  " << data->appointed()->changed() << std::endl
		<< "disconnectedID :  " << data->disconnectedID() << std::endl
		<< std::endl;

}

void NetworkManagerClient::handleEnterStartingPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	std::cout << "loading game world .. " << std::endl;

	GamePacket& packet = PacketFactory::createPacket(PacketFactory::kReady);
	send(packet, from);
}

void NetworkManagerClient::handleEnterPlayingPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	_state = kPlaying;
	std::cout << "state changed to \"kPlaying\"" << std::endl;
}


