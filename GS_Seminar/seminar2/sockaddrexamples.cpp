#include <iostream>
using namespace std;

// �⺻������ socket address�� ����� ���ؼ� ����ؾ� �ߴ� ���׵��� �־���.
// 1. Ipv4, Ipv6 ��� �پ��� address ü����� ���ÿ� ����
// 2. ������ ����� �پ��� address���� �����ϱ� ���� type flag, �� ADDRESS_FAMILY
// 3. �� �� ����(ip, port ��)���� �����ϱ� ���� �迭
//	  �پ��� ������ ���������� �ֱ� ������, �迭�� ������ �� �ۿ� ������.

// �׷��� �Ͽ� sockaddr�� ź���Ͽ���.
// struct sockaddr 
// {
//  	unsigned short sa_family;			// Address family.
//  	char sa_data[14];                   // Up to 14 bytes of direct address.
// };

// ������ ���� ���Ǽ��� ���� ������ �����־���.
// Ipv4�� ����� �ϴ� ��� char sa_data[14] ��� �迭�� ip�� port�� ���� ������ ��ƾ� �Ѵ�.
// ���� ���  ip : 192.1.2.3  port : 8000 ���� ����� �ϰ� ���� ���,
// �� �迭�� ��� ��ġ�� � ���� �־�� �ϴ��� ����ڰ� �˱� �ſ� �����.

// �׷��� Ipv4�� ����ϴ� ����ڸ� ���� ����ü�� �������.

// struct sockaddr_in 
// {
//  	short sin_family;
//  	unsigned short sin_port;
//  	IN_ADDR sin_addr;		//ip�� ���� ������ ��´�.
//  	char sin_zero[8];
// }

// sockaddr_in�� sockaddr�� ����ڰ� ���ϰ� �ʱ�ȭ �� �� �ֵ��� �ϸ鼭,
// sockaddr�� �ٷ� casting�� �����ϵ��� �������� ũ���, ��ȯ�� �������� ������ ���߾ �����Ͽ���.
// �׷��Ƿ� casting �� ������ ���� �����ȴ�.

// sockaddr						sockaddr_in
// (2 byte) sa_family			(2 byte) sin_family			// �پ��� �ּ�ü�踦 ������ type
// (2 byte) sa_data[0~1]		(2 byte) sin_port			// port
// (4 byte) sa_data[2~5]		(4 byte) IN_ADDR			// ip
// (8 byte) sa_data[6~13]		(8 byte) sin_zero[0~7]		// �ϴ��� ����������, �ٸ� �ּ�ü��(Ipv6..)������ ���ȴ�.


// ���� �츮�� �Ʒ��� ���� �ʱ�ȭ �� �� �ְ� �Ǿ���.

// std::string IP = "127.0.0.1";
// struct sockaddr_in address;
// memset((char *)&address, 0, sizeof(address));
// address.sin_family = AF_INET;
// address.sin_port = 8000;
// address.sin_addr.s_addr = inet_addr(IP.c_str());

// address�� (struct sockaddr*) Ÿ������ ���� ����ȯ �Ͽ� sockaddr�� ���� ���� ä���.
// (struct sockaddr*)&address;
// ���� �̰��� bind, send ��� �ʿ��� �� ����ϸ� �ȴ�.

// ���⼭ ip �κ��� sa_data[2~5] �κ��� IN_ADDR�� �������״µ�,
// �� ������ ip�� �ʱ�ȭ �ϱ� ���� ����ڿ��� �� �� �� ���Ǽ��� �����ϱ� �����̴�.
// ���� ��� 192.1.2.3 �̶�� ip�� �ִٰ� �Ѵٸ�, 
// ip�� ��� 192�� ���� �ʿ��� ��쵵 �ְ� 3�� ���� �ʿ��� ��쵵 �ִ�.
// �׷��Ƿ� ��� �и��Ǿ� ���� �� ����� ���忡�� ���� �� ���ϴ�.
// ������ �� ��ǻ�� ���忡���� ip�� �ϳ��� ��(unsigned int - 4 byte)���� ǥ���Ǿ� �־�� ���� ��쵵 �ִ�.
// �׷��Ƿ� �׷� ��� ��쿡 ���� casting�� �����ϵ��� �� �� �� IN_ADDR�̶�� ����ü�� ���� ���̴�.


// �ü������ ����ϴ� �������Ͱ� 32��Ʈ 64��Ʈ ������� �����µ�,
// �ü�� ���忡���� �� ���� ���� ���� ������ ����ؾ� �����ϱⰡ ���ϴ�.
// �׷��� ������ �Ʒ��� ���� temp����ü���� packing�� ���� ���� �޸𸮰� ���� �ǰ�,
// reinterpret_cast�� �� �� �����ߴ� ũ�Ⱑ �ƴϱ� ������ ������ ���� �� �ִ�.

// sockaddr�� �����غ���.
namespace _ex1
{
	// ����ü ����
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

	// ������� ���Ǹ� ����
	void test()
	{
		addr a;
		a.data = 10;
		memset(a.arr, 0, 6);

		// reinterpret_cast�� c++�� ���� ������ casting ������� ����Ʈ ������ �޸𸮸� ������ ������(Ÿ�Կ� �������).
		// �߰� ����
		// c��Ÿ���� casting�� ���������� ������ ������,
		// �������� ��Ÿ���� casting�� ��� �����ϰ� �ֱ� �����̴�.
		// c++�� c�� casting�� �����ϴ� ��Ÿ���� 4���� �з��Ѵ�.
		// static_cast
		// const_cast
		// dynamic_cast
		// reinterpret_cast
		addr_in& aref = *reinterpret_cast<addr_in*>(&a);

		aref.data = 11;
		aref.port = 8000;
		// ip�� �̷� ���·� �ʱ�ȭ�ϴ°��� ������ ����� ���忡�� �����ϴ�.
		// ip 192.211.128.111�� �ϳ��� unsinged int�� ǥ���ϸ� �Ʒ��� ���� ���� �ȴ�.
		aref.ip = 1870713792;

		// #pragma pack(push, 1) �� �� ���� ���� ���� �� size�� �ٸ� ���̴�.
		std::cout << sizeof(addr) << " " << sizeof(addr_in) << std::endl;


		// t�� data�� �ٲ� ���� �� �� �ִ�.
		std::cout << a.data;
		for (int i = 0; i < 6; i++)
		{
			std::cout << a.arr[i] << std::endl;
		}
	}
}


// ip ���� ���ε��� �����غ���.
namespace _ex2
{
#pragma pack(push, 1)
	struct addr
	{
		unsigned char data;
		char arr[6];
	};

	// address�� �� �� �� ����ü�� ���μ�, ����ڰ� �� �� ���ϰ� ip�� �ʱ�ȭ �� �� �ֵ��� �Ѵ�.
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


// ������ �츮�� �̰͵� �����ϴ�.
// sockaddr�� �� ���ϰ� ����� �� �ֵ��� ������ ����.
// 1. ������ ���������� ��� ��ü�� ���Ѵ�.
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

		// �̰͵� �����ϴ�
		SocketAddress a1(htonl(inet_addr(ip.c_str())), stoi(port));
		cout << a1.toString() << endl;

		// �̷��� �������
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