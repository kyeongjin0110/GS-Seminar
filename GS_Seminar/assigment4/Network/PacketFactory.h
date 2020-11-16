#pragma once

#include <string>
#include <functional>
#include "../flatbuffers/GamePacket.hpp"
#include "../flatbuffers/Packets/LoginPacket_generated.h"
#include "../flatbuffers/Packets/MessagePacket_generated.h"
#include "../flatbuffers/Packets/OkayPacket_generated.h"

class PacketFactory
{
public:
	enum PacketType
	{
		kLogin, kOkay, kMessage
	};

	static GamePacket createLoginPacket(const std::string& name);
	static GamePacket createOkayPacket(int id);
	static GamePacket createMessagePacket(const std::string& name, const std::string& message);

	PacketFactory() {}
	~PacketFactory() {};
};

