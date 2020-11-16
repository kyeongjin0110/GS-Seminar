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
	instance.reset(new NetworkManagerClient(client_name));
	instance->init(server_addr);
}

NetworkManagerClient::NetworkManagerClient(const std::string& client_name)
	:
	_socket(UDPSocket::create(SocketUtil::AddressFamily::INET)),
	_address(new SocketAddress()),
	_id(0),
	_client_name(client_name),
	_input_message("")
{}


bool NetworkManagerClient::init(const string& server_addr)
{
	// 자신의 주소 등록
	_socket->bind(*_address);

	// Server의 주소를 생성한다.
	_server_address.reset(SocketAddress::createFromString(server_addr));

	// Server에 로그인 요청을 한다.
	GamePacket& packet = PacketFactory::createLoginPacket(_client_name);
	send(packet, *_server_address);

	return true;
}


void NetworkManagerClient::update()
{
	if (_kbhit())
	{
		char ch = _getch();
		cout << ch;

		// Enter 입력 시
		if (ch == 13)
		{
			GamePacket& packet = PacketFactory::createMessagePacket(_client_name, _input_message);
			send(packet, *_server_address);

			_input_message = "";
		}
		else
		{
			_input_message += ch;
		}
	}

	recv();
}


void NetworkManagerClient::recv()
{
	SocketAddress address;	
	GamePacket packet;

	int read_bytes = _socket->receiveFrom(packet.getData(), GamePacket::MAX_DATA_LENGTH, address);
	
	if (read_bytes == 0)
	{}

	else if (read_bytes == -WSAECONNRESET)
	{}

	else if (read_bytes > 0)
	{
		packet.decodeHeader();
		handlePacketByType(packet, address);
	}
	else
	{}
}


void NetworkManagerClient::send(GamePacket& packet, const SocketAddress& address)
{
	_socket->sendTo(packet.getData(), packet.getLength(), address);
}


void NetworkManagerClient::handlePacketByType(const GamePacket& packet, const SocketAddress& from)
{
	if (packet.getType() == PacketFactory::kOkay)
	{
		cout << "packet : [OkayPacket]  from : " << from.toString() << endl;
		handleOkayPacket(from, packet.getBody(), packet.getBodyLength());
	}
	else if (packet.getType() == PacketFactory::kMessage)
	{
		cout << "packet : [MessagePacket]  from : " << from.toString() << endl;
		handleMessagePacket(from, packet.getBody(), packet.getBodyLength());
	}
	else
	{
		std::cout << "can't handle this packet : " << packet.getType() << std::endl;;
	}
	cout << endl;
}

void NetworkManagerClient::handleOkayPacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Packets::VerifyOkayPacketBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [OkayPacket]!");

	// Process packet logic
	_socket->setNoneBlockingMode(true);

	auto data = Packets::GetOkayPacket(buffer);
	_id = data->id();

	cout << "my id : " << _id << endl;
}

void NetworkManagerClient::handleMessagePacket(
	const SocketAddress& from,
	const uint8_t* buffer,
	size_t length)
{
	// Verify
	assert(Packets::VerifyMessagePacketBuffer(flatbuffers::Verifier(buffer, length)) &&
		"Verify failed [MessagePacket]!");

	// Process packet logic
	auto data = Packets::GetMessagePacket(buffer);
	
	cout  << "name : " << data->name()->c_str() << "    message : " << data->message()->c_str() << endl;
}

