#include <iostream>
#include <bitset>
#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>

#include "flatbuffers/GamePacket.hpp"
#include "flatbuffers/simple_data_generated.h"
#include "flatbuffers/data1_generated.h"
#include "flatbuffers/test_generated.h"

#include <conio.h>
#include "Socket/SocketAddress.h"
#include "Socket/SocketUtil.h"
#include "Socket/UDPSocket.h"

// UDP non-blocking chat example
namespace chat
{
	using namespace std;
	// �Ʒ��� ���� ���ڿ��� ���� ��� Ư���� ������ �����ϵ��� �Ѵ�.

	// Server�� client map�� �� client�� id�� �ּ� ������ �߰��Ѵ�.
	const string kLoginPacket = "<Login>";

	// ������ �������� ���, client���� �Ʒ� ���ڿ��� ������.
	const string kOkayPacket = "<Okay>";


	// UDP Chat Server
	void Server(const string& port)
	{
		cout << "Chat Server start!" << endl;

		// Udp socket�� �����.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// �ڽ��� �ּҸ� socket�� ����Ѵ�.
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		socket->setNoneBlockingMode(true);

		// Client���� ������ hash map
		unordered_map<int, SocketAddress> clients;

		// �ּҷκ��� id�� ���� �ϴ� ���
		unordered_map<SocketAddress, int> addressToID;

		// Chat server�� ���� main loop
		while (1)
		{
			// Client�� �ּҸ� ���� �ӽ� ����
			SocketAddress client_address;

			// Client�κ��� ���� �����͸� ���� ����
			char buffer[1024] = { 0 };

			// Non-blocking mode�� socket������ recv�Լ��� return���� ����
			// ���� � ��Ȳ���� �� �� �ִ�.
			int read_bytes = socket->receiveFrom(buffer, 1024, client_address);

			// ���� ���� ������ �񵿱� ��忡 �°� receiveFrom���� block���� �ʰ� �Ѿ��.
			if (read_bytes == 0)
			{
				// nothing to read
			}
			// Client���� socket�� �������� �˸��� ���̴�.
			// �� ���� return ������ ��ȯ�Ǳ� ���ؼ���,
			// server �ʿ��� sendTo �Լ��� ���� �� client���� ���𰡸� ������ �Ѵ�.
			else if (read_bytes == -WSAECONNRESET)
			{
				cout << "������ ���� client : " << client_address.toString() << endl;

				auto iter = addressToID.find(client_address);
				int id = iter->second;
				clients.erase(id);
				addressToID.erase(client_address);
			}
			// ���� data�� ���� ��� return ������ data�� ũ�Ⱑ ��ȯ�ȴ�.
			else if (read_bytes > 0)
			{
				cout << "[server received] address : "
					<< client_address.toString()
					<< "   data : "
					<< buffer << endl;

				// ���� packet�� ���� ó��
				string data = buffer;
				if (data == kLoginPacket)
				{
					// "<Login>" ������ ���ڿ��� ���� ���, �� client�� clients hashmap�� �߰��Ѵ�.
					int client_num = clients.size();
					clients.emplace(client_num + 1, client_address);
					addressToID.emplace(client_address, client_num + 1);

					// ���������� ���ӵǾ����� client���� �˸���.
					socket->sendTo(kOkayPacket.c_str(), kOkayPacket.size(), client_address);
				}
				else
				{
					auto iter = addressToID.find(client_address);
					int id = iter->second;

					stringstream ss;
					ss << id;
					data = ss.str() + " : " + data;

					// ��� client���� ���� packet(data)�� �����Ѵ�.
					for (auto c : clients)
					{
						socket->sendTo(data.c_str(), data.size(), c.second);
					}
				}
			}
			else
			{
				// uhoh, error? exit or just keep going?
			}

			// process other logics ..
		}
	}

