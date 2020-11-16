#include <iostream>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <conio.h>
#include "Socket/UDPSocket.h"
#include "Socket/TCPSocket.h"
using namespace std;

// TCP blocking example
namespace tcpblocking
{
	// 이전 예제에서 socket을 통해 send/recv 까지 구현해 보았다.
	// 그렇다면 이제 여러 명의 client와 통신하는 server를 구현해 보자.
	// 하지만 막상 아래와 같이 구현을 해 보면 문제가 생기는 것을 알 수 있을 것이다.
	// 기본적으로 accept, send, recv 함수들은 수행하면 현재 main thread를 blocking 상태에 들어가게 하기 때문에,
	// server에서 한 client에게 recv를 하고 있을때 다른 client들의 통신은 무시될 수 밖에 없게 된다. (accept도 마찬가지)
	// 그렇기 때문에 socket을 non-blocking 상태로 바꾸어주어 문제를 보통 해결한다.

	void Server(const string& port)
	{
		cout << "TCP chat server start" << endl;

		// Tcp Socket을 만든다. 
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));
		
		// Port 정보만 이용하여 주소를 만들고 socket에 등록한다.
		// Port만 이용하는 이유는,
		// 자기 자신의 ip를 socket에 등록하는 경우에 ip정보를 따로 받을 필요가 없기 때문이다. 
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);
		
		// Socket을 수동 대기 모드로 변경시킨다.
		// 최대 수용 인원은 32명
		socket->listen(32);

		// Tcp 통신의 경우 연결을 유지해야하기 때문에,
		// 현재 연결을 성공적으로 수행한 socket들을 갖고 있어야 한다.
		vector<unique_ptr<TCPSocket>> clients;

		while (1)
		{
			SocketAddress client_address;
			cout << "blocked in accept" << endl;

			// accept를 통해 새 연결이 성공적으로 받으면,
			// 연결을 요청한 client와 통신하는 socket을 반환하여 vector에 넣는다.
			clients.emplace_back(socket->accept(client_address));

			// 모든 vector를 순회하면서 data를 recv 한다.
			for (auto c = begin(clients); c != end(clients); ++c)
			{
				char buffer[10000] = { 0 };
				(*c)->receive(buffer, 10000);
				cout << buffer << endl;
			}
		}
	}

	void Client(const string& address)
	{
		cout << "TCP chat client start" << endl;

		// TCP socket을 만든다.
		unique_ptr<TCPSocket> socket(TCPSocket::create(SocketUtil::AddressFamily::INET));

		// 연결할 server의 주소
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(address));
		
		// 연결을 요청한다.
		socket->connect(*server_address);


		while (1)
		{
			string message = "here you go, the message from client!";
			socket->send(message.c_str(), message.size());
		}
	}
}

// UDP blocking example
namespace udpblocking
{
	// Udp socket에 경우, tcp와 달리 하나의 socket을 통해 여러 client들의 data를 읽어올 수 있기 때문에,
	// blocking 상태에 빠지더라도 tcp처럼 다른 client들이 통신이 불가능해지는 상황은 생기지 않는다.
	// 하지만 socket이 하나더라도, recv를 통해 blocking 상태에 빠졌을 때는 server의 작업을 수행할 수 없게 된다.
	// 그러므로 결국 udp socket도 blocking mode로는 게임을 구현하기가 힘들다.

	// Socket은 default로 blocking 모드로 설정되어 있기 때문에 udp의 socket도 blocking socket이다.
	// ioctlsocket 함수를 통해 socket을 non-blocking 모드로 바꿀 수 있는데,
	// 이 과정을 UDPsocket class 내부에 setNoneBlockingMode 함수를 통해 구현해 두었다.

	// Socket이 non-blocking 모드에 들어가게 되면 더 이상 send/recv를 해도 blocking 상태에 들어가지 않는다.
	// Data가 준비되어 있으면 가져오고, 준비되어 있지 않다면 바로 넘어간다.
	// 그렇다면 여기서 data가 준비되어 있는지, 준비되어 있지 않은지, 다른 예외가 있는지 확인할 수 있는 방법이 필요할 것이다.

	void Server(const string& port)
	{
		cout << "UDP blocking server start" << endl;

		// Udp socket을 만든다.
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));

		// 자신의 주소를 등록한다.
		SocketAddress address(INADDR_ANY, atoi(port.c_str()));
		socket->bind(address);

		// 현재 socket을 non-blocking mode로 바꾼다.
		socket->setNoneBlockingMode(true);

		// 통신
		while (1)
		{
			SocketAddress client_address;
			char buffer[1024] = { 0 };
			// 여기서 block 상태에 빠지기 때문에~
			socket->receiveFrom(buffer, 1024, client_address);

			cout << "[server received] address : "
				<< client_address.toString()
				<< "   data : "
				<< buffer << endl;

			string message = "hello! client im server!";
			socket->sendTo(message.c_str(), message.size(), client_address);

			// ~게임 logic을 수행하지 못하게 된다.
			cout << "process in game logic.." << endl;
		}
	}


	void Client(const string& server_addr)
	{
		cout << "UDP blocking client start" << endl;

		// Udp socket을 만든다
		unique_ptr<UDPSocket> socket(UDPSocket::create(SocketUtil::AddressFamily::INET));
		
		// 자신의 주소 등록(선택)
		SocketAddress address;
		socket->bind(address);
		//socket->setNoneBlockingMode(true);

		// Server 주소를 만든다.
		unique_ptr<SocketAddress> server_address(SocketAddress::createFromString(server_addr));

		int i = 0;
		while (1)
		{
			if ((i++) % 20 == 0)
			{
				string message = "hello! im client!";
				socket->sendTo(message.c_str(), message.size(), *server_address);

				SocketAddress temp;
				char buffer[1024] = { 0 };
				socket->receiveFrom(buffer, 1024, temp);

				cout << "[client received] address : "
					<< temp.toString()
					<< "   data : "
					<< buffer << endl;
			}
			Sleep(50);
		}
	}
}

// UDP non-blocking chat example
namespace chat
{
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
				cout <<"연결이 끊긴 client : "<< client_address.toString() << endl;

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
			{}
			else if (read_bytes == -WSAECONNRESET)
			{}
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




int main(int argc, char * argv[])
{
	// winsock 초기화
	SocketUtil::staticInit();

	chat::Server("8000");
	//chat::Client("127.0.0.1:8000");


	SocketUtil::cleanUp();
	return 0;
}