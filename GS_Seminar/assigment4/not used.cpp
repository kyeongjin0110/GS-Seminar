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
	// 아래와 같은 문자열이 들어올 경우 특수한 동작을 수행하도록 한다.

	// Server의 client map에 새 client의 id와 주소 정보를 추가한다.
	const string kLoginPacket = "<Login>";

	// 접속이 성공적일 경우, client에게 아래 문자열을 보낸다.
	const string kOkayPacket = "<Okay>";


	// UDP Chat Server
	void Server(const string& port)
	{
		cout << "Chat Server start!" << endl;

		// Udp socket을 만든다.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// 자신의 주소를 socket에 등록한다.
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		socket->setNoneBlockingMode(true);

		// Client들을 저장할 hash map
		unordered_map<int, SocketAddress> clients;

		// 주소로부터 id를 얻어야 하는 경우
		unordered_map<SocketAddress, int> addressToID;

		// Chat server를 위한 main loop
		while (1)
		{
			// Client의 주소를 담을 임시 공간
			SocketAddress client_address;

			// Client로부터 받은 데이터를 담을 버퍼
			char buffer[1024] = { 0 };

			// Non-blocking mode의 socket에서는 recv함수의 return값을 보고
			// 현재 어떤 상황인지 알 수 있다.
			int read_bytes = socket->receiveFrom(buffer, 1024, client_address);

			// 읽을 것이 없었고 비동기 모드에 맞게 receiveFrom에서 block되지 않고 넘어간다.
			if (read_bytes == 0)
			{
				// nothing to read
			}
			// Client에서 socket이 닫혔음을 알리는 값이다.
			// 이 값이 return 값으로 반환되기 위해서는,
			// server 쪽에서 sendTo 함수를 통해 이 client에게 무언가를 보내야 한다.
			else if (read_bytes == -WSAECONNRESET)
			{
				cout << "연결이 끊긴 client : " << client_address.toString() << endl;

				auto iter = addressToID.find(client_address);
				int id = iter->second;
				clients.erase(id);
				addressToID.erase(client_address);
			}
			// 읽을 data가 있을 경우 return 값으로 data의 크기가 반환된다.
			else if (read_bytes > 0)
			{
				cout << "[server received] address : "
					<< client_address.toString()
					<< "   data : "
					<< buffer << endl;

				// 받은 packet에 대한 처리
				string data = buffer;
				if (data == kLoginPacket)
				{
					// "<Login>" 형태의 문자열을 받을 경우, 이 client를 clients hashmap에 추가한다.
					int client_num = clients.size();
					clients.emplace(client_num + 1, client_address);
					addressToID.emplace(client_address, client_num + 1);

					// 성공적으로 접속되었음을 client에게 알린다.
					socket->sendTo(kOkayPacket.c_str(), kOkayPacket.size(), client_address);
				}
				else
				{
					auto iter = addressToID.find(client_address);
					int id = iter->second;

					stringstream ss;
					ss << id;
					data = ss.str() + " : " + data;

					// 모든 client에게 받은 packet(data)를 전달한다.
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

		// Socket을 만든다.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// 자신의 주소를 등록한다.(선택)
		SocketAddress address;
		socket->bind(address);

		// Server의 주소를 생성한다.
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

		// Keyboard 입력을 담을 buffer
		char inputs[128] = { 0 };
		char inputCur = 0;

		// Server에 로그인 요청을 한다.
		socket->sendTo(kLoginPacket.c_str(), kLoginPacket.size(), *server_address);

		// Socket을 non-blocking mode로 바꾼다.
		socket->setNoneBlockingMode(true);

		while (1)
		{
			// Enter가 입력될 때까지 문자를 입력받는다.
			// cin의 경우는 blocking io이기 때문에 사용하지 못한다.
			if (_kbhit())
			{
				char ch = _getch();
				inputs[inputCur++] = ch;
				cout << ch;

				// Enter 입력 시
				if (ch == 13)
				{
					inputs[inputCur] = 0;
					socket->sendTo(inputs, inputCur, *server_address);
					inputCur = 0;
				}
			}

			// Server로부터 데이터를 받는다.
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

	// pos에 대한 packet 생성함수
	// Monster에 대해서도 만들 수 있을 것이다.
	GamePacket createPosPacket(float x, float y, float z)
	{
		// 직렬화
		flatbuffers::FlatBufferBuilder builder;
		auto pos_data = MyGame::Createpos(builder, x, y, z);
		builder.Finish(pos_data);

		// Type정보와 Size정보를 pakcet header에 기록하기 위해 GameMessage로 감싼다.
		GamePacket packet(builder, Types::kPos);
		return packet;

		// GameMessage의 body에는 flatbuffers로 직렬화된 data가 저장된다.
		// packet.getBody();

		// flatbuffers data의 길이
		// packet.getBodyLength();

		// GameMessage의 data에는 flatbuffers data + header data가 저장된다.
		// header data에는 길이 정보와, 타입 정보가 포함된다.
		// packet.getData();

		// flatbuffers data + header data의 길이
		// packet.getLength();

		// 타입이 무엇인지만 확인하는 함수
		// packet.getType();
	}

	void handlePosPacket(const uint8_t* body, size_t body_length)
	{
		// 검증
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// 역직렬화
		auto pos = MyGame::Getpos(body);

		// 출력
		cout << pos->x() << ", " << pos->y() << ", " << pos->z() << endl;
	}

	void handleMonsterPacket(const uint8_t* body, size_t body_length)
	{
		// 검증
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(body, body_length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// 역직렬화
		auto monster = MyGame::GetMonster(body);

		// 출력
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

		// pos에 대한 packet을 만든다.
		GamePacket& send_packet = createPosPacket(x, y, z);

		// 네트워크를 통해 getData()의 data를 send할 수 있을 것이다.
		// buffer를 통해 실제로 send가 된다고 가정하자.
		uint8_t buffer[4000] = { 0 };
		size_t length = 0;

		memcpy(buffer, send_packet.getData(), send_packet.getLength());
		length = send_packet.getLength();


		// send(send_packet.getData(), send_packet.getLength())
		// ..
		// 네트워크를 통해 다른 컴퓨터에서 데이터를 recv한다고 가정한다.
		// 데이터는 recv를 통해 buffer에 저장된다고 가정한다.
		// ..
		// recv(buffer)

		
		// 현재 buffer에는 어떤 데이터가 있는지 알 수 없지만,
		// buffer와 length로 GameMessage를 생성시킨다면,
		GamePacket recv_packet(buffer, length);

		// Type을 읽어올 수 있다.
		if (recv_packet.getType() == Types::kPos)
		{
			// Type에 맞게 body에 있는 flatbuffer로 직렬화 된 data를 처리해준다.
			handlePosPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
		else if (recv_packet.getType() == Types::kMonster)
		{
			handleMonsterPacket(recv_packet.getBody(), recv_packet.getBodyLength());
		}
	}
}

