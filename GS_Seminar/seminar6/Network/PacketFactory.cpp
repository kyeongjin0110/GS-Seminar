#include "PacketFactory.h"
using namespace std;

std::string PacketFactory::packet_name[PacketType::kSize];

void PacketFactory::setPacketNames()
{
	packet_name[kHello] = "Hello";
	packet_name[kIntro] = "Intro";
	packet_name[kJoined] = "Joined";
	packet_name[kMessage] = "Message";
	packet_name[kNotifyDisconnected] = "NotifyDisconnected";
	packet_name[kEnterPlaying] = "EnterPlaying";
	packet_name[kEnterStarting] = "EnterStarting";
	packet_name[kFull] = "Full";
	packet_name[kRequestStart] = "RequestStart";
	packet_name[kReady] = "Ready";
	packet_name[kDisconnection] = "Disconnection";
	packet_name[kCreateRoom] = "CreateRoom";
	packet_name[kRoomIntro] = "RoomIntro";
	packet_name[kJoinRoom] = "JoinRoom";
	packet_name[kRoomIsCreated] = "RoomIsCreated";
	packet_name[kRoomInfo] = "RoomInfo";
	packet_name[kClientHasJoined] = "ClientHasJoined";
	packet_name[kRoomHasDestroyed] = "RoomHasDestroyed";
	packet_name[kRequestShowRoomInfo] = "RequestShowRoomInfo";
}

GamePacket PacketFactory::createPacket(PacketType type)
{
	return GamePacket(type);
}

GamePacket PacketFactory::createHelloPacket(int id, const std::string& name)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	builder.Finish(user_data);

	return GamePacket(builder, kHello);
}

GamePacket PacketFactory::createIntroPacket(
	int id, const std::string& name,
	int appointedID, bool changed)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	auto appointed_data = Data::CreateAppointedData(builder, appointedID, changed);
	auto joined_data = Data::CreateIntroData(builder, user_data, appointed_data);
	builder.Finish(joined_data);

	return GamePacket(builder, kIntro);
}


GamePacket PacketFactory::createJoinedPacket(
	const std::vector<std::pair<int, std::string> >& users,
	int joinedID,
	int appointedID, bool changed)
{
	flatbuffers::FlatBufferBuilder builder;
	std::vector<flatbuffers::Offset<Data::UserData>> users_vec;
	for (const auto& d : users)
	{
		int id = d.first;
		string name = d.second;

		auto fname = builder.CreateString(name);
		auto user_data = Data::CreateUserData(builder, id, fname);
		users_vec.push_back(user_data);
	}
	auto fusers = builder.CreateVector(users_vec);
	auto appointed_data = Data::CreateAppointedData(builder, appointedID, changed);

	auto joined_data = Data::CreateJoinedData(builder, fusers, joinedID, appointed_data);
	builder.Finish(joined_data);

	return GamePacket(builder, kJoined);
}

GamePacket PacketFactory::createMessagePacket(
	int id, const std::string& name,
	const std::string& message)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	auto fmessage = builder.CreateString(message);
	auto message_data = Data::CreateMessageData(builder, user_data, fmessage);
	builder.Finish(message_data);

	return GamePacket(builder, kMessage);
}

GamePacket PacketFactory::createNotifyDisconnectedPacket(
	int disconnectdID,
	int appointedID, bool changed)
{
	flatbuffers::FlatBufferBuilder builder;
	auto appointed_data = Data::CreateAppointedData(builder, appointedID, changed);
	auto disconnected_data = Data::CreateDisconnectedData(builder, appointed_data, disconnectdID);
	builder.Finish(disconnected_data);

	return GamePacket(builder, kNotifyDisconnected);
}

GamePacket PacketFactory::createCreateRoomPacket(int id, const std::string& name)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	builder.Finish(user_data);

	return GamePacket(builder, kCreateRoom);
}

GamePacket PacketFactory::createRoomIntroPacket(int number, const std::string& address)
{
	flatbuffers::FlatBufferBuilder builder;
	auto faddress = builder.CreateString(address);
	auto room_data = Data::CreateRoomData(builder, number, faddress);
	builder.Finish(room_data);

	return GamePacket(builder, kRoomIntro);
}

GamePacket PacketFactory::createJoinRoomPacket(int number)
{
	flatbuffers::FlatBufferBuilder builder;
	auto faddress = builder.CreateString("");
	auto room_data = Data::CreateRoomData(builder, number, faddress);
	builder.Finish(room_data);

	return GamePacket(builder, kJoinRoom);
}

GamePacket PacketFactory::createRoomIsCreatedPacket(int number, const std::string& address)
{
	flatbuffers::FlatBufferBuilder builder;
	auto faddress = builder.CreateString(address);
	auto room_data = Data::CreateRoomData(builder, number, faddress);
	builder.Finish(room_data);

	return GamePacket(builder, kRoomIsCreated);
}

GamePacket PacketFactory::createRoomInfoPacket(const std::vector<std::pair<int, std::string> >& rooms)
{
	flatbuffers::FlatBufferBuilder builder;
	std::vector<flatbuffers::Offset<Data::RoomData>> rooms_vec;
	for (const auto& d : rooms)
	{
		int number = d.first;
		string address = d.second;

		auto faddress = builder.CreateString(address);
		auto room_data = Data::CreateRoomData(builder, number, faddress);
		rooms_vec.push_back(room_data);
	}
	auto frooms = builder.CreateVector(rooms_vec);
	auto room_data = Data::CreateRoomsData(builder, frooms);
	builder.Finish(room_data);

	return GamePacket(builder, kRoomInfo);
}

GamePacket PacketFactory::createClientHasJoinedPacket(int id, const std::string& name)
{
	flatbuffers::FlatBufferBuilder builder;
	auto fname = builder.CreateString(name);
	auto user_data = Data::CreateUserData(builder, id, fname);
	builder.Finish(user_data);

	return GamePacket(builder, kClientHasJoined);
}

GamePacket PacketFactory::createRoomHasDestroyedPacket(
	int number, const std::string& address,
	const std::vector<std::tuple<int, std::string, std::string> >& forward_users)
{
	flatbuffers::FlatBufferBuilder builder;
	auto faddress = builder.CreateString(address);
	auto room_data = Data::CreateRoomData(builder, number, faddress);

	std::vector<flatbuffers::Offset<Data::ForwardUserData>> users_vec;
	for (const auto& e : forward_users)
	{
		int id = std::get<0>(e);
		string name = std::get<1>(e);
		string address = std::get<2>(e);

		auto fname = builder.CreateString(name);
		auto faddress = builder.CreateString(address);
		auto user_data = Data::CreateUserData(builder, id, fname);

		auto forward_user_data = Data::CreateForwardUserData(builder, user_data, faddress);
		users_vec.push_back(forward_user_data);
	}

	auto forward_users_data = builder.CreateVector(users_vec);
	auto data = Data::CreateDestroyRoomData(builder, room_data, forward_users_data);
	builder.Finish(data);

	return GamePacket(builder, kRoomHasDestroyed);
}