	// UDP Chat Client
	void Client(const string& server_addr)
	{
		cout << "Chat Client start!" << endl;

		// Socket�� �����.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// �ڽ��� �ּҸ� ����Ѵ�.(����)
		SocketAddress address;
		socket->bind(address);

		// Server�� �ּҸ� �����Ѵ�.
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

		// Keyboard �Է��� ���� buffer
		char inputs[128] = { 0 };
		char inputCur = 0;

		// Server�� �α��� ��û�� �Ѵ�.
		socket->sendTo(kLoginPacket.c_str(), kLoginPacket.size(), *server_address);

		// Socket�� non-blocking mode�� �ٲ۴�.
		socket->setNoneBlockingMode(true);

		while (1)
		{
			// Enter�� �Էµ� ������ ���ڸ� �Է¹޴´�.
			// cin�� ���� blocking io�̱� ������ ������� ���Ѵ�.
			if (_kbhit())
			{
				char ch = _getch();
				inputs[inputCur++] = ch;
				cout << ch;

				// Enter �Է� ��
				if (ch == 13)
				{
					inputs[inputCur] = 0;
					socket->sendTo(inputs, inputCur, *server_address);
					inputCur = 0;
				}
			}

			// Server�κ��� �����͸� �޴´�.
			SocketAddress temp;
			char buffer[1024] = { 0 };
			int read_bytes = socket->receiveFrom(buffer, 1024, temp);

			if (read_bytes == 0)
			{
			}
			else if (read_bytes == -WSAECONNRESET)
			{
			}
			else if (read_bytes > 0)
			{
				cout << buffer << endl;
			}
			else
			{
			}
		}
	}
}



namespace _5_GamePacket
{
	using namespace std;
	enum Types
	{
		kMonster,
		kPos
	};

	// pos�� ���� packet �����Լ�
	// Monster�� ���ؼ��� ���� �� ���� ���̴�.
	GamePacket createPosPacket(float x, float y, float z)
	{
		// ����ȭ
		flatbuffers::FlatBufferBuilder builder;
		auto pos_data = MyGame::Createpos(builder, x, y, z);
		builder.Finish(pos_data);

		// Type������ Size������ pakcet header�� ����ϱ� ���� GameMessage�� ���Ѵ�.
		GamePacket packet(builder, Types::kPos);
		return packet;

		// GameMessage�� body���� flatbuffers�� ����ȭ�� data�� ����ȴ�.
		// packet.getBody();

		// flatbuffers data�� ����
		// packet.getBodyLength();

		// GameMessage�� data���� flatbuffers data + header data�� ����ȴ�.
		// header data���� ���� ������, Ÿ�� ������ ���Եȴ�.
		// packet.getData();

		// flatbuffers data + header data�� ����
		// packet.getLength();

		// Ÿ���� ���������� Ȯ���ϴ� �Լ�
		// packet.getType();
	}

	void handlePosPacket(const uint8_t* body, size_t body_length)
	{
		// ����
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// ������ȭ
		auto pos = MyGame::Getpos(body);

		// ���
		cout << pos->x() << ", " << pos->y() << ", " << pos->z() << endl;
	}

	void handleMonsterPacket(const uint8_t* body, size_t body_length)
	{
		// ����
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// ������ȭ
		auto monster = MyGame::GetMonster(body);

		// ���
		string name = monster->name()->c_str();
		float x = monster->x();
		float y = monster->y();
		float z = monster->z();
		short mana = monster->mana();
		short hp = monster->hp();
		auto weapons = monster->weapons();

		for (auto w = weapons->begin(); w != weapons->end(); ++w)
		{
			std::cout << w->name()->c_str() << " " << w->damage() << endl;
		}
		std::cout << name << endl;
		std::cout << "pos : " << x << " " << y << " " << z << endl;
		std::cout << "hp/mp : " << hp << " " << mana << endl;
	}
	

	void example()
	{
		std::cout << "_5_GameMessage" << std::endl << std::endl;

		// Server
		float x = 1441.5f;
		float y = -554.7f;
		float z = 994.88f;

		// pos�� ���� packet�� �����.
		GamePacket& send_packet = createPosPacket(x, y, z);

		// ��Ʈ��ũ�� ���� getData()�� data�� send�� �� ���� ���̴�.
		// buffer�� ���� ������ send�� �ȴٰ� ��������.
		uint8_t buffer[4000] = { 0 };
		size_t length = 0;

		memcpy(buffer, send_packet.getData(), send_packet.getLength());
		length = send_packet.getLength();


		// send(send_packet.getData(), send_packet.getLength())
		// ..
		// ��Ʈ��ũ�� ���� �ٸ� ��ǻ�Ϳ��� �����͸� recv�Ѵٰ� �����Ѵ�.
		// �����ʹ� recv�� ���� buffer�� ����ȴٰ� �����Ѵ�.
		// ..
		// recv(buffer)

		
		// ���� buffer���� � �����Ͱ� �ִ��� �� �� ������,
		// buffer�� length�� GameMessage�� ������Ų�ٸ�,
		GamePacket recv_packet(buffer, length);

		// Type�� �о�� �� �ִ�.
		if (recv_packet.getType() == Types::kPos)
		{
			// Type�� �°� body�� �ִ� flatbuffer�� ����ȭ �� data�� ó�����ش�.
			handlePosPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
		else if (recv_packet.getType() == Types::kMonster)
		{
			handleMonsterPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
	}
}

