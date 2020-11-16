#include <iostream>
#include <bitset>
#include <algorithm>
#include "MemoryStream/MemoryBitStream.h"
using namespace std;

// Utils ..

// Data�� �Է¹޾Ƽ� bit ������ ���
template<typename T>
void printBinary(T val)
{
	constexpr int size = 8 * sizeof(T);
	bitset<size> bit(val);
	std::cout << bit << std::endl;
}

// Byte buffer�� �Է¹޾Ƽ� 2������ ���
void printBufferState(char* buffer, int printSize, uint32_t bit_head)
{
	cout << "bit head : " << bit_head << endl;
	cout << "bit buffer" << endl;

	for (int i = 0; i < printSize; i++)
		printBinary(buffer[i]);
	cout << endl;
}

// Byte buffer�� �Է¹޾Ƽ� 2������ ���
void printBufferToBinary(char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		printBinary(buffer[i]);
	}
}

// Byte buffer�� �Է¹޾Ƽ� 2������ ���
void printBufferToBinary(unsigned char* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		printBinary(buffer[i]);
	}
}

// Byte buffer�� �Է¹޾Ƽ� 16������ ���
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
	// Byte buffer�� data�� bit������ 
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

// ����ȭ�� ���� library�� ���� ����� ���Ҵ�.
// ������ MemoryStream�� ������ �ذ����� ���� �������� ���������� ���� �ִ�.
//
// 1. �ϴ� ù��°��, stream�� �ۼ��� ������ ��������� �� ����� ����.
//	  ���� �� float x, float y, string str�� ����ȭ�ؼ� stream���� write�� ���� �ְ�,
//	  ���Ŀ� buffer�� ���� �ٽ� read�� �� ���� (packet�� recv�� ���),
//	  � ����ȭ�� data�� � Ÿ��, � ������ read�ؾ� �ϴ��� �����ڰ� ����ϰ� �־�� �Ѵ�.
//
// 2. �ι�°�� ���ο� ���� data�� ���ǵȴٸ�,
//	  �׿� ���� ����ȭ/������ȭ ��ȯ�� ���� ��� �ڵ带 ���� �ۼ��ؾ� �Ѵ�.
//
// 3. ����°�� ���� array, vector, map�� ���� container ������ �����͸� �����ϱⰡ ���� �ʴ�.
//	  ���� �׿� �ʿ��� �ڵ带 ��� ���� �ۼ��ϸ� �ǰ�����,
//	  ���� vector �ȿ� Monster��� ��ü�� �ְ�, �� Monster�ȿ� array�� �ִ� ������
//	  ������ data��� �ڵ� �ۼ��� ����ġ�� �ʴ�.
//
// �̷��� �������� ������ �ذ��ϱ� ����,
// google������ flatbuffers��� ����ȭ library�� �����Ͽ���.
// ����ȭ�� ���� flatbuffers�� ������ ������ �˾ƺ���.
//
// flatbuffers library������ ������ ������ data��
// ����ȭ/������ȭ�� �����ϵ��� ����鼭,
// �����ڰ� �ڵ带 �ۼ��ϴ� �δ㵵 ���̱� ���ؼ�,
// IDL(Interface Definition Language)�� IDL �����Ϸ��� �̿��Ͽ�
// source code�� �������ִ� ����� ����Ѵ�.
//
// �ڵ� ���� ����� ������ ����.
// IDL�̶� �����ϰ� data�� �����ϴ� ����ε� ������ ���� �����Ѵ�.
// �ڼ��� ������ https://google.github.io/flatbuffers/md__schemas.html
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
// �� �ڵ带 simple_data.fbs�� �����Ѵ�.
// flatc.exe�� �ִ� ���� ~\GS_Seminar\seminar4\flatbuffers\code_gen ��
// simple_data.fbs�� �ְ� cmdâ�� Ų��.
// ~\GS_Seminar\seminar4\flatbuffers\code_gen ���͸��� �̵� ��,
// flatc -c simple_data.fbs
// ��ɾ ġ�� simple_data_generated.h ������ �����ȴ�.
// �� ���Ͽ��� simple_data ���Ͽ� ���ǵ� data��,
// ����ȭ/������ȭ�ϱ� ���� �ʿ��� ��� �ڵ��,
// �������, ���� ��� �߰������� �ʿ��� �ڵ尡 ��� �� �ִ�.
// �� �츮�� �� ������ �̿��Ͽ� pos��� data�� ������� ����ȭ�� �� �ְ� �� ���̴�.
//

// ������ �˾ƺ���
// ���� ������ �ڵ带 ���Խ�Ų��.

#include "flatbuffers\test_generated.h"

// ������ �ڵ忡 ���� idl�� resource���Ͽ� simple_data.fbs�� �ִ�.

namespace _3_flatbuffers1
{
	// �Ϻ� �ü�������� signed data�� ��� sign bit�� ���� ������ ���� �� �����Ƿ�,
	// unsigned�� ����ϴ°� byteũ���� data�� �����ϴµ� ���� �����ϴ�.

