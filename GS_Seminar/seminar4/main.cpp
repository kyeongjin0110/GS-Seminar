#include <iostream>
#include <bitset>
#include <algorithm>
#include "MemoryStream/MemoryBitStream.h"
using namespace std;

// Utils ..

// Data를 입력받아서 bit 단위로 출력
template<typename T>
void printBinary(T val)
{
	constexpr int size = 8 * sizeof(T);
	bitset<size> bit(val);
	std::cout << bit << std::endl;
}

// Byte buffer를 입력받아서 2진수로 출력
void printBufferState(char* buffer, int printSize, uint32_t bit_head)
{
	cout << "bit head : " << bit_head << endl;
	cout << "bit buffer" << endl;

	for (int i = 0; i < printSize; i++)
		printBinary(buffer[i]);
	cout << endl;
}

// Byte buffer를 입력받아서 2진수로 출력
void printBufferToBinary(char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		printBinary(buffer[i]);
	}
}

// Byte buffer를 입력받아서 2진수로 출력
void printBufferToBinary(unsigned char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		printBinary(buffer[i]);
	}
}

// Byte buffer를 입력받아서 16진수로 출력
void printBufferToHex(unsigned char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		std::cout << std::hex << buffer[i] << " ";
	}
}


// Examples ..

namespace _1_Serialize
{
	// Byte buffer에 data를 bit단위로 
	void writeBits(char* buffer, uint8_t data, uint32_t bit_cnt, uint32_t& bit_head)
	{
		uint32_t nextBitHead = bit_head + static_cast<uint32_t>(bit_cnt);

		// Calculate the byteOffset into our buffer
		// by dividing the head by 8
		// and the bitOffset by taking the last 3 bits
		uint32_t byteOffset = bit_head >> 3;
		uint32_t bitOffset = bit_head & 0x7;

		uint8_t currentMask = ~(0xff << bitOffset);
		buffer[byteOffset] = (buffer[byteOffset] & currentMask) | (data << bitOffset);

		// Calculate how many bits were not yet used in
		// our target byte in the buffer
		uint32_t bitsFreeThisByte = 8 - bitOffset;

		// If we needed more than that, carry to the next byte
		if (bitsFreeThisByte < bit_cnt)
		{
			// We need another byte
			buffer[byteOffset + 1] = data >> bitsFreeThisByte;
		}

		bit_head = nextBitHead;
	}

	void writeBits(char* buffer, const void* data, uint32_t bit_cnt, uint32_t& bit_head)
	{
		const char* src_byte = static_cast<const char*>(data);

		// Write all the bytes
		while (bit_cnt > 8)
		{
			writeBits(buffer, *src_byte, 8, bit_head);
			++src_byte;
			bit_cnt -= 8;
		}
		// Write anything left
		if (bit_cnt > 0)
		{
			writeBits(buffer, *src_byte, bit_cnt, bit_head);
		}
	}

	
	void readBits(char* buffer, uint8_t& data, uint32_t bit_cnt, uint32_t& bit_head)
	{
		uint32_t byteOffset = bit_head >> 3;
		uint32_t bitOffset = bit_head & 0x7;

		data = static_cast<uint8_t>(buffer[byteOffset]) >> bitOffset;

		uint32_t bitsFreeThisByte = 8 - bitOffset;
		if (bitsFreeThisByte < bit_cnt)
		{
			// We need another byte
			data |= static_cast<uint8_t>(buffer[byteOffset + 1]) << bitsFreeThisByte;
		}

		// Don't forget a mask so that we only read the bit we wanted...
		data &= (~(0x00ff << bit_cnt));

		bit_head += bit_cnt;
	}

	void readBits(char* buffer, void* data, uint32_t bit_cnt, uint32_t& bit_head)
	{
		uint8_t* destByte = reinterpret_cast<uint8_t*>(data);

		// Write all the bytes
		while (bit_cnt > 8)
		{
			readBits(buffer, *destByte, 8, bit_head);
			++destByte;
			bit_cnt -= 8;
		}
		// Write anything left
		if (bit_cnt > 0)
		{
			readBits(buffer, *destByte, bit_cnt, bit_head);
		}
	}

