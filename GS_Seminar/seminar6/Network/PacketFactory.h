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
#include "../flatbuffers/Data/RoomData_generated.h"
#include "../flatbuffers/Data/RoomsData_generated.h"
#include "../flatbuffers/Data/ForwardUserData_generated.h"
#include "../flatbuffers/Data/DestroyRoomData_generated.h"

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
		kDisconnection,

		kCreateRoom,
		kRoomIntro,
		kJoinRoom,
		kRoomIsCreated,
		kRoomInfo,

		kClientHasJoined,
		kRoomHasDestroyed,
		kRequestShowRoomInfo,
		kSize
	};
	static std::string packet_name[PacketType::kSize];
	static void setPacketNames();

	static GamePacket createPacket(PacketType type);
	static GamePacket createHelloPacket(int id, const std::string& name);
	static GamePacket createIntroPacket(int id, const std::string& name, int appointedID, bool changed);
	static GamePacket createJoinedPacket(
		const std::vector<std::pair<int, std::string> >& users,
		int joinedID,
		int appointedID, bool changed);
	static GamePacket createMessagePacket(int id, const std::string& name, const std::string& message);
	static GamePacket createNotifyDisconnectedPacket(int disconnectdID, int appointedID, bool changed);

	static GamePacket createCreateRoomPacket(int id, const std::string& name);
	static GamePacket createRoomIntroPacket(int number, const std::string& address);
	static GamePacket createJoinRoomPacket(int number);
	static GamePacket createRoomIsCreatedPacket(int number, const std::string& address);
	static GamePacket createRoomInfoPacket(const std::vector<std::pair<int, std::string> >& rooms);

	static GamePacket createClientHasJoinedPacket(int id, const std::string& name);
	static GamePacket createRoomHasDestroyedPacket(
		int number, const std::string& address,
		const std::vector<std::tuple<int, std::string, std::string> >& forward_users);


	PacketFactory() {}
	~PacketFactory() {};
};

