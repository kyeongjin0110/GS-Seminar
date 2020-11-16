#include <iostream>
using namespace std;

// 기본적으로 socket address를 만들기 위해서 고려해야 했던 사항들이 있었다.
// 1. Ipv4, Ipv6 등등 다양한 address 체계들을 동시에 지원
// 2. 위에서 언급한 다양한 address들을 구분하기 위한 type flag, 즉 ADDRESS_FAMILY
// 3. 그 외 정보(ip, port 등)들을 저장하기 위한 배열
//	  다양한 형태의 저장방법들이 있기 때문에, 배열을 선택할 수 밖에 없었다.

// 그렇게 하여 sockaddr이 탄생하였다.
// struct sockaddr 
// {
//  	unsigned short sa_family;			// Address family.
//  	char sa_data[14];                   // Up to 14 bytes of direct address.
// };

// 하지만 아직 편의성에 대한 문제가 남아있었다.
// Ipv4로 통신을 하는 경우 char sa_data[14] 라는 배열에 ip와 port에 대한 정보를 담아야 한다.
// 예를 들어  ip : 192.1.2.3  port : 8000 으로 통신을 하고 싶은 경우,
// 위 배열에 어느 위치에 어떤 값을 넣어야 하는지 사용자가 알기 매우 힘들다.

// 그래서 Ipv4를 사용하는 사용자를 위한 구조체를 만들었다.

// struct sockaddr_in 
// {
//  	short sin_family;
//  	unsigned short sin_port;
//  	IN_ADDR sin_addr;		//ip에 대한 정보를 담는다.
//  	char sin_zero[8];
// }

// sockaddr_in은 sockaddr을 사용자가 편하게 초기화 할 수 있도록 하면서,
// sockaddr로 바로 casting이 가능하도록 내부적인 크기와, 변환될 데이터의 순서를 맞추어서 설계하였다.
// 그러므로 casting 시 다음과 같이 대응된다.

// sockaddr						sockaddr_in
// (2 byte) sa_family			(2 byte) sin_family			// 다양한 주소체계를 구분할 type
// (2 byte) sa_data[0~1]		(2 byte) sin_port			// port
// (4 byte) sa_data[2~5]		(4 byte) IN_ADDR			// ip
// (8 byte) sa_data[6~13]		(8 byte) sin_zero[0~7]		// 일단은 공백이지만, 다른 주소체계(Ipv6..)에서는 사용된다.


// 이제 우리는 아래와 같이 초기화 할 수 있게 되었다.

// std::string IP = "127.0.0.1";
// struct sockaddr_in address;
// memset((char *)&address, 0, sizeof(address));
// address.sin_family = AF_INET;
// address.sin_port = 8000;
// address.sin_addr.s_addr = inet_addr(IP.c_str());

// address를 (struct sockaddr*) 타입으로 강제 형변환 하여 sockaddr의 내부 값을 채운다.
// (struct sockaddr*)&address;
// 이제 이것을 bind, send 등등 필요할 때 사용하면 된다.

// 여기서 ip 부분인 sa_data[2~5] 부분을 IN_ADDR로 대응시켰는데,
// 그 이유는 ip를 초기화 하기 위해 사용자에게 한 번 더 편의성을 제공하기 위함이다.
// 예를 들어 192.1.2.3 이라는 ip가 있다고 한다면, 
// ip의 경우 192만 따로 필요한 경우도 있고 3만 따로 필요한 경우도 있다.
// 그러므로 모두 분리되어 있을 때 사용자 입장에서 조금 더 편하다.
// 하지만 또 컴퓨터 입장에서는 ip가 하나의 값(unsigned int - 4 byte)으로 표현되어 있어야 편한 경우도 있다.
// 그러므로 그런 모든 경우에 대해 casting이 가능하도록 한 번 더 IN_ADDR이라는 구조체로 감싼 것이다.