	void example()
	{
		std::cout << "_1_Serialize" << std::endl << std::endl;;
		char		buffer[16];
		uint32_t	write_head = 0;
		uint32_t	read_head = 0;

		int a = 14;
		std::cout << "write a : "<< a << std::endl;
		writeBits(buffer, &a, sizeof(a) * 8, write_head);
		printBufferState(buffer, 4, write_head);

		float b = 14.134f;
		std::cout << "write b : " << b << std::endl;
		writeBits(buffer, &b, sizeof(b) * 8, write_head);
		printBufferState(buffer, 8, write_head);

		double c = 5661.994;
		std::cout << "write c : " << c << std::endl;
		writeBits(buffer, &c, sizeof(c) * 8, write_head);
		printBufferState(buffer, 16, write_head);

		// client -> server

		// server recv

		int ra;
		readBits(buffer, &ra, sizeof(ra) * 8, read_head);
		cout << "read a : " << ra << endl << endl;

		float rb;
		readBits(buffer, &rb, sizeof(rb) * 8, read_head);
		cout << "read b : " << rb << endl << endl;

		double rc;
		readBits(buffer, &rc, sizeof(rc) * 8, read_head);
		cout << "read c : " << rc << endl << endl;
	}
}

namespace _2_MemoryStream
{
	void example()
	{
		std::cout << "_2_MemoryStream" << std::endl << std::endl;

		OutputMemoryBitStream out;
		int a = 8;
		float b = 17.75f;
		std::string c = "abcd";

		std::cout << "write a : " << a << std::endl;
		out.write(a);
		std::cout << "write b : " << b << std::endl;
		out.write(b);
		std::cout << "write c : " << c << std::endl;
		out.write(c);

		std::cout << out.getByteLength() << " " << out.getBitLength() << endl;;
		printBufferToBinary(const_cast<char*>(out.getBuffer()), out.getByteLength());

		InputMemoryBitStream inputStream(
			const_cast<char*>(out.getBuffer()), out.getBitLength());

		int ia;
		inputStream.read(ia);
		std::cout << "read a : " << ia << std::endl;

		float ib;
		inputStream.read(ib);
		std::cout << "read b : " << ib << std::endl;

		std::string ic;
		inputStream.read(ic);
		std::cout << "read c : " << ic << std::endl;
	}
}

// 직렬화를 위한 library를 직접 만들어 보았다.
// 하지만 MemoryStream은 여전히 해결하지 못한 여러가지 문제점들을 갖고 있다.
//
// 1. 일단 첫번째로, stream에 작성된 내용을 명시적으로 알 방법이 없다.
//	  예를 들어서 float x, float y, string str을 직렬화해서 stream으로 write을 통해 넣고,
//	  이후에 buffer를 통해 다시 read를 할 때는 (packet을 recv한 경우),
//	  어떤 직렬화된 data를 어떤 타입, 어떤 순서로 read해야 하는지 개발자가 기억하고 있어야 한다.
//
// 2. 두번째로 새로운 복합 data가 정의된다면,
//	  그에 대한 직렬화/역직렬화 변환에 대한 모든 코드를 직접 작성해야 한다.
//
// 3. 세번째로 만약 array, vector, map과 같은 container 형태의 데이터를 저장하기가 쉽지 않다.
//	  물론 그에 필요한 코드를 모두 직접 작성하면 되겠지만,
//	  만약 vector 안에 Monster라는 객체가 있고, 또 Monster안에 array가 있는 형태의
//	  복잡한 data라면 코드 작성이 만만치가 않다.
//
// 이러한 복합적인 문제를 해결하기 위해,
// google에서는 flatbuffers라는 직렬화 library를 개발하였다.
// 직렬화를 위해 flatbuffers의 장점과 사용법을 알아보자.
//
// flatbuffers library에서는 복잡한 형태의 data도
// 직렬화/역직렬화가 가능하도록 만들면서,
// 개발자가 코드를 작성하는 부담도 줄이기 위해서,
// IDL(Interface Definition Language)과 IDL 컴파일러를 이용하여
// source code를 생성해주는 방법을 사용한다.
//
// 코드 생성 방법은 다음과 같다.
// IDL이란 간단하게 data를 정의하는 언어인데 다음과 같이 정의한다.
// 자세한 내용은 https://google.github.io/flatbuffers/md__schemas.html
//
// namespace MyGame;
// table pos
// {
//	 x:float;
//	 y:float;
//	 z:float;
// }
// root_type pos;
//
// 이 코드를 simple_data.fbs로 저장한다.
// flatc.exe가 있는 파일 ~\GS_Seminar\seminar4\flatbuffers\code_gen 에
// simple_data.fbs를 넣고 cmd창을 킨다.
// ~\GS_Seminar\seminar4\flatbuffers\code_gen 디렉터리로 이동 후,
// flatc -c simple_data.fbs
// 명령어를 치면 simple_data_generated.h 파일이 생성된다.
// 이 파일에는 simple_data 파일에 정의된 data를,
// 직렬화/역직렬화하기 위해 필요한 모든 코드와,
// 성능향상, 검증 등등 추가적으로 필요한 코드가 모두 들어가 있다.
// 즉 우리는 이 파일을 이용하여 pos라는 data를 마음대로 직렬화할 수 있게 된 것이다.
//

