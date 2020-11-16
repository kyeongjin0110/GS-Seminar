#pragma once

#include <string>
#include <functional>
#include <vector>
#include "../flatbuffers/GamePacket.hpp"
#include "../flatbuffers/Data/AppointedData_generated.h"
#include "../flatbuffers/Data/DisconnectedData_generated.h"
#include "../flatbuffers/Data/MessageData_generated.h"
#include "../flatbuffers/Data/UserData_generated.h"
#include "../flatbuffers/Data/IntroData_generated.h"
#include "../flatbuffers/Data/JoinedData_generated.h"

class PacketFactory
{
public:
	enum PacketType
	{
		kHello,					// UserData
		kIntro,					// UserData, AppointedData
		kJoined,				// UserData[], AppointedData
		kMessage,				// UserData, string(msg)
		kNotifyDisconnected,	// int(disconnectedID), AppointedData 
		kEnterPlaying,
		kEnterStarting,
		kFull,
		kRequestStart, 
		kReady,
		kDisconnection
	};

	static GamePacket createPacket(PacketType type);
	static GamePacket createHelloPacket(int id, const std::string& name);
	static GamePacket createIntroPacket(int id, const std::string& name, int appointedID, bool changed);
	static GamePacket createJoinedPacket(
		const std::vector<std::pair<int, std::string> >& users,
		int appointedID, bool changed);
	static GamePacket createMessagePacket(int id, const std::string& name, const std::string& message);
	static GamePacket createNotifyDisconnectedPacket(int disconnectdID, int appointedID, bool changed);



	PacketFactory() {}
	~PacketFactory() {};
};

