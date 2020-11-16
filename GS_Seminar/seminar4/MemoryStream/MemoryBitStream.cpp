#include "MemoryBitStream.h"
#include <algorithm>
#include <iostream>

void OutputMemoryBitStream::writeBits(uint8_t data, uint32_t bit_cnt)
{
	uint32_t nextBitHead = _bit_head + static_cast<uint32_t>(bit_cnt);
	
	if (nextBitHead > _bit_capacity)
	{
		reallocBuffer(std::max(_bit_capacity * 2, nextBitHead));
	}

	//calculate the byteOffset into our buffer
	//by dividing the head by 8
	//and the bitOffset by taking the last 3 bits
	uint32_t byteOffset = _bit_head >> 3;
	uint32_t bitOffset = _bit_head & 0x7;

	uint8_t currentMask = ~(0xff << bitOffset);
	_buffer[byteOffset] = (_buffer[byteOffset] & currentMask) | (data << bitOffset);

	//calculate how many bits were not yet used in
	//our target byte in the buffer
	uint32_t bitsFreeThisByte = 8 - bitOffset;

	//if we needed more than that, carry to the next byte
	if( bitsFreeThisByte < bit_cnt )
	{
		//we need another byte
		_buffer[byteOffset + 1] = data >> bitsFreeThisByte;
	}

	_bit_head = nextBitHead;
}

void OutputMemoryBitStream::writeBits(const void* data, uint32_t bit_cnt)
{
	const char* srcByte = static_cast<const char*>(data);
	//write all the bytes
	while (bit_cnt > 8)
	{
		writeBits(*srcByte, 8);
		++srcByte;
		bit_cnt -= 8;
	}
	//write anything left
	if (bit_cnt > 0)
	{
		writeBits(*srcByte, bit_cnt);
	}
}


void OutputMemoryBitStream::reallocBuffer(uint32_t new_bit_length)
{
	if( _buffer == nullptr )
	{
		//just need to memset on first allocation
		_buffer = static_cast<char*>(std::malloc(new_bit_length >> 3));
		memset(_buffer, 0, new_bit_length >> 3);
	}
	else
	{
		//need to memset, then copy the buffer
		char* buffer = static_cast<char*>(std::malloc(new_bit_length >> 3));
		memset(buffer, 0, new_bit_length >> 3);
		memcpy(buffer, _buffer, _bit_capacity >> 3);
		std::free(_buffer);
		_buffer = buffer;
	}
	
	//handle realloc failure
	//...
	_bit_capacity = new_bit_length;
}


void test1()
{
	OutputMemoryBitStream mbs;
	
	mbs.writeBits(11, 5);
	mbs.writeBits(52, 6);
}

void InputMemoryBitStream::readBits(uint8_t& data, uint32_t bit_cnt)
{
	uint32_t byteOffset = _bit_head >> 3;
	uint32_t bitOffset = _bit_head & 0x7;

	data = static_cast<uint8_t>(_buffer[byteOffset]) >> bitOffset;

	uint32_t bitsFreeThisByte = 8 - bitOffset;
	if( bitsFreeThisByte < bit_cnt )
	{
		//we need another byte
		data |= static_cast<uint8_t>(_buffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	//don't forget a mask so that we only read the bit we wanted...
	data &= (~(0x00ff << bit_cnt));

	_bit_head += bit_cnt;
}

void InputMemoryBitStream::readBits(void* data, uint32_t bit_cnt)
{
	uint8_t* destByte = reinterpret_cast<uint8_t*>(data);
	//write all the bytes
	while (bit_cnt > 8)
	{
		readBits(*destByte, 8);
		++destByte;
		bit_cnt -= 8;
	}
	//write anything left
	if( bit_cnt > 0 )
	{
		readBits(*destByte, bit_cnt);
	}
}