// 사용법을 알아보자
// 먼저 생성된 코드를 포함시킨다.

#include "flatbuffers\test_generated.h"

// 생성된 코드에 대한 idl은 resource파일에 simple_data.fbs에 있다.

namespace _3_flatbuffers1
{
	// 일부 운영체제에서는 signed data의 경우 sign bit로 인한 문제가 생길 수 있으므로,
	// unsigned를 사용하는게 byte크기의 data를 전달하는데 가장 안전하다.

	// flatbuffer library를 이용한 data 직렬화 예제
	void write(unsigned char* buffer, size_t& length)
	{
		flatbuffers::FlatBufferBuilder builder;

		float x = 324.0f;
		float y = 124.0f;
		float z = -91.0f;

		auto fdata = test::Createposition(builder, x, y, z);
		builder.Finish(fdata);

		// 직렬화된 data
		memcpy(buffer, builder.GetBufferPointer() , builder.GetSize());

		// 직렬화된 data size
		length = builder.GetSize();

		/*
		// flatbuffer 직렬화 builder를 만든다.
		flatbuffers::FlatBufferBuilder builder;

		// data를 만든다.
		float x = 1441.5f;
		float y = -554.7f;
		float z = 994.88f;
		// pos 직렬화를 위한 중간 data 생성
		auto pos_data = MyGame::Createpos(builder, x, y, z);

		// pos data를 builder의 내부 buffer에 저장한다.
		builder.Finish(pos_data);

		// builder의 내부 data를 buffer 배열에 저장한다.
		memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());

		// 길이도 저장 (call by reference)
		length = builder.GetSize();

		// 네트워크로 send를 한다면 위 buffer를 send하면 된다.
		*/
	}

