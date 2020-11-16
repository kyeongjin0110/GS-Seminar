
#include <cstdint>
#include <cstdlib>
#include <string>

class GameObject;

class OutputMemoryBitStream
{
public:

	OutputMemoryBitStream() 
		:
		_bit_head(0),
		_buffer(nullptr)
	{
		reallocBuffer(1500 * 8);
	}

	~OutputMemoryBitStream() { std::free(_buffer); }

	void writeBits(uint8_t data, uint32_t bit_cnt);
	void writeBits(const void* data, uint32_t bit_cnt);

	const char*	getBuffer() const		{ return _buffer; }
	uint32_t getBitLength() const		{ return _bit_head; }
	uint32_t getByteLength() const		{ return (_bit_head + 7) >> 3; }

	void writeBytes(const void* data, uint32_t byte_cnt) { writeBits(data, byte_cnt << 3); }

	/*
	void write( uint32_t inData, uint32_t bit_cnt = 32 )	{ writeBits( &inData, bit_cnt ); }
	void write( int inData, uint32_t bit_cnt = 32 )		{ writeBits( &inData, bit_cnt ); }
	void write( float inData )								{ writeBits( &inData, 32 ); }

	void write( uint16_t inData, uint32_t bit_cnt = 16 )	{ writeBits( &inData, bit_cnt ); }
	void write( int16_t inData, uint32_t bit_cnt = 16 )	{ writeBits( &inData, bit_cnt ); }

	void write( uint8_t inData, uint32_t bit_cnt = 8 )	{ writeBits( &inData, bit_cnt ); }
	*/
	
	template< typename T >
	void write(T data, uint32_t bit_cnt = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic< T >::value || 
					  std::is_enum< T >::value,
					  "Generic write only supports primitive data types");
		writeBits(&data, bit_cnt);
	}
	
	void write(bool data) { writeBits(&data, 1); }

	void write(const std::string& str)
	{
		uint32_t element_cnt = static_cast<uint32_t>(str.size());
		write(element_cnt);
		for( const auto& element : str )
		{
			write(element);
		}
	}
	
private:
	void		reallocBuffer(uint32_t new_bit_capacity);

	char*		_buffer;
	uint32_t	_bit_head;
	uint32_t	_bit_capacity;
};

class InputMemoryBitStream
{
public:
	
	InputMemoryBitStream(char* buffer, uint32_t bit_cnt)
		:
	_buffer(buffer),
	_bit_capacity(bit_cnt),
	_bit_head(0),
	_is_buffer_owner(false) 
	{}
	
	InputMemoryBitStream(const InputMemoryBitStream& copy)
		:
	_bit_capacity(copy._bit_capacity),
	_bit_head(copy._bit_head),
	_is_buffer_owner(true)
	{
		//allocate buffer of right size
		int byteCount = _bit_capacity / 8;
		_buffer = static_cast<char*>(malloc(byteCount));

		//copy
		memcpy(_buffer, copy._buffer, byteCount);
	}
	
	~InputMemoryBitStream() { if (_is_buffer_owner) { free(_buffer); } }
	
	const char*	getBuffer() const			{ return _buffer; }
	uint32_t getRemainingBitCount() const	{ return _bit_capacity - _bit_head; }

	void readBits(uint8_t& data, uint32_t bit_cnt);
	void readBits(void* data, uint32_t bit_cnt);

	void readBytes(void* data, uint32_t inByteCount) { readBits(data, inByteCount << 3); }

	template< typename T >
	void read(T& inData, uint32_t bit_cnt = sizeof(T) * 8)
	{
		static_assert( std::is_arithmetic< T >::value ||
					  std::is_enum< T >::value,
					  "Generic read only supports primitive data types" );
		readBits( &inData, bit_cnt );
	}
	
	void read(uint32_t& data, uint32_t bit_cnt = 32)	{ readBits(&data, bit_cnt); }
	void read(int& data, uint32_t bit_cnt = 32)			{ readBits(&data, bit_cnt); }
	void read(float& data)								{ readBits(&data, 32); }

	void read(uint16_t& data, uint32_t bit_cnt = 16)	{ readBits(&data, bit_cnt); }
	void read(int16_t& data, uint32_t bit_cnt = 16)		{ readBits(&data, bit_cnt); }

	void read(uint8_t& data, uint32_t bit_cnt = 8)		{ readBits(&data, bit_cnt); }
	void read(bool& data)								{ readBits(&data, 1); }
	
	void resetToCapacity(uint32_t byte_capacity)		{ _bit_capacity = byte_capacity << 3; _bit_head = 0; }


	void read(std::string& str)
	{
		uint32_t element_cnt;
		read(element_cnt);
		str.resize(element_cnt);
		for (auto& element : str)
		{
			read(element);
		}
	}

private:

	char*		_buffer;
	uint32_t	_bit_head;
	uint32_t	_bit_capacity;
	bool		_is_buffer_owner;
};