	// flatbuffer library�� �̿��� data ����ȭ ����
	void write(unsigned char* buffer, size_t& length)
	{
		flatbuffers::FlatBufferBuilder builder;

		float x = 324.0f;
		float y = 124.0f;
		float z = -91.0f;

		auto fdata = test::Createposition(builder, x, y, z);
		builder.Finish(fdata);

		// ����ȭ�� data
		memcpy(buffer, builder.GetBufferPointer() , builder.GetSize());

		// ����ȭ�� data size
		length = builder.GetSize();

		/*
		// flatbuffer ����ȭ builder�� �����.
		flatbuffers::FlatBufferBuilder builder;

		// data�� �����.
		float x = 1441.5f;
		float y = -554.7f;
		float z = 994.88f;
		// pos ����ȭ�� ���� �߰� data ����
		auto pos_data = MyGame::Createpos(builder, x, y, z);

		// pos data�� builder�� ���� buffer�� �����Ѵ�.
		builder.Finish(pos_data);

		// builder�� ���� data�� buffer �迭�� �����Ѵ�.
		memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());

		// ���̵� ���� (call by reference)
		length = builder.GetSize();

		// ��Ʈ��ũ�� send�� �Ѵٸ� �� buffer�� send�ϸ� �ȴ�.
		*/
	}

	// flatbuffers library�� �̿��� data ������ȭ ����
	void read(unsigned char* buffer, size_t length)
	{
		// ����
		if (test::VerifypositionBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// ������ȭ
		auto pos = test::Getposition(buffer);

		std::cout
			<< pos->x() << " "
			<< pos->y() << " "
			<< pos->z() << std::endl;
		/*
		// buffer�� ������ ��Ʈ��ũ�κ��� recv �Ǿ��ٰ� ������ �� ���� �ִ�.

		// data�� �ջ���� �ʾҴ��� �����Ѵ�.
		if (MyGame::VerifyposBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// Getpos �Լ��� �̿��Ͽ� buffer�� ������ ����ü�� ���·� ��ȯ�Ѵ�.(������ȭ)
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
		// network ���
		// recv

		read(buffer, length);
	}
}

// flatbuffers�� ������ ������ ����ȭ�� data�� �������� �� ���ֵȴ�.
// ������ data�� ���� ����ȭ�� ������ ����.

#include "flatbuffers\data1_generated.h"
namespace _4_flatbuffers2
{
	void write(unsigned char* buffer, size_t& length)
	{
		flatbuffers::FlatBufferBuilder builder;

		// ���� weapon�� �����.
		short damage = 15;
		string weapon1 = "sword";
		string weapon2 = "speer";

		// string�� ��쿡�� ����ȭ ������ flatbuffer string�� ��ȯ������ �ʿ��ϴ�.
		auto encode_weapon1 = builder.CreateString(weapon1);
		auto encode_weapon2 = builder.CreateString(weapon2);

		auto sword = MyGame::CreateWeapon(builder, encode_weapon1, damage);
		auto axe = MyGame::CreateWeapon(builder, encode_weapon2, damage);

		// container�� ����ȭ
		std::vector<flatbuffers::Offset<MyGame::Weapon>> weapons_vector;
		weapons_vector.push_back(sword);
		weapons_vector.push_back(axe);
		auto weapons = builder.CreateVector(weapons_vector);

		// monster�� ����ȭ
		float x = 10.0f;
		float y = 15.0f;
		float z = -10.0f;
		short mana = 100;
		short hp = 200;

		string name = "monster1";
		auto encode_name = builder.CreateString(name);
		auto monster_data =
			MyGame::CreateMonster(builder, x, y, z, mana, hp, encode_name, weapons);

		// ��.
		builder.Finish(monster_data);

		// buffer�� �����Ѵ�.
		std::memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
		length = builder.GetSize();
	}

	void read(unsigned char* buffer, size_t length)
	{
		// data�� �ջ���� �ʾҴ��� �����Ѵ�.
		if (MyGame::VerifyMonsterBuffer(flatbuffers::Verifier(buffer, length)))
			cout << "good!" << endl;
		else
			cout << "bad!" << endl;

		// buffer�� ����ȭ�� data�� ������ȭ �Ѵ�.
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

		// ���~

		read(buffer, length);

		//std::cout << std::endl << "2����" << std::endl;
		//printBufferToBinary(buffer, length);
		//std::cout << std::endl << "16����" << std::endl;
		//printBufferToHex(buffer, length);
	}
}

// flatbuffer��� ������ ������ ���� ������ data�� ����ȭ �� �� �ְ� �Ǿ���.
// ������, ���⼭ �����ؾ� �ϴ� �߿��� �κ��� �ִ�.
// _4_flatbuffers2 �������� ��Ʈ��ũ�κ��� recv�� �ϰ� �� ��,
// �� data�� MyGame::GetMonster() �Լ��� ������ȭ �Ͽ���.
// �ٵ� recv�ϴ� ���忡���� �� buffer�� � �����Ͱ� ����ִ��� �� ���� ����.
// �׷��Ƿ� �� �����͸� MyGame::GetMonster() �Լ��� ������ȭ �ؾ��Ѵٴ� ��ǵ� �� ����� ����.
// �� ���� �� � Ÿ���� ������������,
// ���̰� �������� ���� ������ packet�� header�κп� �߰��ؼ� data�� ������ �־�� �Ѵ�.
// �� ������ ���ϰ� �����ϱ� ���� flatbuffer�� �ѹ� wrapping�� GameMessage��� class�� �����.

#include "flatbuffers\GamePacket.hpp"
#include "flatbuffers\simple_data_generated.h"

namespace _5_GamePacket
{
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