	// flatbuffers library를 이용한 data 역직렬화 예제
	void read(unsigned char* buffer, size_t length)
	{
		// 검증
		if (test::VerifypositionBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// 역직렬화
		auto pos = test::Getposition(buffer);

		std::cout
			<< pos->x() << " "
			<< pos->y() << " "
			<< pos->z() << std::endl;
		/*
		// buffer의 내용은 네트워크로부터 recv 되었다고 생각해 볼 수도 있다.

		// data가 손상되지 않았는지 검증한다.
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// Getpos 함수를 이용하여 buffer의 내용을 구조체의 형태로 변환한다.(역직렬화)
		auto pos = MyGame::Getpos(buffer);

		cout << pos->x() << ", " << pos->y() << ", " << pos->z() << endl;
		*/
	}

	void example()
	{
		std::cout << "_3_flatbuffers1" << std::endl << std::endl;
		unsigned char buffer[1024] = { 0 };
		size_t length = 0;

		//client
		write(buffer, length);

		// send
		// network 통신
		// recv

		read(buffer, length);
	}
}

// flatbuffers의 진정한 위력은 직렬화할 data가 복잡해질 때 발휘된다.
// 복잡한 data에 대한 직렬화를 수행해 보자.

#include "flatbuffers\data1_generated.h"
namespace _4_flatbuffers2
{
	void write(unsigned char* buffer, size_t& length)
	{
		flatbuffers::FlatBufferBuilder builder;

		// 먼저 weapon을 만든다.
		short damage = 15;
		string weapon1 = "sword";
		string weapon2 = "speer";

		// string의 경우에는 직렬화 가능한 flatbuffer string을 변환과정이 필요하다.
		auto encode_weapon1 = builder.CreateString(weapon1);
		auto encode_weapon2 = builder.CreateString(weapon2);

		auto sword = MyGame::CreateWeapon(builder, encode_weapon1, damage);
		auto axe = MyGame::CreateWeapon(builder, encode_weapon2, damage);

		// container의 직렬화
		std::vector<flatbuffers::Offset<MyGame::Weapon>> weapons_vector;
		weapons_vector.push_back(sword);
		weapons_vector.push_back(axe);
		auto weapons = builder.CreateVector(weapons_vector);

		// monster의 직렬화
		float x = 10.0f;
		float y = 15.0f;
		float z = -10.0f;
		short mana = 100;
		short hp = 200;

		string name = "monster1";
		auto encode_name = builder.CreateString(name);
		auto monster_data =
			MyGame::CreateMonster(builder, x, y, z, mana, hp, encode_name, weapons);

		// 끝.
		builder.Finish(monster_data);

		// buffer에 저장한다.
		std::memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
		length = builder.GetSize();
	}

	void read(unsigned char* buffer, size_t length)
	{
		// data가 손상되지 않았는지 검증한다.
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// buffer의 직렬화된 data를 역직렬화 한다.
		auto monster = MyGame::GetMonster(buffer);

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
		std::cout << "_4_flatbuffers2" << std::endl << std::endl;

		uint8_t buffer[1024] = { 0 };
		size_t length = 0;

		write(buffer, length);

		// 통신~

		read(buffer, length);

		//std::cout << std::endl << "2진수" << std::endl;
		//printBufferToBinary(buffer, length);
		//std::cout << std::endl << "16진수" << std::endl;
		//printBufferToHex(buffer, length);
	}
}

// flatbuffer라는 강력한 도구를 통해 복잡한 data도 직렬화 할 수 있게 되었다.
// 하지만, 여기서 생각해야 하는 중요한 부분이 있다.
// _4_flatbuffers2 예제에서 네트워크로부터 recv를 하고 난 후,
// 그 data를 MyGame::GetMonster() 함수로 역직렬화 하였다.
// 근데 recv하는 입장에서는 그 buffer에 어떤 데이터가 들어있는지 알 길이 없다.
// 그러므로 그 데이터를 MyGame::GetMonster() 함수로 역직렬화 해야한다는 사실도 알 방법이 없다.
// 즉 보낼 때 어떤 타입의 데이터인지와,
// 길이가 얼마인지에 대한 정보를 packet의 header부분에 추가해서 data를 전송해 주어야 한다.
// 그 과정을 편리하게 수행하기 위해 flatbuffer를 한번 wrapping한 GameMessage라는 class를 만든다.

#include "flatbuffers\GamePacket.hpp"
#include "flatbuffers\simple_data_generated.h"

namespace _5_GamePacket
{
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



namespace practice
{
	void example()
	{

	}
}



int main()
{
	//_1_Serialize::example();
	//_2_MemoryStream::example();
	//_3_flatbuffers1::example();
	//_4_flatbuffers2::example();
	//_5_GamePacket::example();
	return 0;
}

