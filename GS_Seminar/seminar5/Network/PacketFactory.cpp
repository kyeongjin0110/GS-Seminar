#include "PacketFactory.h"

using namespace std;

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

	auto joined_data = Data::CreateJoinedData(builder, fusers, appointed_data);
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