// 운영체제에서 사용하는 레지스터가 32비트 64비트 등등으로 나뉘는데,
// 운영체제 입장에서는 한 번에 위와 같은 단위로 사용해야 연산하기가 편하다.
// 그렇기 때문에 아래와 같은 temp구조체에는 packing을 위한 공백 메모리가 들어가게 되고,
// reinterpret_cast를 할 때 예상했던 크기가 아니기 때문에 문제가 생길 수 있다.

// sockaddr을 구현해보자.
namespace _ex1
{
	// 구조체 정렬
#pragma pack(push, 1)
	struct addr
	{
		unsigned char data;
		char arr[6];
	};

#pragma pack(push, 1)
	struct addr_in
	{
		unsigned char data;
		unsigned short port;
		unsigned int ip;
	};

	// 사용자의 편의를 위해
	void test()
	{
		addr a;
		a.data = 10;
		memset(a.arr, 0, 6);

		// reinterpret_cast는 c++의 가장 위험한 casting 방법으로 바이트 단위로 메모리를 복사해 버린다(타입에 상관없이).
		// 추가 내용
		// c스타일의 casting은 여러가지로 문제가 많은데,
		// 여러가지 스타일의 casting을 모두 포함하고 있기 때문이다.
		// c++는 c의 casting이 포함하는 스타일을 4개로 분류한다.
		// static_cast
		// const_cast
		// dynamic_cast
		// reinterpret_cast
		addr_in& aref = *reinterpret_cast<addr_in*>(&a);

		aref.data = 11;
		aref.port = 8000;
		// ip를 이런 형태로 초기화하는것은 여전히 사용자 입장에서 불편하다.
		// ip 192.211.128.111를 하나의 unsinged int로 표현하면 아래와 같은 값이 된다.
		aref.ip = 1870713792;

		// #pragma pack(push, 1) 을 쓸 때와 쓰지 않을 때 size가 다를 것이다.
		std::cout << sizeof(addr) << " " << sizeof(addr_in) << std::endl;


		// t의 data가 바뀐 것을 볼 수 있다.
		std::cout << a.data;
		for (int i = 0; i < 6; i++)
		{
			std::cout << a.arr[i] << std::endl;
		}
	}
}


// ip 까지 감싸도록 구현해보자.
namespace _ex2
{
#pragma pack(push, 1)
	struct addr
	{
		unsigned char data;
		char arr[6];
	};

	// address를 한 번 더 구조체로 감싸서, 사용자가 좀 더 편하게 ip를 초기화 할 수 있도록 한다.
	struct address
	{
		unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned char d;
	};

	struct addr_in
	{
		unsigned char data;
		unsigned short port;
		address addr;
	};

	void test()
	{
		addr a;
		a.data = 10;
		memset(a.arr, 0, 6);

		addr_in& aref = *reinterpret_cast<addr_in*>(&a);

		aref.data = 11;
		aref.port = 8000;
		aref.addr.a = 127;
		aref.addr.b = 0;
		aref.addr.c = 0;
		aref.addr.d = 1;

		std::cout << (*reinterpret_cast<unsigned int*>(&aref.addr));
	}
}


// 하지만 우리는 이것도 불편하다.
// sockaddr을 더 편하게 사용할 수 있도록 구현해 보자.
// 1. 불편한 생성과정을 모두 객체로 감싼다.
// 2. 

#include "SocketAddress.h"
#include "SocketUtil.h"


namespace _ex3
{
	void test()
	{
		SocketUtil::staticInit();


		string ip = "192.211.128.111";
		string port = "8000";

		// 이것도 불편하니
		SocketAddress a1(htonl(inet_addr(ip.c_str())), stoi(port));
		cout << a1.toString() << endl;

		// 이렇게 사용하자
		SocketAddress* a2 = SocketAddress::createFromString(ip + ":" + port);
		cout << a2->toString() << endl;


		SocketUtil::cleanUp();
	}
}

int main()
{
	//_ex1::test();
	//_ex2::test();
	_ex3::test();
	
	return 0;
}