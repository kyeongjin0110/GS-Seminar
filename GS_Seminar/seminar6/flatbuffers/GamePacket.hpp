#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <iostream>
#include <iomanip>

#include "flatbuffers/flatbuffers.h"

class GamePacket
{
public:
	// header�� ��� ����
	// HEADER_LENGTH �ڸ������� ���
	enum { HEADER_LENGTH = 8 };

	// header�� �� 4�ڸ�, body�� ���̸� ����
	enum { SIZE_HEADER_LENGTH = 4 };

	// header�� �� 4�ڸ�, body�� Ÿ���� ����
	enum { TYPE_HEADER_LENGTH = 4 };

	// body�� ��� ����
	// body�� flatbuffer�� ȣȯ ������ buffer�� ���
	enum { MAX_BODY_LENGTH = 2048 };

	enum { MAX_DATA_LENGTH = HEADER_LENGTH + MAX_BODY_LENGTH };

	GamePacket()
		:
		_body_length(0),
		_type(0)
	{}

	explicit GamePacket(int type)
	{
		setBodyLength(0);
		setType(type);
		encodeHeader();
	}

	// ������ GameMessage�� ���� ������� data�� (header data + flatbuffer data)
	// �� data�� ���̸� �̿��� ������.
	GamePacket(const unsigned char* data_with_header, unsigned int length)
	{
		std::memcpy(getData(), data_with_header, length);
		decodeHeader();
	}

	// flatbuffers�� �̿��� GameMessage ������
	// data�� ����ȭ �� ���� builder��,
	// �� builder�� � data�� ��� �ִ����� ���� type ���ڸ� �־��ش�.
	GamePacket(const flatbuffers::FlatBufferBuilder& builder, int type)
	{
		setBodyLength(builder.GetSize());
		setType(type);

		std::memcpy(getBody(), builder.GetBufferPointer(), _body_length);
		encodeHeader();	
	}

	const unsigned char* getData() const
	{
		return _data;
	}

	unsigned char* getData()
	{
		return _data;
	}

	unsigned int getLength() const
	{
		return HEADER_LENGTH + _body_length;
	}

	const unsigned char* getBody() const
	{
		return _data + HEADER_LENGTH;
	}

	unsigned char* getBody()
	{
		return _data + HEADER_LENGTH;
	}

	unsigned int getBodyLength() const
	{
		return _body_length;
	}

	unsigned int getType() const
	{
		return _type;
	}

	void setBodyLength(unsigned int new_length)
	{
		_body_length = new_length;
		if (_body_length > MAX_BODY_LENGTH)
			std::cout << "Body length is too big!" << std::endl;
	}

	void setType(unsigned int new_type)
	{
		_type = new_type;
	}

	// buffer���� �����Ͱ� ���������, �󸶸�ŭ�� ���̰� ����Ǿ����� �� ����� ����.
	// �׷��Ƿ� header�� body�� �и��ϰ�, header �κп� ���̸� ������ �ش�.
	// �� �Լ��� header�� �����͸� ���̷� ��ȯ�� �ִ� �Լ��̴�.
	// ���� �������� header�� body�� ���̰� �ٸ� ���� �̻��� �ִ� ����̰�
	// �̸� return�� ���� �˷��ش�.
	bool decodeHeader()
	{
		using namespace std; // For strncat and atoi.

		char size_header[SIZE_HEADER_LENGTH + 1];
		memcpy(size_header, _data, SIZE_HEADER_LENGTH);
		_body_length = atoi(size_header);

		char type_header[TYPE_HEADER_LENGTH + 1];
		memcpy(type_header, _data + SIZE_HEADER_LENGTH, TYPE_HEADER_LENGTH);
		_type = atoi(type_header);

		if (_body_length > MAX_BODY_LENGTH)
		{
			std::cout << "Body length is too big!" << std::endl;
			_body_length = 0;
			return false;
		}
		return true;
	}

	// �ݴ�� body�� ���̸�ŭ�� ���ۿ� �ִ� �۾�
	// size header�� type header�� ���̸�ŭ�� ������ �α� ���� setw�� ����Ѵ�.
	void encodeHeader()
	{
		std::stringstream ss;

		ss << std::setw(SIZE_HEADER_LENGTH) << _body_length;
		ss << std::setw(TYPE_HEADER_LENGTH) << _type;

		memcpy(_data, ss.str().c_str(), HEADER_LENGTH);
	}

private:
	// �������� ��ü�� �����ϴ� ����
	unsigned char _data[HEADER_LENGTH + MAX_BODY_LENGTH];

	// �������� ���̸� �����ϴ� ����
	unsigned int _body_length;

	// �������� Ÿ���� �����ϴ� ����
	unsigned int _type;
};
